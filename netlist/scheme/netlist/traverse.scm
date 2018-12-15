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

(define-module (netlist traverse)

  ; Import C procedures and variables
  #:use-module (netlist core gettext)

  #:use-module ((ice-9 match))
  #:use-module ((ice-9 rdelim)
                #:select (read-string)
                #:prefix rdelim:)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-26)
  #:use-module (geda attrib)
  #:use-module (geda object)
  #:use-module (geda library)
  #:use-module (geda log)
  #:use-module (geda page)
  #:use-module (netlist config)
  #:use-module (netlist hierarchy)
  #:use-module (netlist rename)
  #:use-module (netlist net)
  #:use-module (netlist option)
  #:use-module (netlist package-pin)
  #:use-module (netlist pin-net)
  #:use-module (netlist schematic-component)
  #:use-module (netlist schematic-connection)
  #:use-module (netlist verbose)
  #:use-module (symbol check net-attrib)

  #:export (traverse
            file->page))


;;; Tracks which objects have been visited so far, and how many
;;; times.
(define %visits '())
;;; Increment the current visit count for a particular OBJECT.
(define (visit! object)
  (set! %visits (cons object %visits)))
;;; Retrieve the current visit count for a particular OBJECT.
(define (visited? object)
  (member object %visits))
;;; Reset all visit counts. Simply clears the %visits completely.
(define (clear-visits!)
  (set! %visits '()))


(define (traverse-net current-nets starting object tag)
  ;; We don't support other object types apart from net-pins and nets yet.
  (define (connection-type object)
    (or (net-pin? object)
        (net? object)
        (and (log! 'critical
                   (_ "Traverse nets: object type ~A is not supported.")
                   (object-type object))
             #f)))

  (define (check-create-refdes refdes pinnumber)
    (if pinnumber
        refdes
        ;; No pinnumber, broken pin, use "?", but probably refdes exists.
        (if refdes
            (begin
              (log! 'critical (_ "Missing pinnumber= for refdes=~A)") refdes)
              refdes)
            (begin
              (log! 'critical (_ "Missing attributes refdes= and pinnumber="))
              "U?"))))

  (define (make-new-net object)
    (if (pin? object)
        (let* ((pinnumber (attrib-value-by-name object "pinnumber"))
               (refdes (attrib-value-by-name (object-component object) "refdes"))
               ;; If refdes= of pin component exists, or there is
               ;; no refdes but pinnumber= exists (which means the
               ;; pin exists too), we consider the pin to be
               ;; normal (in the latter case, the symbol is
               ;; special, like "gnd-1.sym"). Otherwise we believe
               ;; the pin was created from net= attribute.
               (net-attrib-net? (not (or refdes (not pinnumber)))))
          (if net-attrib-net?
              (make-pin-net
                (object-id object)
                #t
                ;; Use hierarchy tag here to make this net unique.
                (create-netattrib (netattrib-search-net (object-component object)
                                                         pinnumber)
                                   tag)
                #f
                #f)
              (make-pin-net
               (object-id object)
                #f
                #f
                (hierarchy-create-refdes (check-create-refdes refdes
                                                               pinnumber)
                                          tag)
                (if refdes pinnumber (or pinnumber "?")))))
        (make-pin-net
         (object-id object)
          #f
          (create-net-netname object tag)
          #f
          #f)))

  (when starting
    (clear-visits!))

  (visit! object)
  (if (connection-type object)
      (let ((nets (cons (make-new-net object) current-nets)))
        (if (or (not (pin? object))
                starting)
            (let loop ((connections (object-connections object))
                       (nets nets))
              (if (null? connections)
                  nets
                  (loop (cdr connections)
                        (let ((conn (car connections)))
                          (if (visited? conn)
                              nets
                              (traverse-net nets #f conn tag))))))
            nets))
      current-nets))


(define %unnamed-net-counter 0)
(define (increment-unnamed-net-counter)
  (set! %unnamed-net-counter (1+ %unnamed-net-counter))
  %unnamed-net-counter)


(define (create-unnamed-netname tag netlist-mode)
  (define (hierarchical-default-name s)
    (create-netname (string-append (gnetlist-config-ref 'default-net-name) s)
                    tag))
  ((if (eq? netlist-mode 'spice) identity hierarchical-default-name)
   (number->string (increment-unnamed-net-counter))))


(define %unnamed-pin-counter 0)
(define (increment-unnamed-pin-counter)
  (set! %unnamed-pin-counter (1+ %unnamed-pin-counter))
  %unnamed-pin-counter)


(define (create-unconnected-netname)
  (string-append "unconnected_pin-"
                 (number->string (increment-unnamed-pin-counter))))

(define %netnames (make-hash-table))

(define (search-in-hash-table nets)
    (and (not (null? nets))
         (or (hash-ref %netnames (pin-net-id (car nets)))
             (search-in-hash-table (cdr nets)))))

(define (get-package-pin-connection pin-object connections)
  (let loop ((groups connections))
    (and (not (null? groups))
         (let ((group (car groups)))
           (or (and (member pin-object (schematic-connection-objects group)) group)
               (loop (cdr groups)))))))


(define (object-pins object tag netlist-mode connections)
  (define (make-pin-attrib-list object)
    (define (add-attrib attrib)
      (cons (string->symbol (attrib-name attrib))
            (attrib-value attrib)))

    (map add-attrib (object-attribs object)))

  (define (make-special-netname nets)
    (if (null? nets)
        (create-unconnected-netname)
        (create-unnamed-netname tag netlist-mode)))

  (define (object->package-pin object)
    (and (net-pin? object)
         (let* ((attribs (make-pin-attrib-list object))
                (nets (if (null? (object-connections object))
                          '()
                          (reverse (traverse-net '() #t object tag))))
                (netname (or
                          ;; If there is no netname, probably
                          ;; some of nets has been already named.
                          (search-net-name nets)
                          ;; Didn't find a name.  Go looking for
                          ;; another net which might have already
                          ;; been named, i.e. we don't want to
                          ;; create a new unnamed net if the net
                          ;; has already been named before.
                          (search-in-hash-table nets)
                          ;; Last resort. We have not found a
                          ;; name. Make a new one.
                          (make-special-netname nets))))
           (and netname
                (for-each
                 (lambda (net) (hash-set! %netnames (pin-net-id net) netname))
                 nets))
           (make-package-pin (object-id object)
                             object
                             'net-pin
                             (assq-ref attribs 'pinnumber)
                             netname
                             (assq-ref attribs 'pinlabel)
                             attribs
                             nets
                             (get-package-pin-connection object connections)))))

  (filter-map object->package-pin (component-contents object)))


;;; Searches for pinnumers in NET-MAPS and, if found, updates
;;; corresponding pins in PIN-LIST, otherwise creates new pins and
;;; adds them to the list.  ID, REFDES, and hierarchy TAG are used
;;; to create hierarchical net name.
(define (net-maps->pins net-maps id refdes tag pin-list)
  (define (pinnumber->pin pinnumber pin-list)
    (and (not (null? pin-list))
         (if (string=? (package-pin-number (car pin-list)) pinnumber)
             (car pin-list)
             (pinnumber->pin pinnumber (cdr pin-list)))))

  (define (check-shorted-nets a b priority)
    (log! 'critical
          (_ "Rename shorted nets (~A= has priority): ~A -> ~A")
          priority
          a
          b)
    (add-net-rename a b))

  (define (unnamed-net-or-unconnected-pin? name)
    (or (string-prefix? "unnamed_net" name)
        (string-prefix? "unconnected_pin" name)))

  (define (update-pin-netname pin netname id refdes)
    (let ((nets (package-pin-nets pin))
          (pinnumber (package-pin-number pin))
          (net-priority #t))
      (set-package-pin-name! pin netname)
      (if (null? nets)
          (set-package-pin-nets! pin
                                 (list (make-pin-net id
                                                     net-priority
                                                     netname
                                                     refdes
                                                     pinnumber)))
          (let ((net (car nets)))
            (set-pin-net-id! net id)
            (set-pin-net-priority! net net-priority)
            (set-pin-net-name! net netname)
            (set-pin-net-connection-package! net refdes)
            (set-pin-net-connection-pinnumber! net pinnumber)))))

  (define (update-pin pin net-map id refdes tag)
    (and refdes
         (let ((netname (create-netattrib (net-map-netname net-map) tag))
               (pin-netname (package-pin-name pin)))
           (if (and pin-netname
                    (not (unnamed-net-or-unconnected-pin? pin-netname)))
               (if (gnetlist-config-ref 'netname-attribute-priority)
                   (check-shorted-nets netname pin-netname 'netname)
                   (check-shorted-nets pin-netname netname 'net))
               (begin
                 (when (unnamed-net-or-unconnected-pin? pin-netname)
                   ;; Rename unconnected pins and unnamed nets.
                   (add-net-rename pin-netname netname))
                 (update-pin-netname pin netname id refdes))))
         #f))

  (define (make-net-map-pin net-map id refdes tag)
    (let* ((pinnumber (net-map-pinnumber net-map))
           (netname (create-netattrib (net-map-netname net-map) tag))
           (net-priority #t)
           (label #f)
           (object #f)
           (attribs '())
           (nets (list (make-pin-net id net-priority netname refdes pinnumber))))
      (make-package-pin id object 'net pinnumber netname label attribs nets #f)))

  (define (add-net-power-pin-override pin net-map tag)
    (define (power-pin? pin)
      (string=? "pwr" (assq-ref (package-pin-attribs pin) 'pintype)))

    (let ((connection (package-pin-connection pin))
          (name (create-netattrib (net-map-netname net-map) tag)))
      (when (power-pin? pin)
        (set-schematic-connection-override-name!
         connection
         (match (schematic-connection-override-name connection)
           ((? list? x) `(,name . ,x))
           (#f name)
           (x `(,name ,x)))))))

  (append pin-list
          (filter-map
           (lambda (net-map)
             (let ((pin (pinnumber->pin (net-map-pinnumber net-map) pin-list)))
               (if pin
                   (begin
                     (add-net-power-pin-override pin net-map tag)
                     (update-pin pin net-map id refdes tag))
                   (make-net-map-pin net-map id refdes tag))))
           net-maps)))


(define (get-sources graphical? inherited-attribs attached-attribs)
  (define (non-null* ls)
    (and (not (null? ls)) ls))

  ;; Given a list of strings, some of which may contain commas,
  ;; splits comma separated strings and returns the new combined
  ;; list
  (define (comma-separated->list ls)
    (append-map (lambda (s) (string-split s #\,)) ls))

  (and (not graphical?)
       (gnetlist-config-ref 'traverse-hierarchy)
       (let ((sources
              (or (non-null* (assq-ref attached-attribs 'source))
                  (non-null* (assq-ref inherited-attribs 'source)))))
         (and=> sources comma-separated->list))))

(define quiet-mode (gnetlist-option-ref 'quiet))

;;; Reads file NAME and outputs a page named NAME
(define (file->page name)
  (with-input-from-file name
    (lambda ()
      (when (not quiet-mode)
        (log! 'message (_ "Loading schematic ~S\n") name))
      (string->page name (rdelim:read-string)))))


(define (hierarchy-down-schematic name)
  (let ((filename (get-source-library-file name)))
    (if filename
        (file->page filename)
        (log! 'error (_ "Failed to load subcircuit ~S.") name))))

(define (traverse-page page hierarchy-tag netlist-mode)
  ;; Get refdes= of OBJECT depending on NETLIST-MODE.
  (define (get-refdes attribs netlist-mode)
    (let ((refdes (and=> (assq-ref attribs 'refdes) car)))
      (case netlist-mode
        ((spice)
         (let ((slot (and=> (assq-ref attribs 'slot) car)))
           (if slot
               (string-append refdes "." slot)
               refdes)))
        ((geda) refdes)
        (else (error (_ "Netlist mode ~S is not supported.") netlist-mode)))))

  ;; Makes attribute list of OBJECT using getter GET-ATTRIBS.
  (define (make-attrib-list get-attribs object)
    (define (add-attrib ls attrib)
      (let* ((name (string->symbol (attrib-name attrib)))
             (prev-value (assq-ref ls name))
             (new-value (attrib-value attrib)))
        (if prev-value
            (assq-set! ls name (cons new-value prev-value))
            (acons name (list new-value) ls))))

    (let loop ((in (get-attribs object))
               (out '()))
      (if (null? in)
          out
          (loop (cdr in)
                (add-attrib out (car in))))))

  (define (refdes-by-net object net-maps graphical)
    ;; If there is net=, it's a power or some other special symbol.
    (and (null? net-maps)
         ;; Do not bother traversing the hierarchy if the symbol has an
         ;; graphical attribute attached to it.
         (not graphical)
         (log! 'critical
               (_ "\nNon-graphical symbol ~S\nat ~A on page ~S\nhas neither refdes= nor net=.")
               (component-basename object)
               (component-position object)
               (page-filename (object-page object)))
         "U?"))

  (define (traverse-object object connections)
    (let* ((id (object-id object))
           (inherited-attribs (make-attrib-list inherited-attribs object))
           (attached-attribs (make-attrib-list object-attribs object))
           (net-maps (check-net-maps object))
           (package (make-schematic-component id
                                              #f   ; get refdes later
                                              hierarchy-tag
                                              #f   ; get sources later
                                              object
                                              inherited-attribs
                                              attached-attribs
                                              '())) ; get pins later
           (graphical (or (schematic-component-graphical? package)
                          (schematic-component-nc? package)))
           (refdes  (hierarchy-create-refdes (or (get-refdes attached-attribs netlist-mode)
                                                 (refdes-by-net object net-maps graphical))
                                             hierarchy-tag))
           (sources (get-sources graphical
                                 inherited-attribs
                                 attached-attribs))
           (pins (net-maps->pins net-maps
                                 id
                                 refdes
                                 hierarchy-tag
                                 (object-pins object hierarchy-tag netlist-mode connections))))
      (set-schematic-component-refdes! package refdes)
      (set-schematic-component-sources! package sources)
      (set-schematic-component-pins! package pins)
      package))

  (when hierarchy-tag
    (log! 'message (_ "Going to traverse source ~S") (page-filename page)))

  (let* ((objects (page-contents page))
         (components (filter component? objects))
         (connections (make-page-schematic-connections page))
         (schematic-components (map (cut traverse-object <> connections) components)))
    schematic-components))

;;; Traverses pages obtained from files defined in the 'source='
;;; attributes of COMPONENT with respect to HIERARCHY-TAG and
;;; NETLIST-MODE.
(define (traverse-component-sources component hierarchy-tag netlist-mode)
  (let ((hierarchy-tag (schematic-component-refdes component))
        (source-pages (map hierarchy-down-schematic
                           (schematic-component-sources component))))
    (traverse-pages source-pages hierarchy-tag netlist-mode)))

(define (traverse-pages pages hierarchy-tag netlist-mode)
  (let* ((schematic-components (append-map (cut traverse-page <> hierarchy-tag netlist-mode) pages))
         (composites (filter schematic-component-sources schematic-components))
         ;; Traverse underlying schematics.
         (underlying-components (append-map (cut traverse-component-sources
                                                 <>
                                                 hierarchy-tag
                                                 netlist-mode)
                                            composites)))
    (append schematic-components underlying-components)))

(define (traverse toplevel-pages netlist-mode)
  (reset-rename!)
  (rename-all (hierarchy-post-process (traverse-pages toplevel-pages
                                                      #f ; toplevel hierarchy tag
                                                      netlist-mode))))
