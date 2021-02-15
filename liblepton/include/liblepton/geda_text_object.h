/* Lepton EDA library
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2016 gEDA Contributors
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
/*! \file geda_text_object.h
 *
 *  \brief Functions operating on text drawing objects
 */

G_BEGIN_DECLS

#define DEFAULT_TEXT_SIZE 10
#define MINIMUM_TEXT_SIZE 1

/* construction, destruction */

LeptonObject*
geda_text_object_new (gint color,
                      gint x,
                      gint y,
                      gint alignment,
                      gint angle,
                      const gchar *string,
                      gint size,
                      gint visibility,
                      gint show_name_value);

LeptonObject*
geda_text_object_copy (const LeptonObject *object);

/* methods */

gboolean
geda_text_object_calculate_bounds (const LeptonObject *object,
                                   gboolean include_hidden,
                                   GedaBounds *bounds);

gint
geda_text_object_get_alignment (const LeptonObject *object);

gint
geda_text_object_get_angle (const LeptonObject *object);

gboolean
geda_text_object_get_position (const LeptonObject *object, gint *x, gint *y);

gint
geda_text_object_get_size (const LeptonObject *object);

gdouble
geda_text_object_get_size_in_points (const LeptonObject *object);

const gchar*
geda_text_object_get_string (const LeptonObject *object);

gint
geda_text_object_get_x (const LeptonObject *object);

gint
geda_text_object_get_y (const LeptonObject *object);

void
geda_text_object_mirror (int world_centerx,
                         int world_centery,
                         LeptonObject *object);

void
geda_text_object_rotate (int world_centerx,
                         int world_centery,
                         int angle,
                         LeptonObject *object);

void
geda_text_object_set_alignment (LeptonObject *object, gint alignment);

void
geda_text_object_set_angle (LeptonObject *object, gint angle);

void
geda_text_object_set_size (LeptonObject *object, gint size);

void
geda_text_object_set_x (LeptonObject *object, gint x);

void
geda_text_object_set_y (LeptonObject *object, gint y);

double
geda_text_object_shortest_distance (LeptonObject *object,
                                    int x,
                                    int y,
                                    int force_soild,
                                    gboolean include_hidden);

gchar*
geda_text_object_to_buffer (const LeptonObject *object);

void
geda_text_object_translate (LeptonObject *object, int dx, int dy);

/* older methods, need renaming */

void
o_text_recreate (LeptonObject *o_current);

void
o_text_set_string (LeptonObject *obj,
                   const gchar *new_string);

LeptonObject*
o_text_read (const char *first_line,
             TextBuffer *tb,
             unsigned int release_ver,
             unsigned int fileformat_ver,
             GError **err);

G_END_DECLS
