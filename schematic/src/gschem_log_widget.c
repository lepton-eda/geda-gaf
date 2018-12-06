/* Lepton EDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2015 gEDA Contributors
 * Copyright (C) 2017-2018 Lepton EDA Contributors
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
 * \file gschem_log_widget.c
 *
 * \brief GType class and functions to support the gschem log window.
 */

#include <config.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"


static void
changed_cb (GtkTextBuffer *buffer, GschemLogWidget *widget);

static void
class_init (GschemLogWidgetClass *klass);

static GtkTextBuffer*
create_text_buffer();

static void
instance_init (GschemLogWidget *log);

static void
log_message (GschemLogWidgetClass *klass, const gchar *message, const gchar *style);

static void
scroll_to_bottom (GtkTextBuffer* buffer, GschemLogWidget* widget);

static void
log_window_clear (GtkMenuItem* item, gpointer data);

static void
populate_popup_menu (GtkTextView* txtview,
                     GtkMenu*     menu,
                     gpointer     data);



/*!
 *  \brief Get the Log class type
 *
 *  \par Function Description
 *
 * On first call, registers the Log class with the GType dynamic type system.
 * On subsequent calls, returns the saved value from first execution.
 * \returns the type identifier for the Log class
 */
GType
gschem_log_widget_get_type ()
{
  static GType type = 0;

  if (type == 0) {
    static const GTypeInfo info = {
      sizeof(GschemLogWidgetClass),
      NULL,                                 /* base_init */
      NULL,                                 /* base_finalize */
      (GClassInitFunc) class_init,
      NULL,                                 /* class_finalize */
      NULL,                                 /* class_data */
      sizeof(GschemLogWidget),
      0,                                    /* n_preallocs */
      (GInstanceInitFunc) instance_init,
    };

    type = g_type_register_static (GSCHEM_TYPE_BIN,
                                   "GschemLogWidget",
                                   &info,
                                   (GTypeFlags) 0);
  }

  return type;
}


/*! \brief create a new status log widget
 *
 *  \return a new status log widget
 */
GtkWidget*
gschem_log_widget_new ()
{
  return GTK_WIDGET (g_object_new (GSCHEM_TYPE_LOG_WIDGET, NULL));
}


/*! \brief Add a message to the status log
 *
 *  \param [in] log_domain
 *  \param [in] log_level The severity of the message
 *  \param [in] message   The message to be displayed
 */
void
x_log_message (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message)
{
  GschemLogWidgetClass *klass = GSCHEM_LOG_WIDGET_CLASS (g_type_class_peek_static (GSCHEM_TYPE_LOG_WIDGET));
  const gchar *style;

  if (log_level & (G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_ERROR)) {
    style = "critical";
  } else if (log_level & G_LOG_LEVEL_WARNING) {
    style = "warning";
  } else {
    style = "message";
  }

  log_message (klass, message, style);
}



/*!
 *  \brief Add a message to the log window
 *
 *  \par Function Description
 *  \param [in] log The log instance
 *  \param [in] message The message to be logged
 *  \param [in] style   The style to use in the text rendering
 */
static void
log_message (GschemLogWidgetClass *klass, const gchar *message, const gchar *style)
{
  GtkTextIter iter;

  g_return_if_fail (klass != NULL);
  g_return_if_fail (klass->buffer != NULL);

  gtk_text_buffer_get_end_iter (klass->buffer, &iter);
  /* Apply the "plain" tag before the level-specific tag in order to
   * reset the formatting */

  if (g_utf8_validate (message, -1, NULL)) {
    gtk_text_buffer_insert_with_tags_by_name (klass->buffer, &iter, message, -1,
                                              "plain", style, NULL);
    gtk_text_buffer_insert (klass->buffer, &iter, "\n", -1);
  } else {
    /* If UTF-8 wasn't valid (due to a system locale encoded filename or
     * other string being included by mistake), log a warning, and print
     * the original message to stderr, where it may be partly intelligible */
    gtk_text_buffer_insert_with_tags_by_name (klass->buffer, &iter,
      _("** Invalid UTF-8 in log message. See stderr or gschem.log.\n"),
                                              -1, "plain", style, NULL);
    fprintf (stderr, "%s", message);
  }
}


/*! \brief callback for changes in the text buffer
 *
 *  Changes in the contents of the buffer cause all text view widgets to scroll
 *  to the bottom.
 *
 *  \param [in] buffer the text buffer triggering the event
 *  \param [in] widget the widget to scroll to the bottom
 */
static void
changed_cb (GtkTextBuffer *buffer, GschemLogWidget *widget)
{
  g_return_if_fail (buffer != NULL);
  g_return_if_fail (widget != NULL);
  g_return_if_fail (widget->viewer != NULL);

  scroll_to_bottom (buffer, widget);
}


/*! \brief initialize class
 */
static void
class_init (GschemLogWidgetClass *klass)
{
  gchar *contents;
/*   GObjectClass *gobject_class = G_OBJECT_CLASS (klass); */

  klass->buffer = create_text_buffer ();

  /* make it read the content of the current log file */
  /* and add its contents to the dialog */
  contents = s_log_read ();

  /* s_log_read can return NULL if the log file cannot be written to */
  if (contents != NULL) {
    log_message (klass, contents, "old");
    g_free (contents);

    x_log_update_func = x_log_message;
  }
}


/*! \brief create the text buffer for storing the status log contents
 *
 *  \return a GtkTextBuffer for storing the status log
 */
static GtkTextBuffer*
create_text_buffer()
{
  GtkTextBuffer *buffer = gtk_text_buffer_new (NULL);

  /* Add some tags for highlighting log messages to the buffer */
  gtk_text_buffer_create_tag (buffer,
                              "plain",
                              "foreground", "black",
                              "foreground-set", TRUE,
                              "weight", PANGO_WEIGHT_NORMAL,
                              "weight-set", TRUE,
                              NULL);

  /* The default "message" style is plain */
  gtk_text_buffer_create_tag (buffer, "message", NULL);

  /* "old" messages are in dark grey */
  gtk_text_buffer_create_tag (buffer,
                              "old",
                              "foreground", "#404040",
                              "foreground-set", TRUE,
                              NULL);

  /* "warning" messages are printed in red */
  gtk_text_buffer_create_tag (buffer,
                              "warning",
                              "foreground", "red",
                              "foreground-set", TRUE,
                              NULL);

  /* "critical" messages are bold red */
  gtk_text_buffer_create_tag (buffer,
                              "critical",
                              "foreground", "red",
                              "foreground-set", TRUE,
                              "weight", PANGO_WEIGHT_BOLD,
                              "weight-set", TRUE,
                              NULL);

  return buffer;
}



/*! \brief initialize instance
 *
 *  \param [in] widget an instance of the widget
 */
static void
instance_init (GschemLogWidget *widget)
{
  GschemLogWidgetClass *klass = GSCHEM_LOG_WIDGET_GET_CLASS (widget);
  GtkWidget *scrolled;

  g_return_if_fail (klass != NULL);
  g_return_if_fail (klass->buffer != NULL);
  g_return_if_fail (widget != NULL);

  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (widget), scrolled);

  /* show scrollbars only when needed: */
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);

  widget->viewer = GTK_TEXT_VIEW (g_object_new (GTK_TYPE_TEXT_VIEW,
                                                /* GtkTextView */
                                                "buffer",   klass->buffer,
                                                "editable", FALSE,
                                                NULL));


  /*
  * Set custom font for the log window
  *
  * Configuration setting description:
  * key:           font
  * group:         schematic.log-window
  * type:          string
  * default value: not set
  *
  * Example:
  * [schematic.log-window]
  * font=Droid Sans Mono 11
  *
  */

  gchar* cwd = g_get_current_dir();
  EdaConfig* cfg = eda_config_get_context_for_path (cwd);
  g_free (cwd);

  if (cfg != NULL)
  {
    GError* err = NULL;
    gchar* font = eda_config_get_string (cfg,
                                         "schematic.log-window",
                                         "font",
                                         &err);

    if (err == NULL)
    {
      PangoFontDescription* fdesc = pango_font_description_from_string (font);

      gtk_widget_modify_font (GTK_WIDGET (widget->viewer), fdesc);

      pango_font_description_free (fdesc);
      g_free (font);
    }

    g_clear_error (&err);
  }


  gtk_container_add (GTK_CONTAINER (scrolled), GTK_WIDGET (widget->viewer));


  g_signal_connect (klass->buffer,
                    "changed",
                    G_CALLBACK (&changed_cb),
                    widget);

  g_signal_connect (widget->viewer,
                    "populate-popup",
                    G_CALLBACK (&populate_popup_menu),
                    NULL);


  scroll_to_bottom (klass->buffer, widget);

} /* instance_init() */



/*! \brief scroll to the bottom of the log window
 *
 *  \param [in] buffer  The text buffer
 *  \param [in] widget  The log widget
 */
static void
scroll_to_bottom (GtkTextBuffer* buffer, GschemLogWidget* widget)
{
  g_return_if_fail (buffer != NULL);
  g_return_if_fail (widget != NULL);
  g_return_if_fail (widget->viewer != NULL);

  /* gtk_text_view_scroll_to_iter()
   * relies upon the results of computations
   * performed in an idle handler.
   * Give that handler an opportunity to run -
   * process pending events before the call, - so that
   * the view will be scrolled correctly (to the bottom).
   *
   * \note using gtk_text_view_scroll_to_mark() instead
   * of gtk_text_view_scroll_to_iter() (as suggested in the
   * documentation of gtk_text_view_scroll_to_iter() doesn't help.
  */
  while (gtk_events_pending())
    gtk_main_iteration();

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter (buffer, &iter);

  gtk_text_view_scroll_to_iter (widget->viewer,
                                &iter,
                                0.0,  /* within_margin */
                                TRUE, /* use_align */
                                0.0,  /* xalign: 0 => left */
                                1.0); /* yalign: 1 => bottom */

} /* scroll_to_bottom() */



/*! \brief "activate" signal handler for "clear log window" menu item
 *
 *  \par Function Description
 *  Deletes content of the log text view
 *
 *  \param item     menu item
 *  \param data     user data (log text view widget)
 */
static void
log_window_clear (GtkMenuItem* item, gpointer data)
{
  GtkTextView* txtview = (GtkTextView*) data;
  g_return_if_fail (txtview != NULL);

  GtkTextBuffer* buffer = gtk_text_view_get_buffer (txtview);
  g_return_if_fail (buffer != NULL);

  GtkTextIter start;
  gtk_text_buffer_get_start_iter (buffer, &start);

  GtkTextIter end;
  gtk_text_buffer_get_end_iter (buffer, &end);


  GtkWidget* dlg = gtk_message_dialog_new (NULL,
                                           (GtkDialogFlags) 0,
                                           GTK_MESSAGE_QUESTION,
                                           GTK_BUTTONS_OK_CANCEL,
                                           _("Clear log window?"));

  gtk_dialog_set_default_response (GTK_DIALOG (dlg), GTK_RESPONSE_CANCEL);

  gtk_dialog_set_alternative_button_order (GTK_DIALOG (dlg),
                                           GTK_RESPONSE_OK,
                                           GTK_RESPONSE_CANCEL,
                                           -1);


  if (gtk_dialog_run (GTK_DIALOG (dlg)) == GTK_RESPONSE_OK)
  {
    gtk_text_buffer_delete (buffer, &start, &end);
  }

  gtk_widget_destroy (dlg);

} /* log_window_clear() */



/*! \brief "populate-popup" signal handler for the log text view
 *
 *  \par Function Description
 *  Extends popup menu for the log text view
 *
 *  \param txtview  log text view widget
 *  \param menu     context menu to be extended
 *  \param data     user data (not used)
 */
static void
populate_popup_menu (GtkTextView* txtview,
                     GtkMenu*     menu,
                     gpointer     data)
{
  /* Add separator:
  */
  GtkWidget* separ = gtk_separator_menu_item_new();
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), separ);
  gtk_widget_show (separ);

  /* Add "Clear Log Window" menu item:
  */
  GtkWidget* item = gtk_menu_item_new_with_mnemonic (_("Clear Log _Window"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
  gtk_widget_show (item);

  g_signal_connect (G_OBJECT (item),
                    "activate",
                    G_CALLBACK (&log_window_clear),
                    txtview);

} /* populate_popup_menu() */

