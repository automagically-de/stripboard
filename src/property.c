#include "property.h"
#include "object.h"

static gboolean double_update_handler(PropertyPrivate *priv, gdouble *var);
static gboolean color_update_handler(PropertyPrivate *priv, guint32 *var);
static gboolean custom_update_handler(PropertyPrivate *priv, gpointer var);

/*****************************************************************************/
/* generic                                                                   */
/*****************************************************************************/

GtkWidget *property_get_widget(PropertyPrivate *priv)
{
	return priv->widget;
}

gboolean property_update_handler(PropertyPrivate *priv, gpointer var)
{
	if(priv->signal_handler >= 0)
		g_signal_handler_disconnect(G_OBJECT(priv->widget),
			(gulong)priv->signal_handler);
	switch(priv->type)
	{
		case PROPERTY_DOUBLE:
			return double_update_handler(priv, (gdouble *)var);
		case PROPERTY_COLOR:
			return color_update_handler(priv, (guint32 *)var);

		case PROPERTY_CUSTOM:
			return custom_update_handler(priv, var);
		default:
			return FALSE;
	}
}

/*****************************************************************************/
/* custom                                                                    */
/*****************************************************************************/

typedef struct {
	const gchar *signal;
	GCallback callback;
} PropertyPrivateCustom;

PropertyPrivate *property_new_custom(GtkWidget *widget, const gchar *signal,
	GCallback callback)
{
	PropertyPrivate *priv;
	PropertyPrivateCustom *cpriv;

	priv = g_new0(PropertyPrivate, 1);
	priv->type = PROPERTY_CUSTOM;
	priv->signal_handler = -1;
	priv->widget = priv->signal_widget = widget;
	cpriv = g_new0(PropertyPrivateCustom, 1);
	cpriv->signal = signal;
	cpriv->callback = callback;
	priv->custom = cpriv;

	return priv;
}

static gboolean custom_update_handler(PropertyPrivate *priv, gpointer var)
{
	PropertyPrivateCustom *cpriv = (PropertyPrivateCustom *)priv->custom;

	priv->signal_handler = g_signal_connect(G_OBJECT(priv->signal_widget),
		cpriv->signal, cpriv->callback, var);

	return TRUE;
}

/*****************************************************************************/
/* double                                                                    */
/*****************************************************************************/

typedef struct {
	gdouble defval;
	gdouble min;
	gdouble max;
} PropertyPrivateDouble;

PropertyPrivate *property_new_double(gdouble defval, gdouble min, gdouble max,
	gdouble step)
{
	PropertyPrivate *priv;
	PropertyPrivateDouble *dpriv;

	priv = g_new0(PropertyPrivate, 1);
	priv->type = PROPERTY_DOUBLE;
	priv->signal_handler = -1;
	dpriv = g_new0(PropertyPrivateDouble, 1);
	dpriv->defval = defval;
	dpriv->min = min;
	dpriv->max = max;
	priv->custom = dpriv;
	priv->widget = gtk_spin_button_new_with_range(min, max, step);
	priv->signal_widget = priv->widget;

	return priv;
}

static void double_spin_changed_cb(GtkSpinButton *spinbutton, gpointer data)
{
	*((gdouble *)data) = gtk_spin_button_get_value(spinbutton);
	object_redraw(NULL);
}

static gboolean double_update_handler(PropertyPrivate *priv, gdouble *var)
{
	PropertyPrivateDouble *dpriv = (PropertyPrivateDouble *)priv->custom;

	priv->signal_handler = g_signal_connect(G_OBJECT(priv->signal_widget),
		"value-changed", G_CALLBACK(double_spin_changed_cb),
		((var != NULL) ? var : &(dpriv->defval)));

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(priv->signal_widget),
		((var != NULL) ? *var : dpriv->defval));

	return TRUE;
}

/*****************************************************************************/
/* color                                                                     */
/*****************************************************************************/

typedef struct {
	guint32 defval;
	GdkColor *color;
} PropertyPrivateColor;

static void color_uint32_to_gdk(GdkColor *gdkcolor, guint32 color)
{
	gdkcolor->red   = ((color & 0xFF000000) >> 24) << 8;
	gdkcolor->green = ((color & 0x00FF0000) >> 16) << 8;
	gdkcolor->blue  = ((color & 0x0000FF00) >> 8)  << 8;
}

static guint32 color_gdk_to_uint32(GdkColor *gdkcolor)
{
	return 0xFF +
		((gdkcolor->red   >> 8) << 24) +
		((gdkcolor->green >> 8) << 16) +
		((gdkcolor->blue  >> 8) << 8);
}

PropertyPrivate *property_new_color(guint32 defval)
{
	PropertyPrivate *priv;
	PropertyPrivateColor *cpriv;

	priv = g_new0(PropertyPrivate, 1);
	priv->type = PROPERTY_COLOR;
	priv->signal_handler = -1;
	cpriv = g_new0(PropertyPrivateColor, 1);
	cpriv->defval = defval;
	cpriv->color = g_new0(GdkColor, 1);
	color_uint32_to_gdk(cpriv->color, cpriv->defval);
	priv->custom = cpriv;
	priv->widget = gtk_color_button_new_with_color(cpriv->color);
	priv->signal_widget = priv->widget;

	return priv;
}

static void color_set_cb(GtkColorButton *widget, gpointer data)
{
	guint32 *var = (guint32 *)data;
	GdkColor color;

	gtk_color_button_get_color(widget, &color);
	*var = color_gdk_to_uint32(&color);

	object_redraw(NULL);
}

static gboolean color_update_handler(PropertyPrivate *priv, guint32 *var)
{
	PropertyPrivateColor *cpriv = (PropertyPrivateColor *)priv->custom;

	priv->signal_handler = g_signal_connect(G_OBJECT(priv->signal_widget),
		"color-set", G_CALLBACK(color_set_cb),
		((var != NULL) ? var : &(cpriv->defval)));

	color_uint32_to_gdk(cpriv->color, ((var != NULL) ? *var : cpriv->defval));
	gtk_color_button_set_color(GTK_COLOR_BUTTON(priv->widget), cpriv->color);

	return TRUE;
}
