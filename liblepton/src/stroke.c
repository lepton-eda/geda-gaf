/* Lepton EDA library
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2015 gEDA Contributors
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

/*! \file stroke.c
 *  \brief Functions for dealing with object's line stroke.
 */

#include "config.h"

#include "liblepton_priv.h"

/*! \brief Init a new #LeptonStroke.
 */
LeptonStroke*
lepton_stroke_new ()
{
  LeptonStroke *stroke;

  stroke = g_new (LeptonStroke, 1);
  stroke->cap_type = END_NONE;
  stroke->type = TYPE_SOLID;
  stroke->width = 0;
  stroke->dash_length = 0;
  stroke->space_length = 0;

  return stroke;
}

/*! \brief Free a #LeptonStroke.
 */
void
lepton_stroke_free (LeptonStroke * stroke)
{
  g_free (stroke);
}


/*! \brief Get the type of a stroke.
 *
 *  \param [in] stroke The stroke.
 *  \return type The line type of the stroke.
 */
LeptonStrokeType
lepton_stroke_get_type (const LeptonStroke *stroke)
{
  g_return_val_if_fail (stroke != NULL, TYPE_SOLID);

  return stroke->type;
}

/*! \brief Set the type of a stroke.
 *
 *  \param [in] stroke The stroke.
 *  \param [in] type   The new stroke type.
 */
void
lepton_stroke_set_type (LeptonStroke *stroke,
                        LeptonStrokeType type)
{
  g_return_if_fail (stroke != NULL);

  stroke->type = type;
}


/*! \brief Get the cap type of a stroke.
 *
 *  \param [in] stroke The stroke.
 *  \return The cap type of the stroke.
 */
LeptonStrokeCapType
lepton_stroke_get_cap_type (const LeptonStroke *stroke)
{
  g_return_val_if_fail (stroke != NULL, END_NONE);

  return stroke->cap_type;
}

/*! \brief Set the cap_type of a stroke.
 *
 *  \param [in] stroke   The stroke.
 *  \param [in] cap_type The new stroke cap type.
 */
void
lepton_stroke_set_cap_type (LeptonStroke *stroke,
                            LeptonStrokeCapType cap_type)
{
  g_return_if_fail (stroke != NULL);

  stroke->cap_type = cap_type;
}


/*! \brief Get the width of a stroke.
 *
 *  \param [in] stroke The stroke.
 *  \return The width of the stroke.
 */
int
lepton_stroke_get_width (const LeptonStroke *stroke)
{
  g_return_val_if_fail (stroke != NULL, END_NONE);

  return stroke->width;
}

/*! \brief Set the width of a stroke.
 *
 *  \param [in] stroke The stroke.
 *  \param [in] width  The new stroke width.
 */
void
lepton_stroke_set_width (LeptonStroke *stroke,
                         int width)
{
  g_return_if_fail (stroke != NULL);

  stroke->width = width;
}


/*! \brief Get the dash length of a stroke.
 *
 *  \param [in] stroke The stroke.
 *  \return The dash length of the stroke.
 */
int
lepton_stroke_get_dash_length (const LeptonStroke *stroke)
{
  g_return_val_if_fail (stroke != NULL, END_NONE);

  return stroke->dash_length;
}

/*! \brief Set the dash length of a stroke.
 *
 *  \param [in] stroke      The stroke.
 *  \param [in] dash_length The new stroke dash length.
 */
void
lepton_stroke_set_dash_length (LeptonStroke *stroke,
                               int dash_length)
{
  g_return_if_fail (stroke != NULL);

  stroke->dash_length = dash_length;
}


/*! \brief Get the space length of a stroke.
 *
 *  \param [in] stroke The stroke.
 *  \return The space length of the stroke.
 */
int
lepton_stroke_get_space_length (const LeptonStroke *stroke)
{
  g_return_val_if_fail (stroke != NULL, END_NONE);

  return stroke->space_length;
}

/*! \brief Set the space length of a stroke.
 *
 *  \param [in] stroke       The stroke.
 *  \param [in] space_length The new stroke space length.
 */
void
lepton_stroke_set_space_length (LeptonStroke *stroke,
                                int space_length)
{
  g_return_if_fail (stroke != NULL);

  stroke->space_length = space_length;
}
