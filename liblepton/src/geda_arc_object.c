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

/*! \file o_arc_basic.c
 *  \brief functions for the arc object
 */

#include <config.h>

#include <stdio.h>
#include <math.h>

#include "libgeda_priv.h"

/*! \brief create a new arc object
 *
 *  The line and fill type of the created arc are set to default.
 *
 *  \param [in] color the color index of the arc
 *  \param [in] center_x the x coordinate of the center
 *  \param [in] center_y the y coordinate of the center
 *  \param [in] radius the radius of the arc
 *  \param [in] start_angle the starting angle in degrees
 *  \param [in] sweep_angle the sweep angle in degrees
 *  \return the new arc object
 */
GedaObject*
geda_arc_object_new (gint color,
                     gint center_x,
                     gint center_y,
                     gint radius,
                     gint start_angle,
                     gint sweep_angle)
{

  GedaObject *new_node;

  new_node = s_basic_new_object (OBJ_ARC, "arc");

  new_node->color = color;

  new_node->arc = geda_arc_new ();

  /*! \note
   *  The ARC structure is initialized with the parameters.
   *  A default initialization is performed for the line and
   *  fill type to avoid misunderstanding.
   *
   *  The functions relative to the use of the object are sets.
   */

  /* World coordinates */
  new_node->arc->x      = center_x;
  new_node->arc->y      = center_y;
  new_node->arc->radius = radius;

  /* must check the sign of start_angle, sweep_angle ... */
  if(sweep_angle < 0) {
    start_angle = start_angle + sweep_angle;
    sweep_angle = -sweep_angle;
  }
  if(start_angle < 0) start_angle = 360 + start_angle;

  new_node->arc->start_angle = start_angle;
  new_node->arc->sweep_angle = sweep_angle;

  /* Default init */
  o_set_line_options (new_node,
                      DEFAULT_OBJECT_END,
                      TYPE_SOLID,
                      LINE_WIDTH,
                      -1,
                      -1);

  o_set_fill_options (new_node,
                      FILLING_HOLLOW, -1, -1, -1, -1, -1);

  return new_node;
}

/*! \brief create a copy of an existing arc object
 *
 *  \param [in] object the arc object to copy
 *  \return The new arc object
 */
GedaObject*
geda_arc_object_copy (const GedaObject *object)
{
  GedaObject *new_object;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->arc != NULL, NULL);
  g_return_val_if_fail (object->type == OBJ_ARC, NULL);

  new_object = geda_arc_object_new (object->color,
                                    object->arc->x,
                                    object->arc->y,
                                    object->arc->radius,
                                    object->arc->start_angle,
                                    object->arc->sweep_angle);

  o_set_line_options (new_object,
                      object->line_end,
                      object->line_type,
                      object->line_width,
                      object->line_length,
                      object->line_space);

  o_set_fill_options (new_object,
                      FILLING_HOLLOW,
                      -1,
                      -1,
                      -1,
                      -1,
                      -1);

  return new_object;
}

/*! \brief Get the x coordinate of the center of the arc
 *
 *  \param [in] object The arc
 *  \return The x coordinate of the center of the arc
 */
gint
geda_arc_object_get_center_x (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (object->arc != NULL, 0);
  g_return_val_if_fail (object->type == OBJ_ARC, 0);

  return object->arc->x;
}

/*! \brief Get the y coordinate of the center of the arc
 *
 *  \param [in] object The arc
 *  \return The y coordinate of the center of the arc
 */
gint
geda_arc_object_get_center_y (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (object->arc != NULL, 0);
  g_return_val_if_fail (object->type == OBJ_ARC, 0);

  return object->arc->y;
}

/*! \brief Get the radius of the arc
 *
 *  \param [in] object The arc
 *  \return The raduis of the arc
 */
gint
geda_arc_object_get_radius (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (object->arc != NULL, 0);
  g_return_val_if_fail (object->type == OBJ_ARC, 0);

  return object->arc->radius;
}

/*! \brief Get the starting angle of the arc
 *
 *  \param [in] object The arc
 *  \return The starting angle of the arc
 */
gint
geda_arc_object_get_start_angle (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (object->arc != NULL, 0);
  g_return_val_if_fail (object->type == OBJ_ARC, 0);

  return object->arc->start_angle;
}

/*! \brief Get the sweep angle of the arc
 *
 *  \param [in] object The arc
 *  \return The sweep angle of the arc
 */
gint
geda_arc_object_get_sweep_angle (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (object->arc != NULL, 0);
  g_return_val_if_fail (object->type == OBJ_ARC, 0);

  return object->arc->sweep_angle;
}

/*! \brief Set the x coordinate of the center of the arc
 *
 *  \param [in,out] object The arc
 *  \param [in] x The new y coordinate for the arc center
 */
void
geda_arc_object_set_center_x (GedaObject *object, gint x)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->arc != NULL);
  g_return_if_fail (object->type == OBJ_ARC);

  object->arc->x = x;
}

/*! \brief Set the y coordinate of the center of the arc
 *
 *  \param [in,out] object The arc
 *  \param [in] y The new y coordinate for the arc center
 */
void
geda_arc_object_set_center_y (GedaObject *object, gint y)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->arc != NULL);
  g_return_if_fail (object->type == OBJ_ARC);

  object->arc->y = y;
}

/*! \brief Set the radius of the arc
 *
 *  \param [in,out] object The arc
 *  \param [in] radius The new raduis for the arc
 */
void
geda_arc_object_set_radius (GedaObject *object, gint radius)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->arc != NULL);
  g_return_if_fail (object->type == OBJ_ARC);

  object->arc->radius = radius;
}

/*! \brief Set the starting angle of the arc
 *
 *  \param [in,out] object The arc
 *  \param [in] angle The new starting angle for the arc
 */
void
geda_arc_object_set_start_angle (GedaObject *object, gint angle)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->arc != NULL);
  g_return_if_fail (object->type == OBJ_ARC);

  object->arc->start_angle = angle;
}

/*! \brief Set the sweep angle of the arc
 *
 *  \param [in,out] object The arc
 *  \param [in] angle The new sweep angle for the arc
 */
void
geda_arc_object_set_sweep_angle (GedaObject *object, gint angle)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->arc != NULL);
  g_return_if_fail (object->type == OBJ_ARC);

  object->arc->sweep_angle = angle;
}

/*! \brief
 *  \par Function Description
 *  This function modifies the internal values of the arc object
 *  *object according to the whichone parameter.
 *
 *  The new values are given by <B>x</B> and/or <B>y</B>. Their
 *  meaning depends on the value of whichone.
 *
 *  If <B>whichone</B> is equal to #ARC_CENTER, the
 *  (<B>x</B>,<B>y</B>) point is taken as the new center of the
 *  arc in world unit.
 *
 *  If <B>whichone</B> is equal to #ARC_RADIUS, the <B>x</B>
 *  parameter is taken to be the radius of the arc in world
 *  unit. The <B>y</B> parameter is ignored.
 *
 *  If <B>whichone</B> is equal to #ARC_START_ANGLE, the <B>x</B>
 *  parameter is the starting angle of the arc.  <B>x</B> is in
 *  degrees. <B>y</B> is ignored.
 *
 *  If <B>whichone</B> is equal to #ARC_SWEEP_ANGLE, the <B>x</B>
 *  parameter is the ending angle of the arc.  <B>x</B> is in
 *  degrees. <B>y</B> is ignored.
 *
 *  \param [in,out] object
 *  \param [in]     x
 *  \param [in]     y
 *  \param [in]     whichone
 */
void
geda_arc_object_modify (OBJECT *object,
                        int x,
                        int y,
                        int whichone)
{

	o_emit_pre_change_notify (object);

	switch(whichone) {
		case ARC_CENTER:
		/* modify the center of arc object */
		object->arc->x = x;
		object->arc->y = y;
		break;

		case ARC_RADIUS:
		/* modify the radius of arc object */
		object->arc->radius = x;
		break;

		case ARC_START_ANGLE:
		/* modify the start angle of the arc object */
		object->arc->start_angle = x;
		break;

		case ARC_SWEEP_ANGLE:
		/* modify the end angle of the arc object */
		object->arc->sweep_angle = x;
		break;

		default:
		break;
	}

	/* update the screen coords and the bounding box */
	o_emit_change_notify (object);
}

/*! \brief
 *  \par Function Description
 *  This function reads a formatted text buffer describing an arc
 *  in the gEDA file format and initializes the corresponding object.
 *
 *  Depending on the version of the file format the data extraction is
 *  performed differently : currently pre-20000704 and 20000704 on one
 *  hand and post-20000704 file format version on the other hand are supported.
 *  The version is specified in string pointed by <B>fileformat_ver</B>.
 *
 *  To get information on the various file formats have a
 *  look to the fileformats.html document.
 *
 *  The object is initialized with the functions #o_set_line_options() and #o_set_fill_options().
 *  The second one is only used to put initialize unused values for an arc as an arc can not be filled.
 *
 *  The arc is allocated initialized with the function #geda_arc_object_new().
 *
 *  A negative or null radius is not allowed.
 *
 *  \param [in] buf
 *  \param [in] release_ver
 *  \param [in] fileformat_ver
 *  \return The ARC OBJECT that was created, or NULL on error.
 */
OBJECT
*o_arc_read (const char buf[],
             unsigned int release_ver,
             unsigned int fileformat_ver,
             GError **err)
{
  OBJECT *new_obj;
  char type;
  int x1, y1;
  int radius;
  int start_angle;
  int sweep_angle;
  int color;
  int arc_width, arc_length, arc_space;
  int arc_type;
  int arc_end;

  /*! \note
   *  Depending on the version of the file format used to describe this arc,
   *  the buffer is parsed differently. The unknown parameters of the less
   *  restrictive - the oldest - file format are set to common values
   */
  if(release_ver <= VERSION_20000704) {
    if (sscanf(buf, "%c %d %d %d %d %d %d", &type,
	       &x1, &y1, &radius, &start_angle, &sweep_angle, &color) != 7) {
      g_set_error (err, EDA_ERROR, EDA_ERROR_PARSE, _("Failed to parse arc object"));
      return NULL;
    }

    arc_width = 0;
    arc_end   = END_NONE;
    arc_type  = TYPE_SOLID;
    arc_space = -1;
    arc_length= -1;
  } else {
    if (sscanf(buf, "%c %d %d %d %d %d %d %d %d %d %d %d", &type,
	       &x1, &y1, &radius, &start_angle, &sweep_angle, &color,
	       &arc_width, &arc_end, &arc_type, &arc_length, &arc_space) != 12) {
      g_set_error (err, EDA_ERROR, EDA_ERROR_PARSE, _("Failed to parse arc object"));
      return NULL;
    }
  }

  /* Error check */
  if (radius <= 0) {
    s_log_message (_("Found a zero radius arc "
                     "[ %1$c %2$d %3$d %4$d %5$d %6$d %7$d ]"),
                   type, x1, y1, radius, start_angle, sweep_angle, color);
    radius = 0;
  }

  if (!color_id_valid (color)) {
    s_log_message(_("Found an invalid color [ %1$s ]"), buf);
    s_log_message(_("Setting color to default color."));
    color = default_color_id();
  }

  /* Allocation and initialization */
  new_obj = geda_arc_object_new (color,
                                 x1,
                                 y1,
                                 radius,
                                 start_angle,
                                 sweep_angle);

  o_set_line_options (new_obj,
                      (OBJECT_END) arc_end,
                      (OBJECT_TYPE) arc_type,
                      arc_width,
                      arc_length,
                      arc_space);
  o_set_fill_options (new_obj,
                      FILLING_HOLLOW, -1, -1, -1, -1, -1);

  return new_obj;
}

/*! \brief create the string representation of an arc object
 *
 *  This function converts the arc object to a string representation for
 *  seiralization. The arc object only uses a single line.
 *
 *  The end of the returned string does not have a newline.
 *
 *  When no longer needed, free the returned string using g_free().
 *
 *  \param [in] object the arc object to convert to string representation
 *  \return the string representation of the arc object
 */
gchar*
geda_arc_object_to_buffer (const GedaObject *object)
{
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->arc != NULL, NULL);
  g_return_val_if_fail (object->type == OBJ_ARC, NULL);

  /* Describe a circle with post-20000704 file format */

  return g_strdup_printf ("%c %d %d %d %d %d %d %d %d %d %d %d",
                          OBJ_ARC,
                          geda_arc_object_get_center_x (object),
                          geda_arc_object_get_center_y (object),
                          geda_arc_object_get_radius (object),
                          geda_arc_object_get_start_angle (object),
                          geda_arc_object_get_sweep_angle (object),
                          geda_object_get_color (object),
                          object->line_width,
                          object->line_end,
                          object->line_type,
                          object->line_length,
                          object->line_space);
}

/*! \brief
 *  \par Function Description
 *  This function applies a translation of (<B>dx</B>,<B>dy</B>)
 *  to the arc described in <B>*object</B>. <B>dx</B> and <B>dy</B> are in world unit.
 *
 *  \param [in,out] object     Arc GedaObject to translate.
 *  \param [in]     dx         x distance to move.
 *  \param [in]     dy         y distance to move.
 */
void
geda_arc_object_translate (GedaObject *object, int dx, int dy)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->arc != NULL);
  g_return_if_fail (object->type == OBJ_ARC);

  /* Do world coords */
  object->arc->x = object->arc->x + dx;
  object->arc->y = object->arc->y + dy;
}

/*! \brief
 *  \par Function Description
 *  This function rotates the world coordinates of an arc of an angle
 *  specified by <B>angle</B>. The center of the rotation is given by
 *  (<B>world_centerx</B>,<B>world_centery</B>).
 *
 *  The arc is translated in order to put the center of the rotation
 *  on the origin. The center of the arc is then rotated of the angle
 *  specified by <B>angle</B>. The start angle of the arc is incremented by <B>angle</B>.
 *
 *  The arc is finally back translated to its previous location on the page.
 *
 *  <B>world_centerx</B> and <B>world_centery</B> are in world units, <B>angle</B> is in degrees.
 *
 *  \param [in] world_centerx
 *  \param [in] world_centery
 *  \param [in] angle
 *  \param [in] object
 */
void
geda_arc_object_rotate (int world_centerx,
                        int world_centery,
                        int angle,
                        OBJECT *object)
{
  int x, y, newx, newy;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->arc != NULL);
  g_return_if_fail (object->type == OBJ_ARC);

  /* translate object to origin */
  object->arc->x -= world_centerx;
  object->arc->y -= world_centery;

  /* get center, and rotate center */
  x = object->arc->x;
  y = object->arc->y;
  if(angle % 90 == 0) {
    geda_point_rotate_90 (x, y, angle % 360, &newx, &newy);
  } else {
    geda_point_rotate (x, y, angle % 360, &newx, &newy);
  }
  object->arc->x = newx;
  object->arc->y = newy;

  /* apply rotation to angles */
  object->arc->start_angle = (object->arc->start_angle + angle) % 360;

  /* translate object to its previous place */
  object->arc->x += world_centerx;
  object->arc->y += world_centery;
}

/*! \brief Mirror the WORLD coordinates of an ARC.
 *  \par Function Description
 *  This function mirrors the world coordinates of an arc.
 *  The symetry axis is given by the vertical line going through the point (<B>world_centerx</B>,<B>world_centery</B>).
 *
 *  The arc is translated in order to put the point (<B>world_centerx</B>,<B>world_centery</B>)
 *  on the origin. The center of the arc is then mirrored. The start angle of the arc
 *  and the sweep of the arc are also mirrored.
 *
 *  The arc is finally back translated to its previous location on the page.
 *
 *  \param [in] world_centerx
 *  \param [in] world_centery
 *  \param [in] object
 */
void
geda_arc_object_mirror (int world_centerx,
                        int world_centery,
			OBJECT *object)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->arc != NULL);
  g_return_if_fail (object->type == OBJ_ARC);

  /* translate object to origin */
  object->arc->x -= world_centerx;

  /* get center, and mirror it (vertical mirror) */
  object->arc->x = -object->arc->x;

  /* apply mirror to angles (vertical mirror) */
  object->arc->start_angle = (180 - object->arc->start_angle) % 360;
  /* start_angle *MUST* be positive */
  if(object->arc->start_angle < 0) object->arc->start_angle += 360;
  object->arc->sweep_angle = -object->arc->sweep_angle;

  /* translate object back to its previous position */
  object->arc->x += world_centerx;
}


/*! \brief
 *  \par Function Description
 *  This function calculates the smallest rectangle the arc can be drawn into.
 *  The <B>OBJECT</B> pointed by object is assumed to be an arc.
 *  The <B>left</B>, <B>top</B>, <B>right</B> and <B>bottom</B> pointed integers define
 *  this rectangle at the end of the function. It is expressed in world units.
 *  The process is divided into two steps : the first step is to calculate the
 *  coordinates of the two ends of the arc and the coordinates of the center.
 *  They forms a first rectangle but (depending on the start angle and the
 *  sweep of the arc) not the right.
 *
 *  \param [in]  object
 *  \param [out] left
 *  \param [out] top
 *  \param [out] right
 *  \param [out] bottom
 */
void
geda_arc_object_calculate_bounds (const OBJECT *object,
                                  gint *left,
                                  gint *top,
                                  gint *right,
                                  gint *bottom)
{
  int x1, y1, x2, y2, x3, y3;
  int radius, start_angle, sweep_angle;
  int i, angle;
  int halfwidth;

  halfwidth = object->line_width / 2;

  radius      = object->arc->radius;
  start_angle = object->arc->start_angle;
  sweep_angle = object->arc->sweep_angle;

  x1 = object->arc->x;
  y1 = object->arc->y;
  x2 = x1 + radius * cos(start_angle * M_PI / 180);
  y2 = y1 + radius * sin(start_angle * M_PI / 180);
  x3 = x1 + radius * cos((start_angle + sweep_angle) * M_PI / 180);
  y3 = y1 + radius * sin((start_angle + sweep_angle) * M_PI / 180);

  *left   = (x1 < x2) ? ((x1 < x3) ? x1 : x3) : ((x2 < x3) ? x2 : x3);
  *right  = (x1 > x2) ? ((x1 > x3) ? x1 : x3) : ((x2 > x3) ? x2 : x3);
  *bottom = (y1 > y2) ? ((y1 > y3) ? y1 : y3) : ((y2 > y3) ? y2 : y3);
  *top    = (y1 < y2) ? ((y1 < y3) ? y1 : y3) : ((y2 < y3) ? y2 : y3);

  /*! \note
   *  The previous rectangle is extended to the final one
   *  by checking whether the arc is over a main axis (vertical or horizontal).
   *  If so, the rectangle is extended in these directions.
   *
   *  In the mirror mode, the sweep angle is negativ. To get a
   *  CCW arc before this calculation we have to move the
   *  start angle to the end angle and reverse the sweep angle.
   */
  if (sweep_angle < 0) {
    start_angle = (start_angle + sweep_angle + 360) % 360;
    sweep_angle = -sweep_angle;
  }
  angle = ((int) (start_angle / 90)) * 90;
  for(i = 0; i < 4; i++) {
    angle = angle + 90;
    if(angle < start_angle + sweep_angle) {
      if(angle % 360 == 0)   *right  = x1 + radius;
      if(angle % 360 == 90)  *bottom = y1 + radius;
      if(angle % 360 == 180) *left   = x1 - radius;
      if(angle % 360 == 270) *top    = y1 - radius;
    } else {
      break;
    }
  }

  /* This isn't strictly correct, but a 1st order approximation */
  *left   -= halfwidth;
  *top    -= halfwidth;
  *right  += halfwidth;
  *bottom += halfwidth;

}

/*! \brief get the position of the center point
 *  \par Function Description
 *  This function gets the position of the center point of an arc object.
 *
 *  \param [in] object   The object to get the position.
 *  \param [out] x       pointer to the x-position
 *  \param [out] y       pointer to the y-position
 *  \return TRUE if successfully determined the position, FALSE otherwise
 */
gboolean
geda_arc_object_get_position (const GedaObject *object, gint *x, gint *y)
{
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->type == OBJ_ARC, FALSE);
  g_return_val_if_fail (object->arc != NULL, FALSE);

  if (x != NULL) {
    *x = object->arc->x;
  }

  if (y != NULL) {
    *y = object->arc->y;
  }

  return TRUE;
}

/*! \brief Calculates the distance between the given point and the closest
 * point on the perimeter of the arc.
 *
 *  \param [in] object         The arc OBJECT.
 *  \param [in] x              The x coordinate of the given point.
 *  \param [in] y              The y coordinate of the given point.
 *  \param [in] force_solid    If true, force treating the object as solid.
 *  \param [in] include_hidden Take hidden text into account.
 *  \return The shortest distance from the object to the point. With an
 *  invalid parameter, this function returns G_MAXDOUBLE.
 */
double
geda_arc_object_shortest_distance (OBJECT *object,
                                   int x,
                                   int y,
                                   int force_solid,
                                   gboolean include_hidden)
{
  double shortest_distance;
  double radius;

  g_return_val_if_fail (object->arc != NULL, G_MAXDOUBLE);

  radius = (double)object->arc->radius;

  if (geda_arc_within_sweep (object->arc, x, y)) {
    double distance_to_center;

    distance_to_center = hypot (x - object->arc->x, y - object->arc->y);

    shortest_distance = fabs (distance_to_center - radius);

  } else {
    double angle;
    double distance_to_end0;
    double distance_to_end1;
    double dx, dy;

    angle = G_PI * ((double)object->arc->start_angle) / 180;

    dx = ((double)x) - radius * cos (angle) - ((double)object->arc->x);
    dy = ((double)y) - radius * sin (angle) - ((double)object->arc->y);

    distance_to_end0 = hypot (dx, dy);

    angle += G_PI * ((double)object->arc->sweep_angle) / 180;

    dx = ((double)x) - radius * cos (angle) - ((double)object->arc->x);
    dy = ((double)y) - radius * sin (angle) - ((double)object->arc->y);

    distance_to_end1 = hypot (dx, dy);

    shortest_distance = MIN (distance_to_end0, distance_to_end1);
  }

  return shortest_distance;
}
