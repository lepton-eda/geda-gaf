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
/*! \file object.h
 */

G_BEGIN_DECLS

struct st_object
{
  int type;                             /* Basic information */
  int sid;
  char *name;

  LeptonPage *page; /* Parent page */

  LeptonBounds bounds;

  LeptonComponent *component;
  LeptonLine *line;
  LeptonCircle *circle;
  LeptonArc *arc;
  LeptonBox *box;
  LeptonText *text;
  LeptonPicture *picture;
  LeptonPath *path;

  GList *conn_list;                     /* List of connections */
  /* to and from this object */

  /* every graphical primitive have more or less the same options. */
  /* depending on its nature a primitive is concerned with one or more */
  /* of these fields. If not, value must be ignored. */
  LeptonLineCapType line_end;
  LeptonStrokeType line_type;
  int line_width;
  int line_space;
  int line_length;

  LeptonFillType fill_type;
  int fill_width;
  int fill_angle1, fill_pitch1;
  int fill_angle2, fill_pitch2;

  gchar *component_basename;            /* Component Library Symbol name */
  LeptonObject *parent;                 /* Parent object pointer */

  int color;                            /* Which color */
  int dont_redraw;                      /* Flag to skip redrawing */
  int selectable;                       /* object selectable flag */
  int selected;                         /* object selected flag */

  /* controls which direction bus rippers go */
  /* it is either 0 for un-inited, */
  /* 1 for right, -1 for left (horizontal bus) */
  /* 1 for up, -1 for down (vertial bus) */
  int bus_ripper_direction;             /* only valid on buses */


  int font_text_size;                   /* used only with fonts defs */
  GList *font_prim_objs;                /* used only with fonts defs */

  int whichend;    /* for pins only, either 0 or 1 */
  int pin_type;    /* for pins only, either NET or BUS */

  GList *attribs;       /* attribute stuff */
  int show_name_value;
  int visibility;
  LeptonObject *attached_to;  /* when object is an attribute */
  LeptonObject *copied_to;    /* used when copying attributes */

  GList *weak_refs; /* Weak references */
};


/* Accessors. */
int
lepton_object_get_id (LeptonObject *object);

void
lepton_object_set_id (LeptonObject *object,
                      int id);
int
lepton_object_get_type (const LeptonObject *object);

void
lepton_object_set_type (LeptonObject *object,
                        int type);

/* Helpers. */

gboolean
lepton_object_is_arc (const LeptonObject *object);

gboolean
lepton_object_is_box (const LeptonObject *object);

gboolean
lepton_object_is_bus (const LeptonObject *object);

gboolean
lepton_object_is_circle (const LeptonObject *object);

gboolean
lepton_object_is_component (const LeptonObject *object);

gboolean
lepton_object_is_line (const LeptonObject *object);

gboolean
lepton_object_is_net (const LeptonObject *object);

gboolean
lepton_object_is_path (const LeptonObject *object);

gboolean
lepton_object_is_picture (const LeptonObject *object);

gboolean
lepton_object_is_pin (const LeptonObject *object);

gboolean
lepton_object_is_text (const LeptonObject *object);

/* construction, destruction */

LeptonObject*
lepton_object_new (int type,
                   char const *prefix);
LeptonObject*
lepton_object_copy (LeptonObject *object);

void
lepton_object_delete (LeptonObject *o_current);

/* methods */

gboolean
lepton_object_calculate_visible_bounds (LeptonObject *o_current,
                                        gboolean include_hidden,
                                        gint *rleft,
                                        gint *rtop,
                                        gint *rright,
                                        gint *rbottom);
gint
lepton_object_get_color (const LeptonObject *object);

gint
lepton_object_get_drawing_color (const LeptonObject *object);

gboolean
lepton_object_get_position (const LeptonObject *object,
                            gint *x,
                            gint *y);
gboolean
lepton_object_get_selectable (const LeptonObject *object);

gint
lepton_object_get_visibility (const LeptonObject *object);

void
lepton_object_rotate (int world_centerx,
                      int world_centery,
                      int angle,
                      LeptonObject *object);
void
lepton_object_mirror (int world_centerx,
                      int world_centery,
                      LeptonObject *object);
void
lepton_object_set_selectable (LeptonObject *object,
                              gboolean selectable);
double
lepton_object_shortest_distance (LeptonObject *object,
                                 int x,
                                 int y,
                                 gboolean include_hidden);
double
lepton_object_shortest_distance_full (LeptonObject *object,
                                      int x,
                                      int y,
                                      int force_solid,
                                      gboolean include_hidden);
void
lepton_object_translate (LeptonObject *object,
                         gint dx,
                         gint dy);
gboolean
lepton_object_get_fill_options (LeptonObject *object,
                                LeptonFillType *type,
                                int *width,
                                int *pitch1,
                                int *angle1,
                                int *pitch2,
                                int *angle2);
gboolean
lepton_object_get_line_options (LeptonObject *object,
                                LeptonLineCapType *end,
                                LeptonStrokeType *type,
                                int *width,
                                int *length,
                                int *space);
LeptonPage*
lepton_object_get_page (LeptonObject *object);

void
lepton_object_page_set_changed (LeptonObject *object);

LeptonObject*
lepton_object_get_parent (LeptonObject *object);

gboolean
lepton_object_is_visible (const LeptonObject *object);

void
lepton_object_set_color (LeptonObject *object,
                         int color);

void
lepton_object_set_fill_options (LeptonObject *o_current,
                                LeptonFillType type,
                                int width,
                                int pitch1,
                                int angle1,
                                int pitch2,
                                int angle2);
void
lepton_object_set_line_options (LeptonObject *o_current,
                                LeptonLineCapType end,
                                LeptonStrokeType type,
                                int width,
                                int length,
                                int space);
void
lepton_object_set_visibility (LeptonObject *object,
                              int visibility);
void
lepton_object_add_change_notify (LeptonToplevel *toplevel,
                                 ChangeNotifyFunc pre_change_func,
                                 ChangeNotifyFunc change_func,
                                 void *user_data);
void
lepton_object_remove_change_notify (LeptonToplevel *toplevel,
                                    ChangeNotifyFunc pre_change_func,
                                    ChangeNotifyFunc change_func,
                                    void *user_data);
void
lepton_object_weak_ref (LeptonObject *object,
                        void (*notify_func)(void *, void *),
                        void *user_data);
void
lepton_object_weak_unref (LeptonObject *object,
                          void (*notify_func)(void *, void *),
                          void *user_data);
void
lepton_object_add_weak_ptr (LeptonObject *object,
                            void *weak_pointer_loc);
void
lepton_object_remove_weak_ptr (LeptonObject *object,
                               void *weak_pointer_loc);

G_END_DECLS
