;;; Lepton EDA netlister
;;; Copyright (C) 2016-2017 gEDA Contributors
;;; Copyright (C) 2017-2018 Lepton EDA Contributors
;;;
;;; This program is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
;;; MA 02111-1301 USA.

(define-module (netlist schematic)
  #:use-module (ice-9 receive)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-9 gnu)
  #:use-module (netlist attrib compare)
  #:use-module (netlist sort)
  #:use-module (netlist traverse)
  #:use-module (netlist package)
  #:use-module (netlist schematic-component)
  #:use-module (netlist schematic-connection)
  #:use-module (netlist package-pin)
  #:use-module (geda page)
  #:use-module (geda attrib)
  #:use-module (geda object)
  #:export-syntax (make-schematic schematic?
                   schematic-id set-schematic-id!
                   schematic-toplevel-pages set-schematic-toplevel-pages!
                   schematic-toplevel-attribs set-schematic-toplevel-attribs!
                   schematic-tree set-schematic-tree!
                   schematic-components set-schematic-components!
                   schematic-packages set-schematic-packages!
                   schematic-graphicals set-schematic-graphicals!
                   schematic-non-unique-nets set-schematic-non-unique-nets!
                   schematic-nets set-schematic-nets!
                   schematic-nc-nets set-schematic-nc-nets!
                   schematic-connections set-schematic-connections!)

  #:export (make-toplevel-schematic
            schematic-toplevel-attrib
            schematic-non-unique-package-names
            schematic-package-names))

(define-record-type <schematic>
  (make-schematic id
                  toplevel-pages
                  toplevel-attribs
                  tree
                  components
                  packages
                  graphicals
                  non-unique-nets
                  nets
                  nc-nets
                  connections)
  schematic?
  (id schematic-id set-schematic-id!)
  (toplevel-pages schematic-toplevel-pages set-schematic-toplevel-pages!)
  (toplevel-attribs schematic-toplevel-attribs set-schematic-toplevel-attribs!)
  (tree schematic-tree set-schematic-tree!)
  (components schematic-components set-schematic-components!)
  (packages schematic-packages set-schematic-packages!)
  (graphicals schematic-graphicals set-schematic-graphicals!)
  (non-unique-nets schematic-non-unique-nets set-schematic-non-unique-nets!)
  (nets schematic-nets set-schematic-nets!)
  (nc-nets schematic-nc-nets set-schematic-nc-nets!)
  (connections schematic-connections set-schematic-connections!))

(set-record-type-printer!
 <schematic>
 (lambda (record port) (format port "#<geda-schematic ~A>" (schematic-id record))))


(define next-schematic-id #f)

;;; Closure to return a new schematic id.
(let ((id 0))
  (set! next-schematic-id
        (lambda () (set! id (1+ id)) (1- id))))


;;; For now, we just make a schematic SXML tree from the list of
;;; pages given on the command line.
(define (schematic->sxml netlist toplevel-pages)
  (define (page->sxml page)
    (define (source-page source)
      (let loop ((pages (active-pages)))
        (and (not (null? pages))
             (if (string=? source (basename (page-filename (car pages))))
                 (page->sxml (car pages))
                 (loop (cdr pages))))))

    (define (composite->sxml p)
      (let ((sources (schematic-component-sources p)))
        (if sources
            `(package ,p ,@(map source-page sources))
            `(package ,p))))

    (define (non-composite->sxml p)
      `(package ,p))

    (define (component->sxml p)
      (and (string=? (basename (page-filename (object-page (schematic-component-object p))))
                     (basename (page-filename page)))
           ((if (schematic-component-sources p) composite->sxml non-composite->sxml) p)))

    `(page ,page ,@(filter-map component->sxml netlist)))


  `(*TOP* (package "TOPLEVEL"
                   ,@(map page->sxml toplevel-pages))))

;;; Gets non unique set of package refdeses.
;;; Backward compatibility procedure for legacy backends.
(define (schematic-non-unique-package-names netlist)
  (sort (filter-map schematic-component-refdes netlist) refdes<?))

;;; Returns a sorted list of unique packages in NETLIST.
;;; Backward compatibility procedure for legacy backends.
(define (schematic-package-names schematic)
  (define netlist (schematic-components schematic))
  ;; Uniqueness of packages is guaranteed by the hashtable.
  (define non-unique-packages (schematic-non-unique-package-names netlist))
  (define ht (make-hash-table (length non-unique-packages)))
  (define (get-value key value) value)
  (for-each (lambda (s) (hashq-set! ht (string->symbol s) s))
            non-unique-packages)
  (sort (hash-map->list get-value ht) refdes<?))


;;; Returns a list of all pin nets in NETLIST.
(define (get-all-nets netlist)
  (define (connected? pin)
    (let ((netname (package-pin-name pin)))
      (and (string? netname)
           (not (string-prefix? "unconnected_pin" netname))
           netname)))

  (append-map
   (lambda (package)
     (filter-map connected? (schematic-component-pins package)))
   netlist))

;;; Returns a sorted list of unique nets in NETLIST.
(define (get-nets netlist)
  (sort-remove-duplicates (get-all-nets netlist)
                          refdes<?))


(define (get-toplevel-attributes toplevel-pages)
  (define (toplevel-attrib? object)
    (and (attribute? object)
         (cons (string->symbol (attrib-name object))
               (attrib-value object))))

  (filter-map toplevel-attrib? (append-map page-contents toplevel-pages)))


;;; Returns #t if NETNAME is a "no-connect" net, that is, one of
;;; its connected symbols is one of PACKAGES and a "no-connect"
;;; symbol. Otherwise returns #f.
(define (nc-net? netname packages)
  (define (pin-netname-matches? pin)
    (string=? (package-pin-name pin) netname))

  (define (wanted-package-pin-netname=? package)
    (any pin-netname-matches? (schematic-component-pins package)))

  (any wanted-package-pin-netname=? packages))


(define (make-schematic-connections pages)
  (append-map make-page-schematic-connections pages))


(define (make-toplevel-schematic toplevel-pages netlist-mode)
  "Creates a new schematic record based on TOPLEVEL-PAGES which
must be a list of pages."
  (define (plain-package? x)
    (and (not (schematic-component-graphical? x))
         (not (schematic-component-nc? x))))

  (let* ((id (next-schematic-id))
         (toplevel-attribs (get-toplevel-attributes toplevel-pages))
         (full-netlist (traverse toplevel-pages netlist-mode))
         (netlist (filter plain-package? full-netlist))
         (packages (make-package-list netlist))
         (connections (make-schematic-connections toplevel-pages))
         (graphicals (filter schematic-component-graphical? full-netlist))
         (tree (schematic->sxml netlist toplevel-pages))
         (nu-nets (get-all-nets netlist))
         (unique-nets (get-nets netlist)))
    ;; Partition all unique net names into 'no-connection' nets
    ;; and plain nets.
    (receive (nc-nets nets)
        (partition (lambda (x)
                     (nc-net? x (filter schematic-component-nc? full-netlist)))
                   unique-nets)
      (make-schematic id
                      toplevel-pages
                      toplevel-attribs
                      tree
                      netlist
                      packages
                      graphicals
                      nu-nets
                      nets
                      nc-nets
                      connections
                      ))))

(define (schematic-toplevel-attrib schematic attrib-name)
  "Returns value of toplevel attribute ATTRIB-NAME for SCHEMATIC."
  (assq-ref (schematic-toplevel-attribs schematic) attrib-name))
