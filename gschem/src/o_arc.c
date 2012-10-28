/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2011 gEDA Contributors (see ChangeLog for details)
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
#include <config.h>
#include <stdio.h>
#include <math.h>

#include "gschem.h"

#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>
#endif


/*! \brief Draw an arc on the screen.
 *  \par Function Description
 *  This function is used to draw an arc on screen. The arc is described
 *  in the object which is referred by <B>o_current</B>. The arc is displayed
 *  according to the current state, described in the GSCHEM_TOPLEVEL object
 *  pointed by <B>w_current</B>.
 *
 *  It first checkes if the object is valid or not. If not it returns
 *  and do not output anything. That should never happen though.
 *
 *  \param [in] w_current  The GSCHEM_TOPLEVEL object.
 *  \param [in] o_current  The arc OBJECT to draw.
 */
void o_arc_draw(GSCHEM_TOPLEVEL *w_current, OBJECT *o_current)
{
  if (o_current->arc == NULL) {
    return;
  }

  gschem_cairo_arc (w_current, o_current->line_width,
                               o_current->arc->x,
                               o_current->arc->y,
                               o_current->arc->width / 2,
                               o_current->arc->start_angle,
                               o_current->arc->end_angle);

  gschem_cairo_set_source_color (w_current,
                                 o_drawing_color (w_current, o_current));
  gschem_cairo_stroke (w_current, o_current->line_type,
                                  o_current->line_end,
                                  o_current->line_width,
                                  o_current->line_length,
                                  o_current->line_space);

  if (o_current->selected && w_current->draw_grips == TRUE) {
    o_arc_draw_grips (w_current, o_current);
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_arc_invalidate_rubber (GSCHEM_TOPLEVEL *w_current)
{
  int cx, cy, radius;

  WORLDtoSCREEN (w_current, w_current->first_wx, w_current->first_wy, &cx, &cy);
  radius = SCREENabs (w_current, w_current->distance);

  /* FIXME: This isn't a tight bounding box */
  o_invalidate_rect (w_current, cx - radius, cy - radius,
                                cx + radius, cy + radius);
}

/*! \brief Draw an arc described by OBJECT with translation
 *  \par Function Description
 *  This function draws the arc object described by <B>*o_current</B>
 *  translated by the vector (<B>dx</B>,<B>dy</B>).
 *  The translation vector is in screen unit.
 *
 *  The arc is displayed with the color of the object.
 *
 *  \param [in] w_current  The GSCHEM_TOPLEVEL object.
 *  \param [in] dx         Delta x coordinate for arc.
 *  \param [in] dy         Delta y coordinate for arc.
 *  \param [in] o_current  Arc OBJECT to draw.
 */
void o_arc_draw_place (GSCHEM_TOPLEVEL *w_current, int dx, int dy, OBJECT *o_current)
{
  g_return_if_fail (o_current->arc != NULL);

  gschem_cairo_arc (w_current, 0, o_current->arc->x + dx,
                                  o_current->arc->y + dy,
                                  o_current->arc->width / 2,
                                  o_current->arc->start_angle,
                                  o_current->arc->end_angle);

  gschem_cairo_set_source_color (w_current,
                                 x_color_lookup_dark (o_current->color));
  gschem_cairo_stroke (w_current, TYPE_SOLID, END_NONE, 0, -1, -1);
}

/*! \brief Start process to input a new arc.
 *  \par Function Description
 *  This function starts the process to input a new arc. Parameters for
 *  this arc are put into/extracted from the <B>w_current</B> toplevel structure.
 *  <B>w_x</B> and <B>w_y</B> are current coordinates of the pointer in screen unit.
 *
 *  First step of the arc input is to set the radius of the arc. The center
 *  of the arc is kept in (<B>w_current->first_wx</B>,<B>w_current->first_wy</B>).
 *  The radius of the arc is in <B>w_current->distance</B>.
 *
 *  \param [in] w_current  The GSCHEM_TOPLEVEL object.
 *  \param [in] w_x        Current x coordinate of pointer in world units.
 *  \param [in] w_y        Current y coordinate of pointer in world units.
 */
void o_arc_start(GSCHEM_TOPLEVEL *w_current, int w_x, int w_y)
{
  /* set the center of the arc */
  w_current->first_wx = w_x;
  w_current->first_wy = w_y;

  /* set the radius */
  w_current->distance = 0;

  /* set the start and end angles */
  w_current->second_wx = w_current->second_wy = 0;

  /* start the rubberbanding process of the radius */
  o_arc_invalidate_rubber (w_current);
  w_current->rubber_visible = 1;
}

/*! \brief End the input of an arc.
 *  \par Function Description
 *  This function ends the input of the radius of the arc.
 *  The (<B>w_x</B>,<B>w_y</B>) point is taken as the other end of the radius segment.
 *  The distance between this point and the center is the radius of the arc.
 *  <B>w_x</B> and <B>w_y</B> are in world coords.
 *
 *  At the end of this function, the center of the arc is at
 *  (<B>w_current->first_wx</B>,<B>w_current->first_wy</B>) and its radius is
 *  <B>w_current->distance</B>.
 *
 *  The two angles needs to be input to fully define the arc.
 *
 *  \param [in] w_current  The GSCHEM_TOPLEVEL object.
 *  \param [in] w_x        (unused)
 *  \param [in] w_y        (unused)
 */
void o_arc_end1(GSCHEM_TOPLEVEL *w_current, int w_x, int w_y)
{
  g_assert( w_current->inside_action != 0 );

  /* erases the previous temporary radius segment */
  /* o_arc_invalidate_rubber (w_current); */
  w_current->rubber_visible = 0;

  /* ack! zero length radius */
  if (w_current->distance == 0) {
    return;
  }

#if DEBUG
  printf("DIST: %d\n", w_current->distance);
#endif

  /* open a dialog to input the start and end angle */
  arc_angle_dialog(w_current, NULL);
}

/*! \brief Ends the process of arc input.
 *  \par Function Description
 *  The #o_arc_end4() function ends the process of the input of an arc.
 *  <B>start_angle</B> and <B>end_angle</B> are the start and end angle of the
 *  arc in degrees. The partial internal representation of the arc, i.e.
 *  the center and the radius of the arc, are converted in world units.
 *  A new object is created and linked to the object list.
 *
 *  \param [in] w_current    The GSCHEM_TOPLEVEL object.
 *  \param [in] radius       Radius of the arc
 *  \param [in] start_angle  Start of angle in degrees.
 *  \param [in] end_angle    End of angle in degrees.
 */
void o_arc_end4(GSCHEM_TOPLEVEL *w_current, int radius, 
		int start_angle, int end_angle)
{
  TOPLEVEL *toplevel = w_current->toplevel;
  OBJECT *new_obj;

  /* create, initialize and link the new arc object */
  new_obj = o_arc_new (toplevel, OBJ_ARC, GRAPHIC_COLOR,
                       w_current->first_wx, w_current->first_wy,
                       radius, start_angle, end_angle);
  s_page_append (toplevel, toplevel->page_current, new_obj);

  w_current->first_wx  = -1;
  w_current->first_wy  = -1;
  w_current->distance = 0;

  /* Call add-objects-hook */
  g_run_hook_object (w_current, "%add-objects-hook", new_obj);

  toplevel->page_current->CHANGED = 1;
  
  o_undo_savestate(w_current, UNDO_ALL);
}

/*! \brief Draw an arc using one angle modification.
 *  \par Function Description
 *  This function draws an arc according to its internal representation
 *  and allows the modification of one of its angle. The start or end
 *  angle of the arc is updated according to <B>whichone</B> with the angle
 *  that the current pointer and the arc center are making with the horizontal.
 *
 *  The previous temporary arc is erased, the angle is then computed
 *  and updated and finally a new temporary arc with the new angle is drawn.
 *
 *  The arc is internally described by :
 *  <DL>
 *    <DT>*</DT><DD>(<B>w_current->first_wx</B>,<B>w_current->first_wy</B>) as
 *                   its center.
 *    <DT>*</DT><DD><B>w_current->distance</B> as its radius.
 *    <DT>*</DT><DD><B>w_current->second_wx</B> and <B>w_current->second_wx</B> as its
 *                  start and end angle respectively.
 *  </DL>
 *
 *  \param [in] w_current  The GSCHEM_TOPLEVEL object.
 *  \param [in] w_x        Current x coordinate of pointer in world units.
 *  \param [in] w_y        Current y coordinate of pointer in world units.
 *  \param [in] whichone   Which angle to change.
 *
 *  <B>whichone</B> can have one of the following values:
 *  <DL>
 *    <DT>ARC_RADIUS</DT>
 *    <DD>at the center of the arc. This grip is used to modify
 *        the radius of the arc.
 *    <DT>ARC_START_ANGLE</DT>
 *    <DD>at one end of the arc. It corresponds to the starting
 *        angle of the arc.
 *    <DT>ARC_END_ANGLE</DT>
 *    <DD>at the other end of the arc. It corresponds to the
 *        ending angle of the arc.
 *  </DL>
 */
void o_arc_motion (GSCHEM_TOPLEVEL *w_current, int w_x, int w_y, int whichone)
{
  int diff_x, diff_y, angle_deg;

  /* erase the previous temporary arc */
  if (w_current->rubber_visible)
    o_arc_invalidate_rubber (w_current);

  if(whichone == ARC_RADIUS) {
    /*
     * The radius is taken as the biggest distance on the x and y
     * axis between the center of the arc and the mouse position.
     */		
    diff_x = abs(w_current->first_wx - snap_grid (w_current, w_x));
    diff_y = abs(w_current->first_wy - snap_grid (w_current, w_y));
    w_current->distance = max(diff_x, diff_y);
  }
  else if((whichone == ARC_START_ANGLE) || (whichone == ARC_END_ANGLE)) {
    /* compute the angle */
    diff_x = w_x - w_current->first_wx;
    diff_y = w_y - w_current->first_wy;
    angle_deg = atan2 (diff_y, diff_x) * 180 / M_PI;

    /* set the start or end angle with this angle */
    switch(whichone) {
    case ARC_START_ANGLE:
      w_current->second_wx = (angle_deg + 360) % 360;
      break;
	
    case ARC_END_ANGLE:
      w_current->second_wy = (((angle_deg + 360) % 360) -
                              w_current->second_wx + 360) % 360;
      if (w_current->which_object->arc->end_angle < 0)
        w_current->second_wy = w_current->second_wy - 360;
      if (w_current->second_wy == 0)
        w_current->second_wy = 360;
      break;
      
      // No default required - one of above two branches
      // *must* be taken - Coverity ID 201571
    }

  }
	
  /* draw the new temporary arc */
  o_arc_invalidate_rubber (w_current);
  w_current->rubber_visible = 1;
}

/*! \brief Draw arc from GSCHEM_TOPLEVEL object.
 *  \par Function Description
 *  This function draws the arc from the variables in the GSCHEM_TOPLEVEL
 *  structure <B>*w_current</B>.
 *  The center of the arc is at (<B>w_current->first_wx</B>,
 *  <B>w_current->first_wy</B>), its radius equal to <B>w_current->distance</B>,
 *  and the start and end angle are given by <B>w_current->second_wx</B> and
 *  <B>w_current->second_wy</B>.
 *
 *  \param [in] w_current  The GSCHEM_TOPLEVEL object.
 */
void o_arc_draw_rubber (GSCHEM_TOPLEVEL *w_current)
{
  double rad_angle;
  int rdx, rdy;

  gschem_cairo_arc (w_current, 0, w_current->first_wx,
                                  w_current->first_wy,
                                  w_current->distance,
                                  w_current->second_wx,
                                  w_current->second_wy);

  gschem_cairo_set_source_color (w_current,
                                 x_color_lookup_dark (SELECT_COLOR));

  /* draw the radius line */
  rad_angle = ((double) w_current->second_wx) * M_PI / 180;
  rdx = (double) w_current->distance * cos (rad_angle);
  rdy = (double) w_current->distance * sin (rad_angle);

  gschem_cairo_line (w_current, END_NONE, 0, w_current->first_wx,
                                             w_current->first_wy,
                                             w_current->first_wx + rdx,
                                             w_current->first_wy + rdy);

  gschem_cairo_stroke (w_current, TYPE_SOLID, END_NONE, 0, -1, -1);
}

/*! \brief Draw grip marks on arc.
 *  \par Function Description
 *  This function draws three grips on the center and on the ends of
 *  the arc object described by <B>*o_current</B>.
 *
 *  \param [in] w_current  The GSCHE_TOPLEVEL object.
 *  \param [in] o_current  Arc OBJECT to draw grip points on.
 */
void o_arc_draw_grips(GSCHEM_TOPLEVEL *w_current, OBJECT *o_current)
{
  double radius, start_angle, end_angle;
  int x1, y1, x2, y2, x3, y3;

  if (w_current->draw_grips == FALSE)
    return;

  /*
   * An arc has three grips:
   * <DL>
   *   <DT>*</DT><DD>one at the center that allows changes on the
   *                 radius - at (<B>x</B>,<B>y</B>).
   *   <DT>*</DT><DD>one at the start of the arc - at (<B>x1</B>,<B>y1</B>).
   *   <DT>*</DT><DD>one at the end of the arc - at (<B>x2</B>,<B>y2</B>).
   */

  x1 = o_current->arc->x;
  y1 = o_current->arc->y;

  radius      = o_current->arc->width / 2.;
  start_angle = o_current->arc->start_angle;
  end_angle   = o_current->arc->end_angle;

  x2 = x1 + radius * cos ( start_angle              * M_PI / 180);
  y2 = y1 + radius * sin ( start_angle              * M_PI / 180);
  x3 = x1 + radius * cos ((start_angle + end_angle) * M_PI / 180);
  y3 = y1 + radius * sin ((start_angle + end_angle) * M_PI / 180);

  /* draw the grip at the center */
  o_grips_draw (w_current, x1, y1);

  /* draw the grip at the start_angle end of the arc */
  o_grips_draw (w_current, x2, y2);

  /* draw the grip at the end_angle end of the arc */
  o_grips_draw (w_current, x3, y3);

}
