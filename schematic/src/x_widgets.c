/* Lepton EDA Schematic Capture
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2016 gEDA Contributors (see ChangeLog for details)
 * Copyright (C) 2017 dmn <graahnul.grom@gmail.com>
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

/*!
 * \file x_widgets.c
 *
 * \brief Widgets management
 *
 * Now there are 5 of them:
 * - in right dock:
 *   - obj props
 *   - txt props
 *   - options
 * - in bottom dock:
 *   - find text results
 *   - log
 */

#include "gschem.h"




static void
x_widgets_show_in_dock (GtkWidget* wbook, GtkWidget* widget);

static void
x_widgets_show_in_dialog (GschemToplevel* w_current,
                          GtkWidget* widget,
                          GtkWidget** dialog,
                          const gchar* title,
                          const gchar* ini_group);

static void
x_widgets_dialog_response (GtkWidget* dlg, gint response, void* data);




void x_widgets_show_options (GschemToplevel* w_current)
{
  g_return_if_fail (w_current != NULL);

  x_widgets_show_in_dock (w_current->right_notebook,
                          w_current->options_widget);
}



void x_widgets_show_text_properties (GschemToplevel* w_current)
{
  g_return_if_fail (w_current != NULL);

  x_widgets_show_in_dock (w_current->right_notebook,
                          w_current->text_properties);

  gschem_text_properties_widget_adjust_focus(
    GSCHEM_TEXT_PROPERTIES_WIDGET (w_current->text_properties));
}



void x_widgets_show_object_properties (GschemToplevel* w_current)
{
  g_return_if_fail (w_current != NULL);

  x_widgets_show_in_dock (w_current->right_notebook,
                          w_current->object_properties);
}



void x_widgets_show_log (GschemToplevel* w_current)
{
  g_return_if_fail (w_current != NULL);

  x_widgets_show_in_dock (w_current->bottom_notebook,
                          w_current->log_widget);
}



void x_widgets_show_find_text_state (GschemToplevel* w_current)
{
  g_return_if_fail (w_current != NULL);

  x_widgets_show_in_dock (w_current->bottom_notebook,
                          w_current->find_text_state);
}




static void
x_widgets_show_in_dock (GtkWidget* wbook, GtkWidget* widget)
{
  g_return_if_fail (wbook != NULL);
  g_return_if_fail (widget != NULL);

  GtkNotebook* nbook = GTK_NOTEBOOK (wbook);

  int page = gtk_notebook_page_num (nbook, widget);
  if (page >= 0)
  {
    gtk_notebook_set_current_page (nbook, page);
    gtk_widget_set_visible (wbook, TRUE);
  }
}



static void
x_widgets_show_in_dialog (GschemToplevel* w_current,
                          GtkWidget* widget,
                          GtkWidget** dialog,
                          const gchar* title,
                          const gchar* ini_group)
{
  g_return_if_fail (w_current != NULL);
  g_return_if_fail (widget != NULL);

  if (*dialog != NULL)
  {
    gtk_window_present (GTK_WINDOW (*dialog));
    return;
  }

  GtkWidget* dlg = gschem_dialog_new_with_buttons(
    title,
    GTK_WINDOW (w_current->main_window),
    0,
    ini_group,
    w_current,
    GTK_STOCK_CLOSE, GTK_RESPONSE_NONE,
    NULL);

  g_signal_connect (G_OBJECT (dlg),
                    "response",
                    G_CALLBACK (&x_widgets_dialog_response),
                    NULL);

  g_signal_connect (G_OBJECT (dlg),
                    "delete-event",
                    G_CALLBACK (&gtk_widget_hide_on_delete),
                    NULL);

  GtkWidget* content_area = gtk_dialog_get_content_area (GTK_DIALOG (dlg));
  gtk_container_add (GTK_CONTAINER (content_area), widget);

  gtk_widget_show_all (dlg);
  gtk_window_present (GTK_WINDOW (dlg));

  *dialog = dlg;

} /* x_widgets_show_in_dialog() */



static void
x_widgets_dialog_response (GtkWidget* dlg, gint response, void* data)
{
  gtk_widget_set_visible (dlg, FALSE);
}
