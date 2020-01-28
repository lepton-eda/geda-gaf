;;; Lepton EDA library - Scheme API
;;; Copyright (C) 2010 Peter Brett <peter@peter-b.co.uk>
;;; Copyright (C) 2010-2017 gEDA Contributors
;;; Copyright (C) 2017-2020 Lepton EDA Contributors
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
;;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA


(define-module (lepton page)

  ;; Import C procedures
  #:use-module (geda core gettext)
  #:use-module (geda core smob)
  #:use-module (geda core page)

  #:use-module (ice-9 optargs)
  #:use-module ((ice-9 rdelim)
                #:select (read-string)
                #:prefix rdelim:)

  #:use-module (lepton os)

  #:export (file->page))

(define-public object-page %object-page)

(define-public page? %page?)
(define-public active-pages %active-pages)
(define-public make-page %new-page)
(define-public close-page! %close-page!)
(define-public page-filename %page-filename)
(define-public set-page-filename! %set-page-filename!)
(define-public page-contents %page-contents)
(define-public page-dirty? %page-dirty?)
(define-public page->string %page->string)
(define-public string->page %string->page)

(define-public (page-append! P . objects)
  (for-each (lambda (x) (%page-append! P x)) objects)
  P)

(define-public (page-remove! P . objects)
  (for-each (lambda (x) (%page-remove! P x)) objects)
  P)

(define*-public (set-page-dirty! page #:optional (state #t))
  (%set-page-dirty! page state))


;;; Reads file FILENAME and outputs a page with the same name.
(define (file-contents->page filename)
  (with-input-from-file filename
    (lambda ()
      (string->page filename (rdelim:read-string)))))


;;; Returns an opened page from PAGES by FILENAME. If no
;;; corresponding page found, returns #f.
(define (page-by-filename filename pages)
  (and (not (null? pages))
       (let ((page (car pages)))
         (if (string= filename (page-filename page))
             page
             (page-by-filename filename (cdr pages))))))


(define* (file->page filename #:optional new-page?)
  "Given FILENAME, returns an opened page for it, or a new page if
none exists. Optional argument NEW-PAGE? can be used to force
creation of a new page for given filename."
  (let ((filename (expand-env-variables filename)))
    (if new-page?
        (file-contents->page filename)
        (or (page-by-filename filename (active-pages))
            (file-contents->page filename)))))
