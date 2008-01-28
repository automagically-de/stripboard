#ifndef _OBJECT_H
#define _OBJECT_H

#include "layers.h"

#include <glib.h>
#include <gtk/gtk.h>
#include <cairo/cairo.h>

typedef struct _ObjectType ObjectType;
typedef struct _Object Object;

typedef gpointer (*ObjectTypeInitFunc)(void);
typedef void (*ObjectTypeCleanupFunc)(gpointer data);
typedef gboolean (*ObjectRenderFunc)(cairo_t *cairo, LayerID layerid,
	Object *o);
typedef gboolean (*ObjectSelectFunc)(Object *o,
	gdouble x, gdouble y, gint32 hx, gint32 hy);
typedef GtkWidget *(*ObjectPropertiesFunc)(Object *o);

struct _ObjectType {
	const gchar *title;
	ObjectTypeInitFunc init;
	ObjectTypeCleanupFunc cleanup;
	ObjectRenderFunc render;
	ObjectSelectFunc select;
	ObjectPropertiesFunc properties;
};

struct _Object {
	ObjectType *type;
	gint32 x1;
	gint32 y1;
	gint32 x2;
	gint32 y2;
	GSList *properties;
	GtkWidget *properties_widget;
	gpointer data;
};

gboolean object_init(void);
void object_cleanup(void);

gboolean object_save_to_file(const gchar *filename);
gboolean object_load_from_file(const gchar *filename);

Object *object_create(ObjectType *type, gpointer data,
	gint32 x1, gint32 y1, gint32 x2, gint32 y2);
gboolean object_add(Object *object);
gboolean object_remove_full(Object *object, gboolean delete);
gboolean object_remove(Object *object);
gboolean object_remove_all(void);
void object_delete(Object *object);
gboolean object_swap_polarity(Object *object);
gboolean object_redraw(Object *object);
gboolean object_render(cairo_t *cairo, LayerID layerid, Object *object);
Object *object_select(gdouble x, gdouble y, gint32 hx, gint32 hy);
Object *object_get_selected(void);
Object *object_get_board(void);

/* generic object callbacks */
gboolean object_select_line(Object *o, gdouble x, gdouble y,
	gint32 hx, gint32 hy);

#include "object_board.h"
#include "object_wire.h"
#include "object_resistor.h"
#include "object_elcap.h"
#include "object_led.h"
#include "object_icsocket.h"

#endif /* _OBJECT_H */
