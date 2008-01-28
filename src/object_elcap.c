#include "main.h"
#include "layers.h"
#include "property.h"
#include "object.h"
#include "object_elcap.h"
#include "misc.h"

gboolean object_elcap_draw(cairo_t *cairo, LayerID layerid, Object *o);
gboolean object_elcap_select(Object *o, gdouble x, gdouble y,
	gint32 hx, gint32 hy);

ObjectType object_elcap = {
	"elcap",
	NULL,
	NULL,
	object_elcap_draw,
	object_elcap_select,
	property_default_properties_handler
};

Object *object_elcap_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2,
	guint32 color, gdouble diameter, gdouble dpin)
{
	Object *o;
	ObjectElCap *elcap;
	PropertyPrivate *priv;

	elcap = g_new0(ObjectElCap, 1);
	elcap->color = color;
	elcap->diameter = diameter;
	elcap->dpin = dpin;

	o = object_create(&object_elcap, elcap, x1, y1, x2, y2);
	priv = property_new_double(elcap->diameter, 3, 20, 0.5);
	property_add(o, "diameter", priv, &(elcap->diameter));
	priv = property_new_double(elcap->dpin, 2.54, 100, 2.54);
	property_add(o, "pin distance", priv, &(elcap->dpin));
	priv = property_new_color(elcap->color);
	property_add(o, "color", priv, &(elcap->color));

	return o;
}

gboolean object_elcap_draw(cairo_t *cairo, LayerID layerid, Object *o)
{
    ObjectElCap *elcap = (ObjectElCap *)o->data;
	gdouble cx, cy, angle, delta, bw;

	cx = MIN(o->x1, o->x2) + ABS((gdouble)(o->x2 - o->x1) / 2.0);
	cy = MIN(o->y1, o->y2) + ABS((gdouble)(o->y2 - o->y1) / 2.0);
	angle = misc_angle(o->x1, o->y1, o->x2, o->y2);
	delta = misc_delta(o->x1, o->y1, o->x2, o->y2);

	cairo_translate(cairo, HOLE_TO_POINT(cx), HOLE_TO_POINT(cy));
	cairo_rotate(cairo, angle);

	switch(layerid)
	{
		case LAYER_WIRES:
			cairo_set_source_rgba(cairo, 0, 0, 0, 0.8);
			cairo_set_line_width(cairo, 4.0);
			cairo_move_to(cairo, MM2PT(elcap->dpin) / 2.0, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(delta / 2.0), 0);
			cairo_move_to(cairo, MM2PT(-elcap->dpin) / 2.0, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(-delta / 2.0), 0);
			cairo_stroke(cairo);
			break;
		case LAYER_PARTS:
			cairo_set_source_rgba(cairo, 0, 0, 0, 1.0);
			cairo_set_line_width(cairo, 1.0);
			cairo_arc(cairo, 0, 0, MM2PT(elcap->diameter) / 2.0,
				0, G_PI * 2.0);
			cairo_stroke(cairo);
			bw = MM2PT(elcap->diameter) / 4;
			cairo_set_source_rgba(cairo,
				((elcap->color & 0xFF000000) >> 24) / 255.0,
				((elcap->color & 0x00FF0000) >> 16) / 255.0,
				((elcap->color & 0x0000FF00) >>  8) / 255.0, 1.0);
			cairo_set_line_width(cairo, bw / 2);
			cairo_arc(cairo, 0, 0, MM2PT(elcap->diameter) / 2.0 - bw / 4,
				0.3, -0.3);
			cairo_stroke(cairo);
			cairo_set_source_rgba(cairo, 1.0, 1.0, 1.0, 1.0);
			cairo_arc(cairo, 0, 0, MM2PT(elcap->diameter) / 2.0 - bw / 4,
				-0.3, 0.3);
			cairo_stroke(cairo);
			cairo_set_source_rgba(cairo, 0.6, 0.6, 0.6, 1.0);
			cairo_arc(cairo, 0, 0, MM2PT(elcap->diameter) / 2.0 - bw / 2,
				0, G_PI * 2);
			cairo_fill(cairo);
			cairo_set_source_rgba(cairo, 0, 0, 0, 0.8);
			cairo_set_line_width(cairo, 2.0);
			cairo_rotate(cairo, DEG2RAD(14));
			cairo_move_to(cairo, bw / 4, 0);
			cairo_line_to(cairo, bw * 1.25, 0);
			cairo_stroke(cairo);
			cairo_rotate(cairo, DEG2RAD(120));
			cairo_move_to(cairo, bw / 4, 0);
			cairo_line_to(cairo, bw * 1.25, 0);
			cairo_stroke(cairo);
			cairo_rotate(cairo, DEG2RAD(120));
			cairo_move_to(cairo, bw / 4, 0);
			cairo_line_to(cairo, bw * 1.25, 0);
			cairo_stroke(cairo);
			break;
		case LAYER_SELECT:
			cairo_set_source_rgba(cairo, 1.0, 0.2, 0.2, 1.0);
			cairo_set_line_width(cairo, 4.0);
			cairo_arc(cairo, 0, 0, MM2PT(elcap->diameter) / 2.0, 0, G_PI * 2);
			cairo_stroke(cairo);
			break;
		case LAYER_PREVIEW:
			cairo_arc(cairo, 0, 0, MM2PT(elcap->diameter) / 2.0, 0, G_PI * 2);
			cairo_move_to(cairo, MM2PT(elcap->dpin) / 2.0, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(delta / 2.0), 0);
			cairo_move_to(cairo, MM2PT(-elcap->dpin) / 2.0, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(-delta / 2.0), 0);
			cairo_set_line_width(cairo, 4.0);
			cairo_set_source_rgba(cairo, 0.7, 0.7, 0.7, 0.7);
			cairo_stroke(cairo);
			break;
		default:
			break;
	}
	return TRUE;
}

gboolean object_elcap_select(Object *o, gdouble x, gdouble y,
	gint32 hx, gint32 hy)
{
	ObjectElCap *elcap = (ObjectElCap *)o->data;
	gdouble cx, cy, d;

	if(object_select_line(o, x, y, hx, hy))
		return TRUE;

	cx = HOLE_TO_POINT(MIN(o->x1, o->x2)) +
		HOLES_TO_POINT(ABS(o->x2 - o->x1) / 2.0);
	cy = HOLE_TO_POINT(MIN(o->y1, o->y2)) +
		HOLES_TO_POINT(ABS(o->y2 - o->y1) / 2.0);
	d = misc_delta(cx, cy, x, y);

	if(d <= (MM2PT(elcap->diameter) / 2.0))
		return TRUE;

	return FALSE;
}

