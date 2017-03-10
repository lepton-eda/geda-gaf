/* gEDA - GPL Electronic Design Automation
 * libgeda - gEDA's library
 * Copyright (C) 2016 Peter Brett <peter@peter-b.co.uk>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */

#include <config.h>

#if !defined(G_OS_WIN32)
#	include <libgen.h>
#endif

#include "libgeda_priv.h"

/* For convenience in this file only! */
#if !defined(GEDADATADIR)
#	define GEDADATADIR NULL
#endif
#if !defined(GEDARCDIR)
#	define GEDARCDIR GEDADATADIR
#endif

#ifdef __cplusplus
static gchar const DATA_ENV[] = "GEDADATA";
static gchar const CONFIG_ENV[] = "GEDADATARC";
static gchar const DATA_XDG_SUBDIR[] = "gEDA";
static gchar const DATA_GUESS_FILE[] = "scheme/geda.scm";
static gchar const USER_DOTDIR[] = ".gEDA";
#else
static const gchar const DATA_ENV[] = "GEDADATA";
static const gchar const CONFIG_ENV[] = "GEDADATARC";
static const gchar const DATA_XDG_SUBDIR[] = "gEDA";
static const gchar const DATA_GUESS_FILE[] = "scheme/geda.scm";
static const gchar const USER_DOTDIR[] = ".gEDA";
#endif

/* ================================================================
 * Private initialisation functions
 * ================================================================ */

/*! \brief Get the gEDA installation's data directory.
 * Attempt to find the "share/gEDA" directory in the prefix where gEDA
 * was installed.  Returns NULL if not on Windows and not a
 * relocatable build (in that case, the install location should have
 * been compiled in via configure options) */
static const gchar *
guess_install_data_dir(void)
{
	static const gchar *install_dir = NULL;
	static gsize is_init = 0;
	if (g_once_init_enter(&is_init)) {
		gchar *tmp_dir = NULL;

#if defined(G_OS_WIN32)
		/* The installation data directory should be the last element in
		 *  g_get_system_data_dirs(). */
		static const gchar * const *sys_dirs;
		for (gint i = 0; sys_dirs[i]; ++i, tmp_dir = sys_dirs[i]);

#elif defined(ENABLE_RELOCATABLE)
		/* Look at /proc/self/exe, if it exists */
		if (g_file_test("/proc/self/exe", G_FILE_TEST_IS_SYMLINK)) {
			gchar *bin = canonicalize_file_name("/proc/self/exe");
			gchar *prefix = dirname(dirname(bin));
			tmp_dir = g_build_filename(prefix, "share/gEDA", NULL);
			free(bin);
		}
#endif

		/* Check that the directory actually exists */
		if (tmp_dir && !g_file_test(tmp_dir, G_FILE_TEST_IS_DIR)) {
			g_free(tmp_dir);
		} else {
			install_dir = tmp_dir;
		}

		g_once_init_leave(&is_init, 1);
	}
	return install_dir;
}

/*! \brief Copy search paths into result structure.
 * Helper function that duplicated search paths from the various
 * sources into an output buffer and returns the number of paths.  If
 * the output pointer is NULL, doesn't copy, but still returns the
 * number of paths.  Only exists for the use of
 * build_search_list(). */
static gsize
copy_search_list(const gchar **output,
                 const gchar * const * env_names,
                 const gchar * const * xdg_dirs,
                 const gchar * const * cfg_dirs)
{
	/* Copy in the entries.  Duplicate everything so the result is fully
	 * owned by us. */
	gsize copied = 0;

#if defined(ENABLE_DEPRECATED)
	/* If one of the environment variables is set, it takes precedence. */
	for (gsize i = 0; env_names && env_names[i]; ++i) {
		const gchar *env_value = g_getenv(env_names[i]);
		if (!env_value) continue;

		if (output) {
			output[0] = g_strdup(env_value);
			output[1] = NULL;
		}
		/* The first environment variable found overrides all. */
		return 2;
	}
#endif /* ENABLE_DEPRECATED */

	/* Otherwise, use the "gEDA" subdirectory of the configured standard
	 * directories. */
	for (gsize i = 0; xdg_dirs && xdg_dirs[i]; ++i) {
		/* Append "gEDA" to each XDG data path */
		if (output) {
			output[copied] =
				g_build_filename(xdg_dirs[i],
				                 DATA_XDG_SUBDIR, NULL);
		}
		++copied;
	}

#if !defined(G_OS_WIN32) && !defined(ENABLE_RELOCATABLE)
	/* Append the configured variables to the list */
	for (gsize i = 0; cfg_dirs && cfg_dirs[i]; ++i) {
		if (output) {
			output[copied] = g_strdup(cfg_dirs[i]);
		}
		++copied;
	}
#endif

	/* Append the guessed install directory to the list, if it's not in
	 * there already. */
	const gchar *install_dir = guess_install_data_dir();
	if (install_dir) {
		if (output) {
			/* Only append to search path if not already an element.
			 * N.b. we could use g_strv_contains() here, but it's only
			 * available in quite recent versions of GLib.*/
			gboolean found = FALSE;
			for (gint i = 0; !found && i < copied; ++i) {
				found = (0 == strcmp(install_dir, output[i]));
			}

			if (!found) {
				output[copied] = g_strdup(install_dir);
				++copied;
			}
		} else {
			/* Allow space just in case */
			++copied;
		}
	}

	if (output) output[copied] = NULL;
	++copied;

	return copied;
}

/*! \brief Build a set of search paths.
 * Build a null-terminated array of constant strings, using the input
 * parameters.
 *
 * \param env_names NULL-terminated list of environment variable
 *                  names.  If any of them is set, it is the only item
 *                  in the returned list.
 * \param xdg_dirs  NULL-terminated list of directory names.  All of
 *                  them have gEDA's XDG subdirectory (i.e. "gEDA")
 *                  appended to them and are added to the list in
 *                  order.
 * \param cfg_dirs NULL terminated list of additional directories to
 *                 append to the list.
 * \return a newly-allocated, NULL-terminated array of strings.
 */
static const gchar **
build_search_list(const gchar * const * env_names,
                  const gchar * const * xdg_dirs,
                  const gchar * const * cfg_dirs)
{
	
	gsize count = 0;
	const gchar **result = NULL;

	while (TRUE) {
		/* The first time copy_search_list() is called, it returns the
		 * length of buffer required. */
		count = copy_search_list(result, env_names, xdg_dirs, cfg_dirs);

		if (result) break;
		result = g_new0(const gchar *, count);
	}

	return result;
}

/* \brief Initialise backwards-compatible environment variables.
 * Ensure that the $GEDADATA environment variable is always set to
 * something sensible within gEDA programs.
 */
static void
eda_paths_init_env(void)
{
#if defined(ENABLE_DEPRECATED)
	static gsize is_init = 0;

	/* If $GEDADATA is not set, find a directory containing
	 * scheme/geda.scm and set $GEDADATA to it.  Note that this
	 * *doesn't* use guess_install_data_dir() because the user might
	 * have deliberately set $XDG_DATA_DIRS to put other directories
	 * ahead of the installation data directory. */
	if (g_once_init_enter(&is_init)) {

		gboolean found = FALSE;
		const gchar * const *dirs = eda_get_system_data_dirs();

		for (int i = 0; dirs[i]; ++i) {

			gchar *guess_file =
				g_build_filename(dirs[i], DATA_GUESS_FILE, NULL);
			found = g_file_test(guess_file, G_FILE_TEST_IS_REGULAR);
			g_free(guess_file);

			if (!found) continue;

			g_setenv(DATA_ENV, dirs[i], FALSE);
			break;
		}

		gsize setup = 1;
		g_once_init_leave(&is_init, setup);
	}
#endif /* ENABLE_DEPRECATED */
}

static gchar *
get_user_dotdir(void)
{
#if defined(ENABLE_DEPRECATED)
	gchar *dotdir = g_build_filename(g_get_home_dir(),
	                                 USER_DOTDIR, NULL);
	if (g_file_test(dotdir, G_FILE_TEST_IS_DIR)) {
		return dotdir;
	}
	g_free(dotdir);
#endif /* ENABLE_DEPRECATED */
	return NULL;
}

/* ================================================================
 * Public accessors
 * ================================================================ */

/*!
 * \brief Get an ordered list of gEDA data directories
 * \par Function Description
 * Return an ordered list of directories to be searched for
 * system-wide gEDA data.  This list is computed as follows:
 *
 * 1. If the $GEDADATA environment variable is set, add it to the
 *    list.
 *
 * 2. If the $GEDADATA environment variable is not set, add the "gEDA"
 *    subdirectory of each of the platform-specific system-wide
 *    application data directories, as provided by
 *    g_get_system_data_dirs().
 *
 * 3. For non-relocatable builds the installation directory configured
 *    at build time is appended to the list.
 *
 * \return An ordered list of directories to be searched for system
 * data.
 */
const gchar * const *
eda_get_system_data_dirs(void)
{
	static const gchar **system_data_dirs;

	if (g_once_init_enter(&system_data_dirs)) {
		const gchar * const env_names[] = { DATA_ENV, NULL };
		const gchar * const * xdg_dirs = g_get_system_data_dirs();
		const gchar * const cfg_dirs[] = { GEDADATADIR, NULL };

		const gchar **dirs =
			build_search_list(env_names, xdg_dirs, cfg_dirs);

		g_once_init_leave(&system_data_dirs, dirs);
	}

	return system_data_dirs;
}

/*!
 * \brief Get an ordered list of gEDA configuration directories
 * \par Function Description
 * Return an ordered list of directories to be searched for
 * system-wide gEDA configuration.  This list is computed as follows:
 *
 * 1. If the $GEDADATARC environment variable is set, add it to the
 *    list.  Otherwise, if $GEDADATA is set, add that to the list.
 *
 * 2. If neither environment variable is set, add the "gEDA"
 *    subdirectory of each of the platform-specific system-wide
 *    application directories, as provided by
 *    g_get_system_config_dirs().
 *
 * 3. For non-relocatable builds the configuration installation
 * directory configured at build time is appended to the list.
 *
 * \return An ordered list of directories to be searched for system
 * configuration.
 */
const gchar * const *
eda_get_system_config_dirs(void)
{
	static const gchar **system_config_dirs;

	if (g_once_init_enter(&system_config_dirs)) {
		const gchar * const env_names[] = { CONFIG_ENV, DATA_ENV, NULL };
		const gchar * const * xdg_dirs = g_get_system_config_dirs();
		const gchar * const cfg_dirs[] = { GEDARCDIR, NULL };

		const gchar **dirs =
			build_search_list(env_names, xdg_dirs, cfg_dirs);

		g_once_init_leave(&system_config_dirs, dirs);
	}

	return system_config_dirs;
}

const gchar *
eda_get_user_data_dir(void)
{
	static gchar *user_data_dir;

	if (g_once_init_enter(&user_data_dir)) {
		gchar *dir = get_user_dotdir();

		if (!dir) {
			dir = g_build_filename(g_get_user_data_dir(),

			                       DATA_XDG_SUBDIR, NULL);
		}

		g_once_init_leave(&user_data_dir, dir);
	}

	return user_data_dir;
}

const gchar *
eda_get_user_config_dir(void)
{
	static gchar *user_config_dir;

	if (g_once_init_enter(&user_config_dir)) {
		gchar *dir = get_user_dotdir();

		if (!dir) {
			dir = g_build_filename(g_get_user_config_dir(),
			                       DATA_XDG_SUBDIR, NULL);
		}

		g_once_init_leave(&user_config_dir, dir);
	}

	return user_config_dir;
}

/* ================================================================
 * Module initialisation
 * ================================================================ */

/*!
 * \brief Initialise data and configuration search paths.
 * \par Function Description
 * Compute and initialise configuration and data search paths used by
 * gEDA, and set any related environment variables.  Should only be
 * called (once) by libgeda_init().
 */
void
eda_paths_init(void)
{
	/* Force initialisation */
	eda_get_system_data_dirs();
	eda_get_system_config_dirs();
	eda_get_user_data_dir();
	eda_get_user_config_dir();

	eda_paths_init_env();
}
