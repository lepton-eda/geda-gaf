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

#ifndef __EDA_PATHS_H__
#define __EDA_PATHS_H__

G_BEGIN_DECLS

/* ----------------------------------------------------------------
 * gEDA standard directories
 * ---------------------------------------------------------------- */

const gchar * const *eda_get_system_data_dirs(void);
const gchar * const *eda_get_system_config_dirs(void);
const gchar *eda_get_user_data_dir(void);
const gchar *eda_get_user_config_dir(void);
const gchar *eda_get_user_cache_dir(void);

/* ----------------------------------------------------------------
 * Initialisation
 * ---------------------------------------------------------------- */

#ifdef __LIBLEPTON_INTERNAL_API__
G_GNUC_INTERNAL void eda_paths_init(void);
#endif /* __LIBLEPTON_PRIV_H_INSIDE__ */

G_END_DECLS

#endif /* !__EDA_PATHS_H__ */
