/* Lepton EDA library
 * Copyright (C) 1998, 1999, 2000 Kazu Hirata / Ales Hvezda
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2010, 2016 gEDA Contributors
 * Copyright (C) 2017-2021 Lepton EDA Contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "config.h"
#include "version.h"

#include "libgeda_priv.h"
#include "liblepton/libleptonguile.h"

/*! \brief Perform runtime initialization of liblepton library.
 *
 *  \par Function Description
 *  This function is responsible for making sure that any runtime
 *  initialization is done for all the liblepton routines. It should
 *  be called before any other liblepton functions are called.
 */
void liblepton_init(void)
{
#ifdef ENABLE_NLS
  /* Initialise gettext */
  bindtextdomain (LIBLEPTON_GETTEXT_DOMAIN, LOCALEDIR);
  bind_textdomain_codeset(LIBLEPTON_GETTEXT_DOMAIN, "UTF-8");
#endif

  eda_paths_init();

  s_clib_init();
  s_attrib_init();
  s_color_init();

  if (getenv ("LEPTON_INHIBIT_RC_FILES") == NULL) {
    g_register_libgeda_dirs ();
  }

  edascm_init ();
}



/*! \brief Prepend Lepton compiled path to Guile compiled paths env var.
 *  \note  To take effect, must be called before scm_boot_guile().
 */
void
set_guile_compiled_path()
{
  const char  varname[] = "GUILE_LOAD_COMPILED_PATH";
  const char* lepton_precompile_dir = LEPTON_SCM_PRECOMPILE_DIR;
  const char* value = getenv (varname);

  if (value == NULL || strlen (value) == 0)
  {
    setenv (varname, lepton_precompile_dir, 1);
  }
  else
  {
    const size_t len = strlen (lepton_precompile_dir)
                       + 1  /* ":" */
                       + strlen (value)
                       + 1; /* \0  */
    char* buffer = (char*) malloc (len);

    strcpy (buffer, lepton_precompile_dir);
    strcat (buffer, ":");
    strcat (buffer, value);

    setenv (varname, buffer, 1);

    free (buffer);
  }

} /* set_guile_compiled_path() */
