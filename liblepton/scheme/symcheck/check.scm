;;; Lepton EDA Symbol Checker
;;; Scheme API
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

(define-module (symcheck check)
  #:use-module ((ice-9 rdelim)
                #:select (read-string)
                #:prefix rdelim:)

  #:use-module (lepton core gettext)
  #:use-module (lepton log)
  #:use-module (lepton page)
  #:use-module (lepton repl)
  #:use-module (lepton version)
  #:use-module (symcheck option)
  #:use-module (symcheck report)
  #:use-module (symbol check)
  #:use-module (symbol check log)

  #:export (check-all-symbols))

(define (usage)
  (format #t
          (_ "Usage: ~A [OPTIONS] FILE ...

Check one or more Lepton EDA symbol FILEs.

General options:
  -h, --help        Print usage
  -V, --version     Show version information
  -q, --quiet       Quiet mode
  -v, --verbose     Verbose mode (cumulative, i.e. -v will show error
                    messages, -vv will show errors and warnings, and
                    -vvv displays also informational messages)

Report bugs at <~A>
Lepton EDA homepage: <~A>
")
          (car (program-arguments))
          (lepton-version 'bugs)
          (lepton-version 'url))
  (primitive-exit 0))


(define (lepton-symcheck-version)
  (format #t "~a~%" (lepton-version 'msg))
  (primitive-exit 0))


;;; Reads file NAME and outputs a page named NAME
(define (file->page name)
  (with-input-from-file name
    (lambda ()
      (unless (symcheck-option-ref 'quiet)
        (log! 'message (_ "Loading schematic ~S") name))
      (string->page name (rdelim:read-string)))))


(define (check-all-symbols)
  (define (report-symbol-statistics page)
    (unless (symcheck-option-ref 'quiet)
      (check-log! 'message (_ "Checking: ~A\n") (page-filename page)))
    (check-symbol page)
    (check-report `(,page . ,(page-contents page))))

  (define (error-no-files-specified)
    (format #t
            (_ "No schematic files specified for processing.
Run `~A --help' for more information.\n")
            (car (program-arguments)))
    (primitive-exit 1))

  ;; Symcheck logs to stdout by default.
  (set-check-log-destination! 'stdout)

  (let ((files (symcheck-option-ref '()))
        (help (symcheck-option-ref 'help))
        (version (symcheck-option-ref 'version))
        (interactive (symcheck-option-ref 'interactive)))
    (if version
        (lepton-symcheck-version))
    (if help
        (usage))

        (let ((pages (map file->page files)))
          (if interactive
              ;; Interactive mode. Just run the REPL to work with
              ;; schematic pages.
              (lepton-repl)
              ;; Non-interactive mode.
              (if (null? pages)
                  (error-no-files-specified)
                  ;; now report the info/warnings/errors to the user
                  (primitive-exit (apply + (map report-symbol-statistics pages))))))))
