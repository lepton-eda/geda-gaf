;; Lepton EDA Schematic Capture
;; Scheme API
;; Copyright (C) 2017 Lepton EDA Contributors
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
;;

(define-module (gschem symbol check)
  #:use-module (srfi srfi-1)
  #:use-module (gschem core gettext)
  #:use-module (gschem window)
  #:use-module (geda page)
  #:use-module (geda core toplevel)
  #:use-module (symbol blame)
  #:use-module ((symbol check) #:prefix sym:))

(define-public (check-symbol)
  "Checks the active page which should be a symbol, and returns
its blamed objects."
  (define (warning-or-error blame)
    (or (eq? 'error (car blame)) (eq? 'warning (car blame))))

  (define (blamed-object? object)
    (not (null? (filter warning-or-error (object-blames object)))))

  (let ((page (active-page)))
    (sym:check-symbol page)
    (let ((page-info (object-blaming-info page)))
      ((@@ (guile-user) gschem-msg) (if (string-null? page-info)
                                        (_ "Symbol has no pin info.")
                                        page-info)))
    (filter blamed-object? (page-contents page))))

(define-public (object-blaming-info object)
  (string-join (map cdr (object-blames object)) "\n" 'suffix))
