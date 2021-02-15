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
/*! \file geda_box_object.h
 *
 *  \brief Functions operating on box drawing objects
 */

G_BEGIN_DECLS

#define BOX_UPPER_LEFT  0
#define BOX_LOWER_RIGHT 1
#define BOX_UPPER_RIGHT 2
#define BOX_LOWER_LEFT  3

/* construction, destruction */

LeptonObject*
geda_box_object_new (char type,
                     int color,
                     int x1,
                     int y1,
                     int x2,
                     int y2);

LeptonObject*
geda_box_object_copy (LeptonObject *o_current);

/* methods */

void
geda_box_object_calculate_bounds (const LeptonObject *object,
                                  GedaBounds *bounds);

gboolean
geda_box_object_get_position (const LeptonObject *object, gint *x, gint *y);

void
geda_box_object_mirror (int world_centerx,
                        int world_centery,
                        LeptonObject *object);

void
geda_box_object_modify (LeptonObject *object,
                        int x,
                        int y,
                        int whichone);

void
geda_box_object_modify_all (LeptonObject *object,
                            int x1,
                            int y1,
                            int x2,
                            int y2);

void
geda_box_object_rotate (int world_centerx,
                        int world_centery,
                        int angle,
                        LeptonObject *object);

double
geda_box_object_shortest_distance (LeptonObject *object,
                                   int x,
                                   int y,
                                   int force_soild,
                                   gboolean include_hidden);

gchar*
geda_box_object_to_buffer (const LeptonObject *object);

void
geda_box_object_translate (LeptonObject *object, int dx, int dy);

LeptonObject*
o_box_read (const char buf[],
            unsigned int release_ver,
            unsigned int fileformat_ver,
            GError **err);

G_END_DECLS
