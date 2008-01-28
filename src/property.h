#ifndef _PROPERTY_H
#define _PROPERTY_H

#include <gtk/gtk.h>

#include "object.h"

typedef enum {
	PROPERTY_UNHANDLED = 0,
	PROPERTY_DOUBLE,
	PROPERTY_COLOR,
	PROPERTY_CUSTOM = 255
} PropertyType;

typedef struct {
	PropertyType type;
	GtkWidget *widget;
	GtkWidget *signal_widget;
	gint32 signal_handler;
	gpointer custom;
} PropertyPrivate;

typedef struct {
	gpointer var;
	const gchar *title;
	PropertyPrivate *priv;
} Property;

GtkWidget *property_get_widget(PropertyPrivate *priv);
gboolean property_update_handler(PropertyPrivate *priv, gpointer var);
gboolean property_add(Object *o, const gchar *title, PropertyPrivate *priv,
		gpointer var);

GtkWidget *property_default_properties_handler(Object *o);

PropertyPrivate *property_new_double(gdouble defval, gdouble min, gdouble max,
	gdouble step);
PropertyPrivate *property_new_color(guint32 defval);
PropertyPrivate *property_new_custom(GtkWidget *widget, const gchar *signal,
	GCallback callback);

#endif /* _PROPERTY_H */
