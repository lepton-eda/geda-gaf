/* Lepton EDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2010 gEDA Contributors (see ChangeLog for details)
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
 * \file gschem_macro_widget.c
 *
 * \brief A widget for entering macros
 */

#include <config.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_MATH_H
#include <math.h>
#endif

#include "gschem.h"



enum
{
  PROP_0,
  PROP_LABEL_TEXT,
  PROP_MACRO_STRING,
  PROP_TOPLEVEL
};




static void
activate_entry (GtkWidget *entry, GschemMacroWidget *widget);

static void
click_cancel (GtkWidget *button, GschemMacroWidget *widget);

static void
click_evaluate (GtkWidget *entry, GschemMacroWidget *widget);

static void
dispose (GObject *object);

static void
finalize (GObject *object);

static void
get_property (GObject *object, guint param_id, GValue *value, GParamSpec *pspec);

static void
gschem_macro_widget_class_init (GschemMacroWidgetClass *klass);

static void
gschem_macro_widget_init (GschemMacroWidget *view);

static void
set_property (GObject *object, guint param_id, const GValue *value, GParamSpec *pspec);

static void
notify_entry_text (GtkWidget *entry, GParamSpec *pspec, GschemMacroWidget *widget);

static void
history_add (GtkListStore* store, const gchar* line);

static void
history_save (GtkListStore* store);

static void
history_load (GtkListStore* store);

static void
history_truncate (GtkListStore* store);

static void
command_entry_set_font (GtkWidget* entry);



static GObjectClass *gschem_macro_widget_parent_class = NULL;



/* Callback for when the user presses enter in the entry widget
 */
static void
activate_entry (GtkWidget *entry, GschemMacroWidget *widget)
{
  g_return_if_fail (widget != NULL);

  if (gtk_entry_get_text_length (GTK_ENTRY (widget->entry)) > 0)
  {
    gtk_info_bar_response (GTK_INFO_BAR (widget), GTK_RESPONSE_OK);

    history_add  (widget->store,
                  gtk_entry_get_text (GTK_ENTRY (widget->entry)));
    history_truncate (widget->store);
    history_save (widget->store);
  }
  else
  {
    gtk_info_bar_response (GTK_INFO_BAR (widget), GTK_RESPONSE_CANCEL);
  }
}



/* Callback for when the user clicks the cancel button
 */
static void
click_cancel (GtkWidget *button, GschemMacroWidget *widget)
{
  gtk_info_bar_response (GTK_INFO_BAR (widget), GTK_RESPONSE_CANCEL);
}



/* Callback for when the user clicks the evaluate button
 */
static void
click_evaluate (GtkWidget *entry, GschemMacroWidget *widget)
{
  g_return_if_fail (widget != NULL);

  if (gtk_entry_get_text_length (GTK_ENTRY (widget->entry)) > 0)
  {
    gtk_info_bar_response (GTK_INFO_BAR (widget), GTK_RESPONSE_OK);

    history_add  (widget->store,
                  gtk_entry_get_text (GTK_ENTRY (widget->entry)));
    history_truncate (widget->store);
    history_save (widget->store);
  }
}



/*! \brief Dispose of the object
 */
static void
dispose (GObject *object)
{
  /* lastly, chain up to the parent dispose */

  g_return_if_fail (gschem_macro_widget_parent_class != NULL);
  gschem_macro_widget_parent_class->dispose (object);
}



/*! \brief Finalize object
 */
static void
finalize (GObject *object)
{
  /* lastly, chain up to the parent finalize */

  g_return_if_fail (gschem_macro_widget_parent_class != NULL);
  gschem_macro_widget_parent_class->finalize (object);
}



/*! \brief Get a property
 *
 *  \param [in]     object
 *  \param [in]     param_id
 *  \param [in,out] value
 *  \param [in]     pspec
 */
static void
get_property (GObject *object, guint param_id, GValue *value, GParamSpec *pspec)
{
  GschemMacroWidget *widget = GSCHEM_MACRO_WIDGET (object);

  switch (param_id) {
    case PROP_LABEL_TEXT:
      g_value_set_string (value, gschem_macro_widget_get_label_text (widget));
      break;

    case PROP_MACRO_STRING:
      g_value_set_string (value, gschem_macro_widget_get_macro_string (widget));
      break;

    case PROP_TOPLEVEL:
      g_value_set_pointer (value, widget->toplevel);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
  }
}



/*! \brief Initialize GschemMacroWidget class
 *
 *  \param [in] klass The class for the GschemMacroWidget
 */
static void
gschem_macro_widget_class_init (GschemMacroWidgetClass *klass)
{
  gschem_macro_widget_parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));

  G_OBJECT_CLASS (klass)->dispose  = dispose;
  G_OBJECT_CLASS (klass)->finalize = finalize;

  G_OBJECT_CLASS (klass)->get_property = get_property;
  G_OBJECT_CLASS (klass)->set_property = set_property;

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   PROP_LABEL_TEXT,
                                   g_param_spec_string ("label-text",
                                                        "Label Text",
                                                        "Label Text",
                                                        _("Macro:"),
                                                        (GParamFlags) (G_PARAM_READWRITE
                                                                       | G_PARAM_CONSTRUCT)));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
                                   PROP_MACRO_STRING,
                                   g_param_spec_string ("macro-string",
                                                        "Macro String",
                                                        "Macro String",
                                                        "",
                                                        (GParamFlags) (G_PARAM_READWRITE
                                                                       | G_PARAM_CONSTRUCT)));

  GParamFlags flags = (GParamFlags) (G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
  GParamSpec* spec  = g_param_spec_pointer ("toplevel", "", "", flags);
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_TOPLEVEL, spec);
}



/*! \brief Get the entry
 *
 *  \param [in] widget This GschemMacroWidget
 *  \return The entry
 */
GtkWidget*
gschem_macro_widget_get_entry (GschemMacroWidget *widget)
{
  g_return_val_if_fail (widget != NULL, NULL);

  return widget->entry;
}



/*! \brief Get the label text
 *
 *  \param [in] widget This GschemMacroWidget
 *  \return The label text
 */
const char*
gschem_macro_widget_get_label_text (GschemMacroWidget *widget)
{
  g_return_val_if_fail (widget != NULL, NULL);

  return gtk_label_get_text (GTK_LABEL (widget->label));
}



/*! \brief Get the macro string
 *
 *  \param [in] widget This GschemMacroWidget
 *  \return The macro string
 */
const char*
gschem_macro_widget_get_macro_string (GschemMacroWidget *widget)
{
  g_return_val_if_fail (widget != NULL, NULL);

  return gtk_entry_get_text (GTK_ENTRY (widget->entry));
}



/*! \brief Get/register GschemMacroWidget type.
 */
GType
gschem_macro_widget_get_type ()
{
  static GType type = 0;

  if (type == 0) {
    static const GTypeInfo info = {
      sizeof(GschemMacroWidgetClass),
      NULL,                                                    /* base_init */
      NULL,                                                    /* base_finalize */
      (GClassInitFunc) gschem_macro_widget_class_init,
      NULL,                                                    /* class_finalize */
      NULL,                                                    /* class_data */
      sizeof(GschemMacroWidget),
      0,                                                       /* n_preallocs */
      (GInstanceInitFunc) gschem_macro_widget_init,
    };

    type = g_type_register_static (GTK_TYPE_INFO_BAR,
                                   "GschemMacroWidget",
                                   &info,
                                   (GTypeFlags)  0);
  }

  return type;
}



/*! \brief Initialize GschemMacroWidget instance
 *
 *  \param [in,out] view the GschemMacroWidget
 */
static void
gschem_macro_widget_init (GschemMacroWidget *widget)
{
  GtkWidget *action = gtk_info_bar_get_action_area (GTK_INFO_BAR (widget));
  GtkWidget *button_box;
  GtkWidget *cancel_button;
  GtkWidget *content = gtk_info_bar_get_content_area (GTK_INFO_BAR (widget));

  g_return_if_fail (widget != NULL);

  gtk_widget_set_no_show_all (GTK_WIDGET (widget), TRUE);

  widget->label = gtk_label_new (NULL);
  gtk_widget_set_visible (widget->label, TRUE);
  gtk_box_pack_start (GTK_BOX (content), widget->label, FALSE, FALSE, 0);


  /* command history list store:
  */
  widget->store = gtk_list_store_new (1, G_TYPE_STRING);
  GtkTreeModel* model = GTK_TREE_MODEL (widget->store);


  /* command entry combo box:
  */
  widget->combo = gtk_combo_box_new_with_model_and_entry (model);

  gtk_combo_box_set_entry_text_column (GTK_COMBO_BOX (widget->combo), 0);
  gtk_box_pack_start (GTK_BOX (content), widget->combo, TRUE, TRUE, 0);
  gtk_widget_set_visible (widget->combo, TRUE);


  /* GtkEntry inside the combo box:
  */
  widget->entry = gtk_bin_get_child (GTK_BIN (widget->combo));


  /* set font for the command entry:
  */
  command_entry_set_font (widget->entry);


  /* load command history:
  */
  history_load (widget->store);
  history_truncate (widget->store);


  /* enable text completion in the command entry:
  */
  GtkEntryCompletion* comp = gtk_entry_completion_new();
  gtk_entry_completion_set_model (comp, GTK_TREE_MODEL (widget->store));
  gtk_entry_completion_set_text_column (comp, 0);
  gtk_entry_set_completion (GTK_ENTRY (widget->entry), comp);


  button_box = gtk_hbutton_box_new ();
  gtk_widget_set_visible (button_box, TRUE);
  gtk_box_pack_start (GTK_BOX (content), button_box, FALSE, FALSE, 0);

  widget->evaluate_button = gtk_button_new_with_label (_("Evaluate"));
  gtk_widget_set_sensitive (widget->evaluate_button, FALSE);
  gtk_widget_set_visible (widget->evaluate_button, TRUE);
  gtk_box_pack_start (GTK_BOX (button_box), widget->evaluate_button, FALSE, FALSE, 0);

  cancel_button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_widget_set_visible (cancel_button, TRUE);
  gtk_box_pack_start (GTK_BOX (button_box), cancel_button, FALSE, FALSE, 0);

  gtk_widget_set_no_show_all (action, TRUE);
  gtk_widget_set_visible (action, FALSE);

  g_signal_connect (G_OBJECT (widget->entry),
                    "activate",
                    G_CALLBACK (activate_entry),
                    widget);

  g_signal_connect (G_OBJECT (cancel_button),
                    "clicked",
                    G_CALLBACK (click_cancel),
                    widget);

  g_signal_connect (G_OBJECT (widget->evaluate_button),
                    "clicked",
                    G_CALLBACK (click_evaluate),
                    widget);

  g_signal_connect (G_OBJECT (widget->entry),
                    "notify::text",
                    G_CALLBACK (notify_entry_text),
                    widget);
}



/*! \brief Set the label text
 *
 *  \param [in,out] view This GschemMacroWidget
 *  \param [in]     text The label text
 */
void
gschem_macro_widget_set_label_text (GschemMacroWidget *widget, const char *text)
{
  g_return_if_fail (widget != NULL);

  gtk_label_set_text (GTK_LABEL (widget->label), text);

  g_object_notify (G_OBJECT (widget), "label-text");
}



/*! \brief Set the macro string
 *
 *  \param [in,out] view This GschemMacroWidget
 *  \param [in]     str  The macro string
 */
void
gschem_macro_widget_set_macro_string (GschemMacroWidget *widget, const char *str)
{
  g_return_if_fail (widget != NULL);

  gtk_entry_set_text (GTK_ENTRY (widget->entry), str);

  g_object_notify (G_OBJECT (widget), "macro-string");
}



/*! \brief Update the sensitivity of the evaluate button
 */
static void
notify_entry_text (GtkWidget *entry, GParamSpec *pspec, GschemMacroWidget *widget)
{
  g_return_if_fail (widget != NULL);

  gtk_widget_set_sensitive (widget->evaluate_button,
                            (gtk_entry_get_text_length (GTK_ENTRY (widget->entry)) > 0));
}



/*! \brief Set a gobject property
 */
static void
set_property (GObject *object, guint param_id, const GValue *value, GParamSpec *pspec)
{
  GschemMacroWidget *widget = GSCHEM_MACRO_WIDGET (object);

  switch (param_id) {
    case PROP_LABEL_TEXT:
      gschem_macro_widget_set_label_text (widget, g_value_get_string (value));
      break;

    case PROP_MACRO_STRING:
      gschem_macro_widget_set_macro_string (widget, g_value_get_string (value));
      break;

    case PROP_TOPLEVEL:
      widget->toplevel = GSCHEM_TOPLEVEL (g_value_get_pointer (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
  }
}



/*! \brief Add a string to history
 *
 *  \param store GtkListStore history container
 *  \param line  string to be added to history
 */
static void
history_add (GtkListStore* store, const gchar* line)
{
  g_return_if_fail (store != NULL);
  g_return_if_fail (line != NULL);

  const gint column = 0;

  /* determine the most recent entry in history:
  */
  GtkTreeIter iter;
  gchar* last = NULL;

  GtkTreeModel* mod = GTK_TREE_MODEL (store);

  if (gtk_tree_model_get_iter_first (mod, &iter))
  {
    gtk_tree_model_get (mod, &iter, 0, &last, -1);
  }

  /* do not save duplicated consequent commands:
  */
  if (last == NULL || g_strcmp0 (last, line) != 0)
  {
    /* add current command to the list store:
    */
    gtk_list_store_prepend (store, &iter);
    gtk_list_store_set (store, &iter, column, line, -1);
  }

  g_free (last);

} /* history_add() */



/*! \brief Truncate the history list
 *
 *  \par Function Description
 *  Read max history size configuration from the "history-length"
 *  key in the "schematic.macro-widget" group (USER context).
 *  If that key is absent, maximum history size will be set
 *  to MACRO_WIDGET_HISTORY_MAX (\see gschem_macro_widget.h).
 *  Truncate history to be <= maximum size.
 *
 *  \param store GtkListStore history container
 */
static void
history_truncate (GtkListStore* store)
{
  g_return_if_fail (store != NULL);

  /* default history size:
  */
  gint count_max = MACRO_WIDGET_HISTORY_MAX;

  /* get USER config context:
  */
  EdaConfig* cfg = eda_config_get_user_context();

  /* schematic.macro-widget::history-length key
   * can be changed at run-time.
   * try to reload config:
  */
  GError* error = NULL;
  eda_config_load (cfg, &error);
  g_clear_error (&error);

  /* read configuration:
  */
  gint val = eda_config_get_int (cfg,
                                 "schematic.macro-widget",
                                 "history-length",
                                 &error);

  if (error == NULL && val > 0)
  {
    count_max = val;
  }

  g_clear_error (&error);


  GtkTreeModel* mod = GTK_TREE_MODEL (store);
  gint count = gtk_tree_model_iter_n_children (mod, NULL);

  /* remove excessive items from the history store:
  */
  GtkTreeIter iter;
  for (gint i = count; i > count_max; --i)
  {
    if (gtk_tree_model_iter_nth_child (mod, &iter, NULL, i - 1))
    {
      gtk_list_store_remove (store, &iter);
    }
  }

} /* history_truncate() */



/*! \brief Save history to configuration
 *
 *  \param store GtkListStore history container
 */
static void
history_save (GtkListStore* store)
{
  g_return_if_fail (store != NULL);

  GtkTreeModel* mod = GTK_TREE_MODEL (store);
  GtkTreeIter iter;

  if (!gtk_tree_model_get_iter_first (mod, &iter))
  {
    /* the history is empty */
    return;
  }


  /* allocate an array:
  */
  gint count = gtk_tree_model_iter_n_children (mod, NULL);

  gchar** lines = (gchar**) g_malloc0_n (count + 1, sizeof(gchar*));
  lines[ count ] = NULL;

  /* add elements from the list store to [lines] array:
  */
  gint i = 0;

  do
  {
    gtk_tree_model_get (mod, &iter, 0, &lines[ i++ ], -1);
  }
  while (gtk_tree_model_iter_next (mod, &iter));


  /* get config context:
  */
  EdaConfig* ctx = eda_config_get_cache_context();

  /* write configuration:
  */
  eda_config_set_string_list(ctx,
                             "schematic.macro-widget",
                             "history",
                             (const gchar**) lines,
                             count);
  eda_config_save (ctx, NULL);


  /* free() array and its elements:
  */
  g_strfreev (lines);

} /* history_save() */



/*! \brief Load history from configuration
 *
 *  \param store GtkListStore history container
 */
static void
history_load (GtkListStore* store)
{
  g_return_if_fail (store != NULL);

  const gint column = 0;

  /* get config context:
  */
  EdaConfig* ctx = eda_config_get_cache_context();

  /* read configuration:
  */
  gsize len = 0;
  gchar** lines = eda_config_get_string_list (ctx,
                                             "schematic.macro-widget",
                                             "history",
                                             &len,
                                             NULL);

  if (lines != NULL && len > 0)
  {
    GtkTreeIter iter;

    /* populate the list store:
    */
    for (gsize i = 0; i < len; ++i)
    {
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, column, lines[i], -1);
    }

    g_strfreev (lines);
  }

} /* history_load() */



/*! \brief Set font for the command entry widget
 *
 *  \par Function Description
 *  Read custom font configuration from the "font" key
 *  in the "schematic.macro-widget" group. If the key
 *  exists and contains some font name (e.g. "Monospace 12"),
 *  set macro widget text entry's font to that value.
 *
 *
 *  \param entry  GtkEntry widget inside macro command combo box
 */
static void
command_entry_set_font (GtkWidget* entry)
{
  g_return_if_fail (entry != NULL);

  gchar* cwd = g_get_current_dir();
  EdaConfig* cfg = eda_config_get_context_for_path (cwd);
  g_free (cwd);

  if (cfg == NULL)
  {
    return;
  }

  GError* err = NULL;
  gchar* font = eda_config_get_string (cfg,
                                       "schematic.macro-widget",
                                       "font",
                                       &err);

  if (err == NULL)
  {
    PangoFontDescription* fdesc = pango_font_description_from_string (font);

    gtk_widget_modify_font (entry, fdesc);

    pango_font_description_free (fdesc);
    g_free (font);
  }

  g_clear_error (&err);

} /* command_entry_set_font() */

