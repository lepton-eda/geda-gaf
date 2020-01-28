;;; Lepton EDA library - Scheme API
;;; Copyright (C) 2016 gEDA Contributors
;;; Copyright (C) 2019-2020 Lepton EDA Contributors
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

;;; Source library is a list of directories to search for source
;;; files by their basenames. The directories are searched for
;;; files recursively. First found file with a given basename is
;;; returned.
;;; If any given directory is not readable, error is returned.
;;; If there are several files with the same given basename,
;;; a warning is output that some of those files won't be used.

(define-module (lepton library)
  #:use-module (geda core gettext)

  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (ice-9 ftw)
  #:use-module (ice-9 match)
  #:use-module (lepton file-system)
  #:use-module (lepton library component)
  #:use-module (lepton log)
  #:use-module (lepton os)

  #:export (%default-source-library
            ;; deprecated
            source-library
            source-library-search
            reset-source-library
            ;; new
            source-library-contents
            set-source-library-contents!
            ;; temporary
            get-source-library-file)

  #:re-export (component-library
               component-library-search
               component-library-command
               component-library-funcs
               reset-component-library))


(define-record-type <source-library>
  (make-source-library contents)
  source-library?
  (contents library-contents set-library-contents!))

;;; Default source library. Used for support of legacy source
;;; library procedures.
(define %default-source-library (make-source-library '()))

(define (source-library-contents lib)
  "Returns the contents of given source library."
  (library-contents lib))

(define (set-source-library-contents! lib contents)
  "Sets the contents of given source library into the given value."
  (set-library-contents! lib contents))

;;; Prepends PATH to the contents of the source library LIB.
(define (source-library-prepend! lib path)
  (set-library-contents! lib
                         (cons path (library-contents lib))))

#|
;;; Appends PATH to the contents of the source library LIB.
(define (source-library-append! lib path)
  (set-library-contents! lib
                         (append (library-contents lib)
                                 (list path))))
|#

(define (source-library path)
  "Prepends the contents of given path to the default source
library. This procedure is legacy and should be avoided in new
code. Use set-library-contents! instead."
  (if (string? path)
      ;; take care of any shell variables
      (let ((expanded-path (expand-env-variables path)))
        (if (file-readable? expanded-path)
            (source-library-prepend! %default-source-library
                                     (if (absolute-file-name? expanded-path)
                                         expanded-path
                                         (string-append (getcwd)
                                                        file-name-separator-string
                                                        expanded-path)))
            (and
             (log! 'critical (_ "Invalid path ~S or source not readable.\n") path)
             #f)))
      (and (log! 'critical (_ "Source library path must be a string.\n")) #f)))


(define (reset-source-library)
  "Resets source library, that is, sets its contents to '(). This
procedure is legacy and should be avoided in new code. Use
set-library-contents! instead."
  (set-library-contents! %default-source-library '()))


;;; Transforms the tree of directories into a plain list of paths,
;;; filtering out plain files and some VCS related directories.
(define get-tree
  (match-lambda
    ((name stat)                        ; flat file
     #f)
    ((name stat children ...)           ; directory
     (and (not (member name '(".git" ".svn" "CVS")))
          (let ((contents (filter-map get-tree children)))
            (if (null? contents)
                (list name)
                (cons name
                 (map (lambda (x) (string-append name file-name-separator-string x))
                      (append-map identity contents)))))))))

(define (source-library-search path)
  "Recursively prepends the contents of given path to the default
source library. This procedure is legacy and should be avoided in
new code. Use set-library-contents! instead."
  (map (lambda (x) (source-library
               (string-append path file-name-separator-string x)))
       (get-tree (file-system-tree path))))


;;; This is a temporary procedure for hierarchy traversing support
;;; and its use should be avoided.
(define (get-source-library-file base-name)
  "Searches in default source library for a file with given
basename and returns its full name. Returns #f if no file has
been found."
  (define get-files
    (match-lambda
      ((name stat)                      ; flat file
       name)
      (_ #f)))
  (define get-dir-files
    (match-lambda ((name stat children ...)
                   (filter-map get-files children))))

  (let loop ((paths (source-library-contents
                     %default-source-library)))
    (and (not (null? paths))
         (if (member base-name (get-dir-files (file-system-tree (car paths))))
             (let ((full-name (string-append (car paths)
                                             file-name-separator-string
                                             base-name)))
               (if (file-readable? full-name)
                   full-name
                   (begin
                     (log! 'critical (_ "File ~S is not readable.\n")
                           full-name)
                     (loop (cdr paths)))))
             (loop (cdr paths))))))
