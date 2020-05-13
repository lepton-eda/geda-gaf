/* Lepton EDA library
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2016 gEDA Contributors
 * Copyright (C) 2017-2020 Lepton EDA Contributors
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

/*! \file geda_circle_object.c
 *
 *  \brief Functions operating on circle drawing objects
 */

#include <config.h>

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#include "libgeda_priv.h"


/*! \brief Create and add circle OBJECT to list.
 *  \par Function Description
 *  This function creates a new object representing a circle.
 *
 *  The circle is described by its center (<B>x</B>,<B>y</B>) and its radius
 *  <B>radius</B>.
 *  The <B>type</B> parameter must be equal to <B>OBJ_CIRCLE</B>. The <B>color</B>
 *  corresponds to the color the box will be drawn with.
 *
 *  The <B>OBJECT</B> structure is allocated with the #s_basic_new_object()
 *  function. The structure describing the circle is allocated and initialized
 *  with the parameters given to the function.
 *
 *  Both the line type and the filling type are set to default values : solid
 *  line type with a width of 0, and no filling. It can be changed after
 *  with #o_set_line_options() and #o_set_fill_options().
 *
 *  \param [in]     color        Circle line color.
 *  \param [in]     center_x     Center x coordinate.
 *  \param [in]     center_y     Center y coordinate.
 *  \param [in]     radius       Radius of new circle.
 *  \return A pointer to the new end of the object list.
 */
GedaObject*
geda_circle_object_new (gint color,
                        gint center_x,
                        gint center_y,
                        gint radius)
{
  GedaObject *new_node;

  /* create the object */
  new_node = s_basic_new_object (OBJ_CIRCLE, "circle");
  new_node->color  = color;

  new_node->circle = geda_circle_new ();

  /* describe the circle with its center and radius */
  new_node->circle->center_x = center_x;
  new_node->circle->center_y = center_y;
  new_node->circle->radius   = radius;

  /* line type and filling initialized to default */
  o_set_line_options (new_node,
                      DEFAULT_OBJECT_END,
                      TYPE_SOLID,
                      LINE_WIDTH,
                      -1,
                      -1);

  o_set_fill_options (new_node,
                      FILLING_HOLLOW,
                      -1,
                      -1,
                      -1,
                      -1,
                      -1);

  return new_node;
}

/*! \brief Create a copy of a circle.
 *  \par Function Description
 *  The function #geda_circle_object_copy() creates a verbatim copy of the object
 *  pointed by <B>o_current</B> describing a circle.
 *
 *  \param [in]  o_current  Circle OBJECT to copy.
 *  \return The new OBJECT
 */
GedaObject*
geda_circle_object_copy (const GedaObject *object)
{
  GedaObject *new_obj;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->circle != NULL, NULL);
  g_return_val_if_fail (object->type == OBJ_CIRCLE, NULL);

  new_obj = geda_circle_object_new (object->color,
                                    object->circle->center_x,
                                    object->circle->center_y,
                                    object->circle->radius);

  o_set_line_options (new_obj,
                      object->line_end,
                      object->line_type,
                      object->line_width,
                      object->line_length,
                      object->line_space);

  o_set_fill_options (new_obj,
                      object->fill_type,
                      object->fill_width,
                      object->fill_pitch1,
                      object->fill_angle1,
                      object->fill_pitch2,
                      object->fill_angle2);

  return new_obj;
}

/*! \brief Get the x coordinate of the center of the circle
 *
 *  \param [in] object The circle
 *  \return The x coordinate of the center of the circle
 */
gint
geda_circle_object_get_center_x (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (object->circle != NULL, 0);
  g_return_val_if_fail (object->type == OBJ_CIRCLE, 0);

  return object->circle->center_x;
}

/*! \brief Get the y coordinate of the center of the circle
 *
 *  \param [in] object The circle
 *  \return The y coordinate of the center of the circle
 */
gint
geda_circle_object_get_center_y (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (object->circle != NULL, 0);
  g_return_val_if_fail (object->type == OBJ_CIRCLE, 0);

  return object->circle->center_y;
}

/*! \brief Get the radius of the circle
 *
 *  \param [in] object The circle
 *  \return The radius of the circle
 */
gint
geda_circle_object_get_radius (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (object->circle != NULL, 0);
  g_return_val_if_fail (object->type == OBJ_CIRCLE, 0);

  return object->circle->radius;
}

/*! \brief Set the x coordinate of the center of the circle
 *
 *  \param [in,out] object The circle
 *  \param [in] x The new y coordinate for the circle center
 */
void
geda_circle_object_set_center_x (GedaObject *object, gint x)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->circle != NULL);
  g_return_if_fail (object->type == OBJ_CIRCLE);

  object->circle->center_x = x;
}

/*! \brief Set the y coordinate of the center of the circle
 *
 *  \param [in,out] object The circle
 *  \param [in] y The new y coordinate for the circle center
 */
void
geda_circle_object_set_center_y (GedaObject *object, gint y)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->circle != NULL);
  g_return_if_fail (object->type == OBJ_CIRCLE);

  object->circle->center_y = y;
}

/*! \brief Set the radius of the circle
 *
 *  The radius must be greater than zero.
 *
 *  \param [in,out] object The circle
 *  \param [in] radius The new radius for the circle
 */
void
geda_circle_object_set_radius (GedaObject *object, gint radius)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->circle != NULL);
  g_return_if_fail (object->type == OBJ_CIRCLE);
  g_return_if_fail (radius > 0);

  object->circle->radius = radius;
}

/*! \brief Modify the description of a circle OBJECT.
 *  \par Function Description
 *  This function modifies the description of the circle object <B>*object</B>
 *  depending on <B>whichone</B> that give the meaning of the <B>x</B> and <B>y</B>
 *  parameters.
 *
 *  If <B>whichone</B> is equal to <B>CIRCLE_CENTER</B>, the new center of the
 *  circle is given by (<B>x</B>,<B>y</B>) where <B>x</B> and <B>y</B> are in world units.
 *
 *  If <B>whichone</B> is equal to <B>CIRCLE_RADIUS</B>, the radius is given by
 *  <B>x</B> - in world units. <B>y</B> is ignored.
 *
 *  The bounding box of the circle object is updated after the modification of its
 *  parameters.
 *
 *  \param [in,out] object     Circle OBJECT to modify.
 *  \param [in]     x          New center x coordinate, or radius value.
 *  \param [in]     y          New center y coordinate.
 *                             Unused if radius is being modified.
 *  \param [in]     whichone   Which circle parameter to modify.
 *
 *  <B>whichone</B> can have the following values:
 *  <DL>
 *    <DT>*</DT><DD>CIRCLE_CENTER
 *    <DT>*</DT><DD>CIRCLE_RADIUS
 *  </DL>
 */
void
geda_circle_object_modify (GedaObject *object,
                           gint x,
                           gint y,
                           gint whichone)
{
  o_emit_pre_change_notify (object);

  switch(whichone) {
    case CIRCLE_CENTER:
      geda_circle_object_set_center_x (object, x);
      geda_circle_object_set_center_y (object, y);
      break;

    case CIRCLE_RADIUS:
      geda_circle_object_set_radius (object, x);
      break;

    default:
      break;
  }

  o_emit_change_notify (object);
}

/*! \brief Create circle OBJECT from character string.
 *  \par Function Description
 *  The #o_circle_read() function gets from the character string <B>*buff</B> the
 *  description of a circle.
 *
 *  Depending on <B>*version</B>, the right file format is considered.
 *  Currently two file format revisions are supported :
 *  <DL>
 *    <DT>*</DT><DD>the file format used until 2000704 release.
 *    <DT>*</DT><DD>the file format used for the releases after 20000704.
 *  </DL>
 *
 *  \param [in]  buf             Character string with circle description.
 *  \param [in]  release_ver     libgeda release version number.
 *  \param [in]  fileformat_ver  libgeda file format version number.
 *  \return A pointer to the new circle object, or NULL on error.
 */
GedaObject*
o_circle_read (const char buf[],
               unsigned int release_ver,
               unsigned int fileformat_ver,
               GError ** err)
{
  GedaObject *new_obj;
  char type;
  int x1, y1;
  int radius;
  int color;
  int circle_width, circle_space, circle_length;
  int fill_width, angle1, pitch1, angle2, pitch2;
  int circle_end;
  int circle_type;
  int circle_fill;

  if(release_ver <= VERSION_20000704) {
    /*
     * The old geda file format, i.e. releases 20000704 and older, does not
     * handle the line type and the filling of the box object. They are set
     * to default.
     */
    if (sscanf(buf, "%c %d %d %d %d\n", &type, &x1, &y1, &radius, &color) != 5) {
      g_set_error(err, EDA_ERROR, EDA_ERROR_PARSE, _("Failed to parse circle object"));
      return NULL;
    }

    circle_width = 0;
    circle_end   = END_NONE;
    circle_type  = TYPE_SOLID;
    circle_length= -1;
    circle_space = -1;

    circle_fill  = FILLING_HOLLOW;
    fill_width  = 0;
    angle1      = -1;
    pitch1      = -1;
    angle2      = -1;
    pitch2      = -1;

  } else {

    /*
     * The current line format to describe a circle is a space separated
     * list of characters and numbers in plain ASCII on a single line. The
     * meaning of each item is described in the file format documentation.
     */
    if (sscanf(buf, "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
	       &type, &x1, &y1, &radius, &color,
	       &circle_width, &circle_end, &circle_type,
	       &circle_length, &circle_space, &circle_fill,
	       &fill_width, &angle1, &pitch1, &angle2, &pitch2) != 16) {
      g_set_error(err, EDA_ERROR, EDA_ERROR_PARSE, _("Failed to parse circle object"));
      return NULL;
    }
  }


  if (radius <= 0) {
    s_log_message(_("Found a zero or negative radius circle "
                    "[ %1$c %2$d %3$d %4$d %5$d ]"),
                  type, x1, y1, radius, color);
    s_log_message (_("Setting radius to 0."));
    radius = 0;
  }

  if (color < 0 || color > MAX_COLORS) {
    s_log_message(_("Found an invalid color [ %1$s ]"), buf);
    s_log_message(_("Setting color to default color."));
    color = DEFAULT_COLOR;
  }

  /*
   * A circle is internally described by its center and its radius.
   *
   * A new object is allocated, initialized and added to the object list.
   * Its filling and line type are set according to the values of the field
   * on the line.
   */
  new_obj = geda_circle_object_new (color, x1, y1, radius);

  o_set_line_options (new_obj,
                      (OBJECT_END) circle_end,
                      (OBJECT_TYPE) circle_type,
                      circle_width,
                      circle_length,
                      circle_space);
  o_set_fill_options (new_obj,
                      (OBJECT_FILLING) circle_fill,
                      fill_width,
                      pitch1,
                      angle1,
                      pitch2,
                      angle2);

  return new_obj;
}

/*! \brief Create a character string representation of a circle OBJECT.
 *  \par Function Description
 *  This function formats a string in the buffer <B>*buff</B> to describe the
 *  circle object <B>*object</B>.
 *  It follows the post-20000704 release file format that handle the line
 *  type and fill options.
 *
 *  \param [in] object  Circle OBJECT to create string from.
 *  \return A pointer to the circle OBJECT character string.
 *
 *  \note
 *  Caller must g_free returned character string.
 *
 */
gchar*
geda_circle_object_to_buffer (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->circle != NULL, NULL);
  g_return_val_if_fail (object->type == OBJ_CIRCLE, NULL);

  return g_strdup_printf ("%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                          OBJ_CIRCLE,
                          geda_circle_object_get_center_x (object),
                          geda_circle_object_get_center_y (object),
                          geda_circle_object_get_radius (object),
                          geda_object_get_color (object),
                          object->line_width,
                          object->line_end,
                          object->line_type,
                          object->line_length,
                          object->line_space,
                          object->fill_type,
                          object->fill_width,
                          object->fill_angle1,
                          object->fill_pitch1,
                          object->fill_angle2,
                          object->fill_pitch2);
}

/*! \brief Translate a circle position in WORLD coordinates by a delta.
 *  \par Function Description
 *  This function applies a translation of (<B>x1</B>,<B>y1</B>) to the circle
 *  described by <B>*object</B>. <B>x1</B> and <B>y1</B> are in world unit.
 *
 *  \param [in,out] object  Circle GedaObject to translate.
 *  \param [in]     dx      x distance to move.
 *  \param [in]     dy      y distance to move.
 */
void
geda_circle_object_translate (GedaObject *object, gint dx, gint dy)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->circle != NULL);
  g_return_if_fail (object->type == OBJ_CIRCLE);

  /* Do world coords */
  object->circle->center_x = object->circle->center_x + dx;
  object->circle->center_y = object->circle->center_y + dy;
}

/*! \brief Rotate Circle OBJECT using WORLD coordinates.
 *  \par Function Description
 *  The function #geda_circle_object_rotate() rotate the circle described by
 *  <B>*object</B> around the (<B>world_centerx</B>,<B>world_centery</B>) point by
 *  angle <B>angle</B> degrees.
 *  The center of rotation is in world unit.
 *
 *  \param [in]      world_centerx  Rotation center x coordinate in WORLD units.
 *  \param [in]      world_centery  Rotation center y coordinate in WORLD units.
 *  \param [in]      angle          Rotation angle in degrees (See note below).
 *  \param [in,out]  object         Circle OBJECT to rotate.
 */
void
geda_circle_object_rotate (gint world_centerx,
                           gint world_centery,
                           gint angle,
                           GedaObject *object)
{
  int newx, newy;
  int x, y;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->circle != NULL);
  g_return_if_fail (object->type == OBJ_CIRCLE);

  /* Only 90 degree multiple and positive angles are allowed. */
  /* angle must be positive */
  if(angle < 0) angle = -angle;
  /* angle must be a 90 multiple or no rotation performed */
  if((angle % 90) != 0) return;

  /*
   * The center of rotation (<B>world_centerx</B>,<B>world_centery</B>) is
   * translated to the origin. The rotation of the center around the origin
   * is then performed. Finally, the rotated circle is translated back to
   * its previous location.
   */

  /* translate object to origin */
  object->circle->center_x -= world_centerx;
  object->circle->center_y -= world_centery;

  /* rotate the center of the circle around the origin */
  x = object->circle->center_x;
  y = object->circle->center_y;
  geda_point_rotate_90 (x, y, angle, &newx, &newy);
  object->circle->center_x = newx;
  object->circle->center_y = newy;

  /* translate back in position */
  object->circle->center_x += world_centerx;
  object->circle->center_y += world_centery;
}

/*! \brief Mirror circle using WORLD coordinates.
 *  \par Function Description
 *  This function recalculates the screen coords of the <B>o_current</B> pointed
 *  circle object from its world coords.
 *
 *  The circle coordinates and its bounding are recalculated as well as the
 *  OBJECT specific (line width, filling ...).
 *
 *  \param [in]     world_centerx  Origin x coordinate in WORLD units.
 *  \param [in]     world_centery  Origin y coordinate in WORLD units.
 *  \param [in,out] object         Circle OBJECT to mirror.
 */
void
geda_circle_object_mirror (gint world_centerx,
                           gint world_centery,
                           GedaObject *object)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->circle != NULL);
  g_return_if_fail (object->type == OBJ_CIRCLE);

  /* translate object to origin */
  object->circle->center_x -= world_centerx;

  /* mirror the center of the circle */
  object->circle->center_x = -object->circle->center_x;

  /* translate back in position */
  object->circle->center_x += world_centerx;
}

/*! \brief Get circle bounding rectangle in WORLD coordinates.
 *  \par Function Description
 *  This function sets the <B>left</B>, <B>top</B>, <B>right</B> and <B>bottom</B>
 *  parameters to the boundings of the circle object described in <B>*circle</B>
 *  in world units.
 *
 *  \param [in]  object    Circle OBJECT to read coordinates from.
 *  \param [out] bounds    The bounds of the circle object.
 */
void
geda_circle_object_calculate_bounds (const GedaObject *object,
                                     GedaBounds *bounds)
{
  gint expand;

  geda_bounds_init (bounds);

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->type == OBJ_CIRCLE);
  g_return_if_fail (object->circle != NULL);

  geda_circle_calculate_bounds (object->circle, bounds);

  expand = (object->line_width + 1) / 2;

  /* This isn't strictly correct, but a 1st order approximation */
  geda_bounds_expand (bounds, bounds, expand, expand);
}

/*! \brief get the position of the center point
 *  \par Function Description
 *  This function gets the position of the center point of a circle object.
 *
 *  \param [in] object   The object to get the position.
 *  \param [out] x       pointer to the x-position
 *  \param [out] y       pointer to the y-position
 *  \return TRUE if successfully determined the position, FALSE otherwise
 */
gboolean
geda_circle_object_get_position (const GedaObject *object, gint *x, gint *y)
{
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->type == OBJ_CIRCLE, FALSE);
  g_return_val_if_fail (object->circle != NULL, FALSE);

  if (x != NULL) {
    *x = object->circle->center_x;
  }

  if (y != NULL) {
    *y = object->circle->center_y;
  }

  return TRUE;
}

/*! \brief Calculates the distance between the given point and the closest
 * point on the perimeter of the circle.
 *
 *  \param [in] object         The circle OBJECT.
 *  \param [in] x              The x coordinate of the given point.
 *  \param [in] y              The y coordinate of the given point.
 *  \param [in] force_solid    If true, force treating the object as solid.
 *  \param [in] include_hidden Take hidden text into account.
 *  \return The shortest distance from the object to the point.  With an
 *  invalid parameter, this function returns G_MAXDOUBLE.
 */
gdouble
geda_circle_object_shortest_distance (GedaObject *object,
                                      gint x,
                                      gint y,
                                      gint force_solid,
                                      gboolean include_hidden)
{
  gboolean solid;

  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->circle != NULL, G_MAXDOUBLE);
  g_return_val_if_fail (object->type == OBJ_CIRCLE, FALSE);

  solid = force_solid || object->fill_type != FILLING_HOLLOW;

  return geda_circle_shortest_distance (object->circle, x, y, solid);
}

