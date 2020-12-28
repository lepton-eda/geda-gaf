/* Lepton EDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2015 gEDA Contributors
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
#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif


#include "gschem.h"

#include <glib/gstdio.h>

#define DEFAULT_MAX_RECENT_FILES 10
#define RECENT_MENU_ITEM_NAME "Open Recen_t"

struct PopupEntry
{
  const gchar* name;
  const gchar* action;
  const gchar* stock_id;
};

static struct PopupEntry popup_items[] =
{
  { N_("Add Co_mponent..."), "&add-component", "insert-symbol" },
  { N_("Add Te_xt..."),      "&add-text",      "insert-text" },
  { N_("Add _Attribute..."), "&add-attribute", "insert-attribute" },
  { "SEPARATOR", NULL, NULL },
  { N_("Add _Net"), "&add-net", "insert-net" },
  { N_("Add _Bus"), "&add-bus", "insert-bus" },
  { "SEPARATOR", NULL, NULL },
  { N_("Cu_t"),    "&clipboard-cut",   "gtk-cut" },
  { N_("_Copy"),   "&clipboard-copy",  "gtk-copy" },
  { N_("_Paste"),  "&clipboard-paste", "gtk-paste" },
  { N_("_Delete"), "&edit-delete",     "gtk-delete" },
  { "SEPARATOR", NULL, NULL },
  { N_("_Edit..."),              "&edit-edit",              NULL },
  { N_("Ed_it Text..."),         "&edit-text",              "gtk-edit" },
  { N_("_Object Properties..."), "&edit-object-properties", "gtk-properties" },
  { "SEPARATOR", NULL, NULL },
  { N_("Hierarchy: Down _Schematic"), "&hierarchy-down-schematic", "gtk-go-down" },
  { N_("Hierarchy: Down S_ymbol"),    "&hierarchy-down-symbol",    "gtk-goto-bottom" },
  { N_("Hierarchy: _Up"),             "&hierarchy-up",             "gtk-go-up" },

  { NULL, NULL, NULL }, /* Guard */
};



static void
x_menu_attach_recent_files_submenu (GschemToplevel* w_current,
                                    GtkWidget*      menuitem);



/*! \brief Callback function for menu items. Execute action \a action.
 */
static void g_menu_execute(GtkAction *action, gpointer user_data)
{
  const gchar *action_name = gtk_action_get_name (action);
  GschemToplevel *w_current = (GschemToplevel *) user_data;
  g_action_eval_by_name (w_current, action_name);
}


static GtkWidget*
make_separator_menu_item ()
{
#ifdef ENABLE_GTK3
  return gtk_separator_menu_item_new ();
#else
  return gtk_menu_item_new();
#endif
}


static GschemAction*
make_menu_action (const char *action_name,
                  const char *menu_item_name,
                  const char *menu_item_keys,
                  const char *menu_item_stock,
                  GschemToplevel *w_current)
{
  GtkStockItem stock_info;
  GschemAction *action =
    GSCHEM_ACTION (g_object_new (GSCHEM_TYPE_ACTION,
                                 "name", action_name,
                                 "label", menu_item_name,
                                 "tooltip", menu_item_name,
                                 "multikey-accel", menu_item_keys,
                                 NULL));

  /* If stock name corresponds to a GTK stock item, then use it.
   * Otherwise, look it up in the icon theme. */
  if (menu_item_stock != NULL &&
      gtk_stock_lookup (menu_item_stock, &stock_info))
  {
    gtk_action_set_stock_id (GTK_ACTION (action), menu_item_stock);
  }
  else
  {
    gtk_action_set_icon_name (GTK_ACTION (action), menu_item_stock);
  }

  g_signal_connect (G_OBJECT (action),
                    "activate",
                    G_CALLBACK (g_menu_execute),
                    w_current);

  return action;
}


/*! \brief Create and return the main menu widget.
 *
*  \par Function Description
 *  The key/value data (see g_object_set_data()/g_object_get_data())
 *  associated with the menu widget created by this function:
 *  - key:   action name, string, e.g. "&edit-object-properties"
 *  - value: pointer to a corresponding GschemAction object
 *
 *  \todo Refactor
 */
GtkWidget*
get_main_menu (GschemToplevel* w_current)
{
  GschemAction *action;
  GtkWidget *menu_item;
  GtkWidget *root_menu;
  GtkWidget *menu_bar;
  GtkWidget *menu;
  int scm_items_len;
  SCM scm_items;
  SCM scm_item;
  SCM scm_item_name;
  SCM scm_item_func;
  SCM scm_item_stock;
  SCM scm_index;
  SCM scm_keys;
  char *menu_name;
  char *action_name;
  char **raw_menu_name = (char**) g_malloc (sizeof (char *));
  char *menu_item_name;
  char *raw_menu_item_name;
  char *menu_item_stock;
  char *menu_item_keys;
  int i, j;

  menu_bar = gtk_menu_bar_new ();

  scm_dynwind_begin ((scm_t_dynwind_flags) 0);
  g_dynwind_window (w_current);
  /*! \bug This function may leak memory if there is a non-local exit
   * in Guile code. At some point, unwind handlers need to be added to
   * clean up heap-allocated strings. */

  for (i = 0 ; i < s_menu_return_num(); i++) {
    
    scm_items = s_menu_return_entry(i, raw_menu_name);   
    if (*raw_menu_name == NULL) {
      fprintf(stderr, "Oops.. got a NULL menu name in get_main_menu()\n");
      exit(-1);
    }

    menu = gtk_menu_new();

    menu_item = gtk_tearoff_menu_item_new ();
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
    gtk_widget_show(menu_item);

    scm_items_len = (int) scm_ilength (scm_items);


    /* cycle through menu items:
    */
    for (j = 0 ; j < scm_items_len; j++)
    {
      scm_index = scm_from_int (j);

      /* menu item:
      */
      scm_item = scm_list_ref (scm_items, scm_index);

      /* check if [scm_item] is valid, it must be a
       * ( list TEXT ACTION ICON ):
      */
      if ( scm_is_false (scm_list_p (scm_item)) )
        continue;

      if ( scm_to_int (scm_length (scm_item)) != 3 )
        continue;


      scm_item_name = SCM_CAR (scm_item);
      scm_item_func = SCM_CADR (scm_item);
      scm_item_stock = scm_is_pair (SCM_CDDR (scm_item)) ?
                         SCM_CADDR (scm_item) : SCM_BOOL_F;
      SCM_ASSERT(scm_is_string(scm_item_name), scm_item_name, SCM_ARGn, "get_main_menu item_name");
      SCM_ASSERT(scm_is_symbol (scm_item_func) ||
                    scm_is_false (scm_item_func),
                 scm_item_func, SCM_ARGn, "get_main_menu item_func");
      SCM_ASSERT (scm_is_string (scm_item_stock) ||
                    scm_is_false (scm_item_stock),
                  scm_item_stock, SCM_ARGn, "get_main_menu stock");

      raw_menu_item_name = scm_to_utf8_string(scm_item_name);
      scm_dynwind_begin ((scm_t_dynwind_flags) 0);
      scm_dynwind_free(raw_menu_item_name);

      menu_item_name = (char *) gettext(raw_menu_item_name);

      if (strcmp(menu_item_name, "SEPARATOR") == 0) {
        menu_item = make_separator_menu_item ();
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
      } else {

        if(scm_is_false (scm_item_func)) {
          menu_item = gtk_menu_item_new_with_mnemonic(menu_item_name);
        } else {

          GtkStockItem stock_info;

          /* Look up key binding in global keymap */
          SCM s_expr =
            scm_list_2 (scm_from_utf8_symbol ("find-key"),
                        scm_list_2 (scm_from_utf8_symbol ("quote"),
                                    scm_item_func));

          scm_keys = g_scm_eval_protected (s_expr, scm_interaction_environment ());

          if (scm_is_false (scm_keys)) {
            menu_item_keys = (char*) "";
          } else {
            menu_item_keys = scm_to_utf8_string (scm_keys);
            scm_dynwind_free(menu_item_keys);
          }

          if (scm_is_false (scm_item_stock))
            menu_item_stock = NULL;
          else
            menu_item_stock = scm_to_utf8_string (scm_item_stock);

          action_name = scm_to_utf8_string (scm_symbol_to_string (scm_item_func));
          action = make_menu_action (action_name,
                                     menu_item_name,
                                     menu_item_keys,
                                     menu_item_stock,
                                     w_current);

          g_object_set_data (G_OBJECT (menu_bar), action_name, action);


          free(action_name);
          free(menu_item_stock);

          menu_item = gtk_action_create_menu_item (GTK_ACTION (action));

        } /* scm_item_func == TRUE */

        gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);

      } /* !separator */

      gtk_widget_show (menu_item);


      if (strcmp (raw_menu_item_name, RECENT_MENU_ITEM_NAME) == 0)
      {
        x_menu_attach_recent_files_submenu (w_current, menu_item);
      }


      scm_dynwind_end();

    } /* for j: menu items */
    
    menu_name = (char *) gettext(*raw_menu_name);
    root_menu = gtk_menu_item_new_with_mnemonic (menu_name);
    /* do not free *raw_menu_name */

    gtk_widget_show (root_menu);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), root_menu);

  } /* fot i: menus (File, Edit, ...) */

  scm_dynwind_end ();

  g_free(raw_menu_name);
  return menu_bar;

} /* get_main_menu() */



/*! \brief Create and return the popup menu widget.
 *
 *  \par Function Description
 *  The key/value data (see g_object_set_data()/g_object_get_data())
 *  associated with the menu widget created by this function:
 *  - key:   action name, string, e.g. "&edit-object-properties"
 *  - value: pointer to a corresponding GschemAction object
 */
GtkWidget*
get_main_popup (GschemToplevel* w_current)
{
  GschemAction *action;
  GtkWidget *menu_item;
  GtkWidget *menu;
  GtkStockItem stock_info;
  int i;

  menu = gtk_menu_new ();

  for (i = 0; popup_items[i].name != NULL; i++) {
    struct PopupEntry e = popup_items[i];

    /* No action --> add a separator */
    if (e.action == NULL) {
      menu_item = make_separator_menu_item ();
      gtk_widget_show (menu_item);
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
      continue;
    }

    /* Don't bother showing keybindings in the popup menu */
    action = make_menu_action (e.action,
                               gettext (e.name),
                               NULL,
                               e.stock_id,
                               w_current);

    menu_item = gtk_action_create_menu_item (GTK_ACTION (action));
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);

    g_object_set_data (G_OBJECT (menu), e.action, action);
  }

  return menu;
}

/*! \brief Show the popup menu.
 *
 *  \note
 *  need to look at this... here and the setup
 */
gint do_popup (GschemToplevel *w_current, GdkEventButton *event)
{
  GtkWidget *menu = (GtkWidget *) w_current->popup_menu;
  g_return_val_if_fail (menu != NULL, FALSE);

  gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
                  event->button, event->time);

  return FALSE;
}

/*! \brief Enable/disable menu item linked to the action \a action_name.
 *
 *  \par Function Description
 *  Use the key/value data associated with the \a menu to find
 *  an action object, and if found, set its sensitivity (\a sensitive).
 *
 *  \param menu         Menu widget (menubar, popup_menu in st_gschem_toplevel)
 *  \param action_name  Action name (e.g. "&edit-object-properties")
 *  \param sensitive    Boolean, enable or disable the action
 */
void
x_menus_sensitivity (GtkWidget*   menu,
                     const gchar* action_name,
                     gboolean     sensitive)
{
  GObject* obj = G_OBJECT (menu);
  gpointer data = g_object_get_data (obj, action_name);

  GschemAction* action = (GschemAction*) data;
  if (action != NULL)
  {
    gtk_action_set_sensitive (GTK_ACTION (action), sensitive);
  }
  else
  {
    g_debug ("x_menus_sensitivity(): cannot find action [%s]", action_name);
  }
}

/*! \brief Callback for recent-chooser.
 *
 *  \par Function Description
 *  Will be called if element of recent-file-list is activated
 */
void
recent_chooser_item_activated (GtkRecentChooser *chooser, GschemToplevel *w_current)
{
  PAGE *page;
  gchar *uri;
  gchar *filename;

  uri = gtk_recent_chooser_get_current_uri (chooser);
  filename = g_filename_from_uri(uri, NULL, NULL);
  if (w_current->recent_manager != NULL) {
    gtk_recent_manager_add_item (w_current->recent_manager, uri);
  }
  page = x_window_open_page(w_current, (char *)filename);
  x_window_set_current_page(w_current, page);

  g_free(uri);
  g_free(filename);
}



/*! \brief Attach 'Open Recent' submenu to \a menuitem.
 */
static void
x_menu_attach_recent_files_submenu (GschemToplevel* w_current,
                                    GtkWidget*      menuitem)
{
  GtkRecentFilter *recent_filter;
  GtkWidget *menuitem_file_recent_items;

  w_current->recent_manager = gtk_recent_manager_get_default ();

  menuitem_file_recent_items =
    gtk_recent_chooser_menu_new_for_manager (w_current->recent_manager);

  /* Show only schematic- and symbol-files (*.sch and *.sym) in list */
  recent_filter = gtk_recent_filter_new();
  gtk_recent_filter_add_mime_type(recent_filter, "application/x-lepton-schematic");
  gtk_recent_filter_add_mime_type(recent_filter, "application/x-lepton-symbol");
  gtk_recent_filter_add_pattern(recent_filter, "*.sch");
  gtk_recent_filter_add_pattern(recent_filter, "*.sym");
  gtk_recent_chooser_add_filter(GTK_RECENT_CHOOSER(menuitem_file_recent_items), recent_filter);

  gtk_recent_chooser_set_show_tips(GTK_RECENT_CHOOSER(menuitem_file_recent_items), TRUE);
  gtk_recent_chooser_set_sort_type(GTK_RECENT_CHOOSER(menuitem_file_recent_items),
                                   GTK_RECENT_SORT_MRU);

  /* read configuration: maximum number of recent files: */
  gchar* cwd = g_get_current_dir();
  EdaConfig* cfg = eda_config_get_context_for_path (cwd);
  g_free (cwd);

  gint max_items = DEFAULT_MAX_RECENT_FILES;

  if (cfg != NULL)
  {
    GError* err = NULL;
    gint val = eda_config_get_int (cfg, "schematic.gui", "max-recent-files", &err);

    if (err == NULL && val > 0)
    {
      max_items = val;
    }

    g_clear_error (&err);
  }

  gtk_recent_chooser_set_limit(GTK_RECENT_CHOOSER(menuitem_file_recent_items), max_items);

  gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(menuitem_file_recent_items), FALSE);
  gtk_recent_chooser_menu_set_show_numbers(GTK_RECENT_CHOOSER_MENU(menuitem_file_recent_items), TRUE);
  g_signal_connect (G_OBJECT (menuitem_file_recent_items), "item-activated",
                    G_CALLBACK (recent_chooser_item_activated), w_current);

  if (menuitem == NULL)
    return;

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), menuitem_file_recent_items);
}
