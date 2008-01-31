#include "main.h"
#include "layers.h"
#include "object.h"
#include "property.h"
#include "misc.h"
#include "object_%template%.h"

gboolean object_%template%_draw(cairo_t *cairo, LayerID layerid, Object *o);
gboolean object_%template%_select(Object *o, gdouble x, gdouble y,
	gint32 hx, gint32 hy);

ObjectType object_%template% = {
	"%template%",
	NULL, /* init */
	NULL, /* cleanup */
	object_%template%_draw, /* draw */
	object_%template%_select, /* select */
	property_default_properties_handler
};

typedef struct {
	gdouble prop;
} Object%Template%;

Object *object_%template%_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2)
{
	Object *o;
	Object%Template% *%template%;
	/* PropertyPrivate *priv; */

	%template% = g_new0(Object%Template%, 1);
	o = object_create(&object_%template%, %template%, x1, y1, x2, y2);

	/*
	priv = property_new_double(%template%->prop, 3, 20, 0.5);
	property_add(o, "prop", priv, &(%template%->prop));
	*/

	return o;
}

gboolean object_%template%_draw(cairo_t *cairo, LayerID layerid, Object *o)
{
	Object%Template% *%template% = (Object%Template% *)o->data;
	gdouble cx, cy, angle, delta;

	cx = MIN(o->x1, o->x2) + ABS((gdouble)(o->x2 - o->x1) / 2.0);
	cy = MIN(o->y1, o->y2) + ABS((gdouble)(o->y2 - o->y1) / 2.0);
	angle = misc_angle(o->x1, o->y1, o->x2, o->y2);
	delta = misc_delta(o->x1, o->y1, o->x2, o->y2);

	switch(layerid)
	{
		case LAYER_WIRES:
			break;
		case LAYER_PARTS:
			break;
		case LAYER_SELECT:
			break;
		case LAYER_PREVIEW:
			break;
		default:
			break;
	}
	return TRUE;
}

gboolean object_%template%_select(Object *o, gdouble x, gdouble y,
	gint32 hx, gint32 hy)
{
	Object%Template% *%template% = (Object%Template% *)o->data;
	gdouble cx, cy, d;

	if(object_select_line(o, x, y, hx, hy))
		return TRUE;

	cx = HOLE_TO_POINT(MIN(o->x1, o->x2)) +
		HOLES_TO_POINT(ABS(o->x2 - o->x1) / 2.0);
	cy = HOLE_TO_POINT(MIN(o->y1, o->y2)) +
		HOLES_TO_POINT(ABS(o->y2 - o->y1) / 2.0);
	d = misc_delta(cx, cy, x, y);

	return FALSE;
}
