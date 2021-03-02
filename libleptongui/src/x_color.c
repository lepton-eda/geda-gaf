/* Lepton EDA Schematic Capture
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

#include <config.h>
#include "gschem.h"



extern GedaColorMap display_colors;
extern GedaColorMap display_outline_colors;



/*! \brief Initializes the display and outline color maps to defaults.
 */
void
x_color_init()
{
  lepton_color_map_init (display_colors);
  lepton_color_map_init (display_outline_colors);
}



/*! \brief Get a display color map color for specified \a color_id as GdkColor.
 *
 *  \note Caller must gdk_color_free() the returned value.
 */
GdkColor*
x_color_lookup_gdk (size_t color_id)
{
  GedaColor* color = x_color_lookup (color_id);

  /* Extrapolate 8-bpp color into 16-bpp GDK color:
  */
  GdkColor color_gdk;
  color_gdk.red   = color->r + (color->r << 8);
  color_gdk.green = color->g + (color->g << 8);
  color_gdk.blue  = color->b + (color->b << 8);

  return gdk_color_copy (&color_gdk);
}



/*! \brief Get a color for specified \a color_id from the display color map.
 */
GedaColor*
x_color_lookup (size_t color_id)
{
  g_return_val_if_fail (color_id_valid (color_id),
                        &display_colors[ default_color_id() ]);


  return &display_colors[ color_id ];
}



/*! \brief Whether a color \a color_id is enabled in the display color map.
 */
gboolean
x_color_display_enabled (size_t color_id)
{
  return display_colors[ color_id ].enabled;
}



/*! \brief: Change a color in the display color map
 */
void
x_color_set_display (size_t color_id, GdkColor* color)
{
  display_colors[ color_id ].r = color->red   >> 8;
  display_colors[ color_id ].g = color->green >> 8;
  display_colors[ color_id ].b = color->blue  >> 8;
}



/*! \brief: Change a color in the outline color map
 */
void
x_color_set_outline (size_t color_id, GdkColor* color)
{
  display_outline_colors[ color_id ].r = color->red   >> 8;
  display_outline_colors[ color_id ].g = color->green >> 8;
  display_outline_colors[ color_id ].b = color->blue  >> 8;
}



/*! \brief: Generate Scheme representation of a color map
 *
 *  \param  cmap   A color map
 *  \return        Scheme code as a string
 */
static GString*
x_color_map2str (GedaColorMap cmap)
{
  GString* str = g_string_new(NULL);

  g_string_append (str, "'(\n");

  for (size_t color_index = 0; color_index < colors_count(); color_index++)
  {
    GedaColor color = cmap[ color_index ];

    const gchar* scm_str = color_get_name (color_index);

    if (color.enabled)
    {
      guint8 r = color.r;
      guint8 g = color.g;
      guint8 b = color.b;

      /* the line will look like:
       * (background "#AABBCC")
      */
      g_string_append_printf (str, "  (%-20s \"#%.2x%.2x%.2x\")",
                             scm_str,
                             r, g, b);
    }
    else
    {
      g_string_append_printf (str, "  (%-20s #f)", scm_str);
    }

    g_string_append (str, "\n");

  } /* for */

  g_string_append (str, ")");

  return str;

} /* x_color_map2str() */



/*! \brief: Generate Scheme code for display color map
 *
 *  \return Scheme code as a string
 */
GString*
x_color_map2str_display()
{
  return x_color_map2str (display_colors);
}



/*! \brief: Generate Scheme code for outline color map
 *
 *  \return Scheme code as a string
 */
GString*
x_color_map2str_outline()
{
  return x_color_map2str (display_outline_colors);;
}
