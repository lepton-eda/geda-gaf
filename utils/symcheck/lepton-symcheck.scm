#!/usr/bin/env sh
export GUILE_LOAD_COMPILED_PATH="@ccachedir@:${GUILE_LOAD_COMPILED_PATH}"
exec @GUILE@ -s "$0" "$@"
!#
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
;;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

(load-extension (or (getenv "LIBLEPTON") "@libdir@/liblepton")
                "liblepton_init")

;;; Localization.
(define %textdomain "lepton-symcheck")
(textdomain %textdomain)
(bindtextdomain %textdomain "@localedir@")
(bind-textdomain-codeset %textdomain "UTF-8")

;;; At compile time of this program guile won't be aware of these
;;; modules, since it compiles the code before loading the above
;;; extension. Let's make it quiet here.
(define with-toplevel (@@ (lepton core toplevel) %with-toplevel))
(define make-toplevel (@@ (lepton core toplevel) %make-toplevel))

(primitive-eval '(use-modules (symcheck check)))

(with-toplevel (make-toplevel) check-all-symbols)
