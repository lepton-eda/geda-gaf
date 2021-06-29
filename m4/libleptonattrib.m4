# libleptonattrib.m4                                     -*-Autoconf-*-
# serial 1

dnl libleptonattrib-specific setup
dnl Copyright (C) 2020 Lepton EDA Contributors
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

# Work out the gettext domain that libgeda should use
AC_DEFUN([AX_LIBLEPTONATTRIB],
[
  AC_PREREQ([2.60])dnl

  # First argument is the shared library version to use.
  AC_MSG_CHECKING([libleptonattrib shared library version])
  AC_MSG_RESULT($1)
  AC_SUBST([LIBLEPTONATTRIB_SHLIB_VERSION], $1)

  VER_MAJOR=`echo ${LIBLEPTONATTRIB_SHLIB_VERSION} | sed 's;:.*;;g'`
  AC_SUBST([LIBLEPTONATTRIB_MAJOR], $VER_MAJOR)
])
