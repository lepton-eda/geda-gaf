/* System headers which libgeda headers rely on */
#include <glib.h>
#include <glib-object.h>
#include <libguile.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gstdio.h>

/* Public libgeda headers */
#define __LIBLEPTON_INTERNAL_API__ 1

#include "defines.h"

#include "geda_color.h"
#include "geda_fill_type.h"
#include "geda_line_type.h"
#include "geda_line_cap_type.h"
#include "geda_point.h"
#include "geda_string.h"

#include "geda_angle.h"
#include "geda_arc.h"
#include "geda_bezier.h"
#include "geda_bounds.h"
#include "geda_box.h"
#include "geda_circle.h"
#include "geda_component.h"
#include "geda_coord.h"
#include "geda_line.h"
#include "geda_path.h"
#include "geda_picture.h"
#include "geda_text.h"
#include "geda_transform.h"

#include "geda_forward.h"

#include "struct.h"
#include "geda_object.h"
#include "geda_object_list.h"
#include "geda_page.h"
#include "geda_toplevel.h"
#include "geda_undo.h"

#include "geda_arc_object.h"
#include "geda_box_object.h"
#include "geda_bus_object.h"
#include "geda_circle_object.h"
#include "geda_component_object.h"
#include "geda_line_object.h"
#include "geda_net_object.h"
#include "geda_path_object.h"
#include "geda_picture_object.h"
#include "geda_pin_object.h"
#include "geda_text_object.h"

#include "globals.h"
#include "o_types.h"
#include "prototype.h"
#include "funcs.h"
#include "edaconfig.h"
#include "edaerrors.h"
#include "edapaths.h"

#include "geda_list.h"

/* Private libgeda headers */
#include "defines_priv.h"
#include "prototype_priv.h"
#include "i_vars_priv.h"

/* Gettext translation */
#include "gettext_priv.h"
