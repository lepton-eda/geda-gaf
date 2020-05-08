G_BEGIN_DECLS

/* a_basic.c */
int
o_save (const GList *object_list,
        const char *filename,
        GError **err);

GList *o_read_buffer(TOPLEVEL *toplevel, GList *object_list, char *buffer, const int size, const char *name, GError **err);
GList *o_read(TOPLEVEL *toplevel, GList *object_list, char *filename, GError **err);

/* f_basic.c */
gchar *f_get_autosave_filename (const gchar *filename);
gboolean f_has_active_autosave (const gchar *filename, GError **err);
int f_open(TOPLEVEL *toplevel, PAGE *page, const gchar *filename, GError **err);
int f_open_flags(TOPLEVEL *toplevel, PAGE *page, const gchar *filename,
                 const gint flags, GError **err);
void f_close(TOPLEVEL *toplevel);

int
f_save (PAGE *page,
        const char *filename,
        GError **error);

gchar *f_normalize_filename (const gchar *filename, GError **error) G_GNUC_WARN_UNUSED_RESULT;
char *follow_symlinks (const gchar *filename, GError **error);

/* g_basic.c */
SCM g_scm_eval_protected (SCM exp, SCM module_or_state);
SCM g_scm_eval_string_protected (SCM str);
SCM g_scm_c_eval_string_protected (const gchar *str);
gboolean g_read_file(TOPLEVEL *toplevel, const gchar *filename, GError **err);

/* g_rc.c */
gboolean g_rc_parse_system (TOPLEVEL *toplevel, const gchar *rcname, GError **err);
gboolean g_rc_parse_user (TOPLEVEL *toplevel, const gchar *rcname, GError **err);
gboolean g_rc_parse_local (TOPLEVEL *toplevel, const gchar *rcname, const gchar *path, GError **err);
gboolean g_rc_load_cache_config (TOPLEVEL* toplevel, GError** err);
void g_rc_parse(TOPLEVEL *toplevel, const gchar* pname, const gchar* rcname, const gchar* rcfile);
void g_rc_parse_handler (TOPLEVEL *toplevel, const gchar *rcname, const gchar *rcfile, ConfigParseErrorFunc handler, void *user_data);
SCM g_rc_rc_filename();
SCM g_rc_parse_rc (SCM pname_s, SCM rcname_s);

/* liblepton.c */
void liblepton_init(void);
void set_guile_compiled_path();
char* version_message();

/* m_hatch.c */
void m_hatch_box(GedaBox *box, gint angle, gint pitch, GArray *lines);
void m_hatch_circle(GedaCircle *circle, gint angle, gint pitch, GArray *lines);
void m_hatch_path(GedaPath *path, gint angle, gint pitch, GArray *lines);

/* m_polygon.c */
void m_polygon_append_bezier(GArray *points, GedaBezier *bezier, int segments);
void m_polygon_append_point(GArray *points, int x, int y);

/* o_attrib.c */
void
o_attrib_add (OBJECT *object,
              OBJECT *item);

void
o_attrib_attach (OBJECT *attrib,
                 OBJECT *object,
                 int set_color);

void
o_attrib_attach_list (GList *attr_list,
                      OBJECT *object,
                      int set_color);

void
o_attrib_detach_all (OBJECT *object);

void o_attrib_print(GList *attributes);

void
o_attrib_remove (GList **list,
                 OBJECT *remove);

gboolean o_attrib_string_get_name_value (const gchar *string, gchar **name_ptr, gchar **value_ptr);
gboolean o_attrib_get_name_value (const OBJECT *attrib, gchar **name_ptr, gchar **value_ptr);
const char *o_attrib_get_name (const OBJECT *attrib);
GList *o_attrib_find_floating_attribs (const GList *list);
char *o_attrib_search_floating_attribs_by_name (const GList *list, const char *name, int counter);
char *o_attrib_search_attached_attribs_by_name (OBJECT *object, const char *name, int counter);
char *o_attrib_search_inherited_attribs_by_name (OBJECT *object, const char *name, int counter);
char *o_attrib_search_object_attribs_by_name (OBJECT *object, const char *name, int counter);
GList *o_attrib_return_attribs(OBJECT *object);
int o_attrib_is_inherited(const OBJECT *attrib);
gboolean o_attrib_is_attrib (const OBJECT *attrib);

/* o_embed.c */
void o_embed (OBJECT *o_current);
void o_unembed (OBJECT *o_current);

/* o_selection.c */
SELECTION *o_selection_new( void );

void
o_selection_add (SELECTION *selection,
                 OBJECT *o_selected);

void o_selection_print_all(const SELECTION *selection);

void
o_selection_remove (SELECTION *selection,
                    OBJECT *o_selected);

/* s_attrib.c */
int s_attrib_add_entry(char *new_attrib);
void s_attrib_print(void);
int s_attrib_uniq(char *name);
void s_attrib_free(void);
void s_attrib_init(void);
char *s_attrib_get(int counter);

/* s_basic.c */
gchar *s_expand_env_variables (const gchar *string);

/* s_clib.c */
void s_clib_free (void);
GList *s_clib_get_sources (const gboolean sorted);
const CLibSource *s_clib_get_source_by_name (const gchar *name);
void s_clib_refresh ();
const CLibSource *s_clib_add_directory (const gchar *directory,
					const gchar *name);
const CLibSource *s_clib_add_command (const gchar *list_cmd,
                                      const gchar *get_cmd,
				      const gchar *name);
const CLibSource *s_clib_add_scm (SCM listfunc, SCM getfunc,
				  const gchar *name);
const gchar *s_clib_source_get_name (const CLibSource *source);
GList *s_clib_source_get_symbols (const CLibSource *source);
const gchar *s_clib_symbol_get_name (const CLibSymbol *symbol);
gchar *s_clib_symbol_get_filename (const CLibSymbol *symbol);
const CLibSource *s_clib_symbol_get_source (const CLibSymbol *symbol);
gchar *s_clib_symbol_get_data (const CLibSymbol *symbol);
GList *s_clib_search (const gchar *pattern, const CLibSearchMode mode);
void s_clib_flush_search_cache ();
void s_clib_flush_symbol_cache ();
void s_clib_symbol_invalidate_data (const CLibSymbol *symbol);
const CLibSymbol *s_clib_get_symbol_by_name (const gchar *name);
gchar *s_clib_symbol_get_data_by_name (const gchar *name);
GList *s_toplevel_get_symbols (const TOPLEVEL *toplevel);

/* s_conn.c */
void s_conn_remove_object_connections (OBJECT *to_remove);
void s_conn_update_object (PAGE* page, OBJECT *object);
int s_conn_net_search(OBJECT* new_net, int whichone, GList * conn_list);
GList *s_conn_return_others(GList *input_list, OBJECT *object);

/* s_hierarchy.c */
PAGE *s_hierarchy_down_schematic_single(TOPLEVEL *toplevel, const gchar *filename, PAGE *parent, int page_control, int flag, GError **err);
void s_hierarchy_down_symbol (TOPLEVEL *toplevel, const CLibSymbol *symbol, PAGE *parent);
PAGE *s_hierarchy_find_up_page(GedaPageList *page_list, PAGE *current_page);
PAGE* s_hierarchy_load_subpage (PAGE *page, const char *filename, GError **error);
GList* s_hierarchy_traversepages(TOPLEVEL *toplevel, PAGE *p_current, gint flags);
gint s_hierarchy_print_page(PAGE *p_current, void * data);
PAGE *s_hierarchy_find_prev_page(GedaPageList *page_list, PAGE *current_page);
PAGE *s_hierarchy_find_next_page(GedaPageList *page_list, PAGE *current_page);

/* s_log.c */
void s_log_init (const gchar *filename);
void s_log_close (void);
gchar *s_log_read (void);

/* s_menu.c */
int s_menu_return_num(void);
SCM s_menu_return_entry(int index, char **menu_name);
int s_menu_add_entry(char *new_menu, SCM menu_items);
void s_menu_print(void);
void s_menu_free(void);
void s_menu_init(void);

/* s_slot.c */
char *s_slot_search_slot(OBJECT *object, OBJECT **return_found);

void
s_slot_update_object (OBJECT *object);

/* s_textbuffer.c */
TextBuffer *s_textbuffer_new (const gchar *data, const gint size, const gchar* name);
TextBuffer *s_textbuffer_free (TextBuffer *tb);
const gchar *s_textbuffer_next (TextBuffer *tb, const gssize count);
const gchar *s_textbuffer_next_line (TextBuffer *tb);
gsize s_textbuffer_linenum (TextBuffer* tb);

/* i_vars.c */


/* \struct OptionStringInt
 * \brief  A mapping of a string option's value to an int value.
 */
struct OptionStringInt
{
  const gchar* str_; /* a string value of an option */
  gint         int_; /* an int value of an option */
};

gboolean
cfg_read_bool (const gchar* group,
               const gchar* key,
               gboolean defval,
               gboolean* result);

gboolean
cfg_read_int (const gchar* group,
              const gchar* key,
              gint defval,
              gint* result);

gboolean
cfg_check_int_not_0 (gint val);

gboolean
cfg_check_int_greater_0 (gint val);

gboolean
cfg_check_int_greater_eq_0 (gint val);

gboolean
cfg_check_int_text_size (gint val);

gboolean
cfg_read_int_with_check (const gchar* group,
                         const gchar* key,
                         gint         defval,
                         gint*        result,
                         gboolean     (*pfn_check)(int));

gboolean
cfg_read_string2int (const gchar* group,
                     const gchar* key,
                     gint         defval,
                     const struct OptionStringInt* vals,
                     size_t       nvals,
                     gint*        result);

G_END_DECLS
