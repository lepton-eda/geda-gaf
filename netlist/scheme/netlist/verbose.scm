;;; Lepton EDA netlister
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

(define-module (netlist verbose)
  #:use-module (netlist option)
  #:use-module (netlist package)
  #:use-module (netlist package-pin)
  #:use-module (netlist pin-net)

  #:export (verbose-print-netlist))

(define (verbose-print-netlist netlist)
  (define (print-net net)
    (let ((package (pin-net-connection-package net))
          (pinnumber (pin-net-connection-pinnumber net)))
      (if (and package pinnumber)
          (format #f "\t\t~A ~A [~A]\n"
                  package
                  pinnumber
                  (pin-net-id net))
          "")))

  (define (print-nets net-list)
    (string-join (map print-net net-list) ""))

  (define (print-pin-info pin)
    (format #f "\tpin~A (~A) ~A\n~A\n"
            (or (package-pin-number pin) "?")
            (or (package-pin-label pin) "")
            (or (package-pin-name pin) "Null net name")
            (print-nets (package-pin-nets pin))))

  (define (print-pin-list pin-list)
    (map print-pin-info pin-list))

  (define (print-package-info package)
    (format #f "component ~S\n~
                Hierarchy tag: ~S\n~
                ~A\n"
            (or (package-refdes package) "SPECIAL")
            (or (package-tag package) "")
            (print-pin-list (package-pins package))))

   (format #t "\nInternal netlist representation:\n\n~
               ~A\n"
           (string-join (map print-package-info netlist) "")))
