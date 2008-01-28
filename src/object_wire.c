#include "main.h"
#include "layers.h"
#include "object.h"
#include "object_wire.h"
#include "misc.h"
#include "property.h"

gboolean object_wire_draw(cairo_t *cairo, LayerID layerid, Object *o);

ObjectType object_wire = {
	"wire",
	NULL,
	NULL,
	object_wire_draw,
	object_select_line,
	property_default_properties_handler
};

Object *object_wire_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2,
	guint32 color)
{
	Object *o;
	ObjectWire *wire;
	PropertyPrivate *priv;

	wire = g_new0(ObjectWire, 1);
	wire->color = color;

	o = object_create(&object_wire, wire, x1, y1, x2, y2);
	priv = property_new_color(0x20FF40FF);
	property_add(o, "color", priv, &(wire->color));

	return o;
}

gboolean object_wire_draw(cairo_t *cairo, LayerID layerid, Object *o)
{
    ObjectWire *wire = (ObjectWire *)o->data;
	gdouble cx, cy, angle, delta;

	cx = MIN(o->x1, o->x2) + ABS((gdouble)(o->x2 - o->x1) / 2.0);
	cy = MIN(o->y1, o->y2) + ABS((gdouble)(o->y2 - o->y1) / 2.0);
	angle = misc_angle(o->x1, o->y1, o->x2, o->y2);
	delta = misc_delta(o->x1, o->y1, o->x2, o->y2);

	cairo_translate(cairo, HOLE_TO_POINT(cx), HOLE_TO_POINT(cy));
	cairo_rotate(cairo, angle);

	switch(layerid)
	{
		case LAYER_WIRES:
			cairo_rectangle(cairo, HOLES_TO_POINT(-delta / 2.0) + 10.0, -3.5,
				HOLES_TO_POINT(delta) - 20.0, 7.0);
			cairo_set_source_rgba(cairo,
				((wire->color & 0xFF000000) >> 24) / 255.0,
				((wire->color & 0x00FF0000) >> 16) / 255.0,
				((wire->color & 0x0000FF00) >>  8) / 255.0, 1.0);
			cairo_fill_preserve(cairo);
			cairo_set_line_width(cairo, 1.0);
			cairo_set_source_rgba(cairo, 0, 0, 0, 0.8);
			cairo_stroke(cairo);
			cairo_set_line_width(cairo, 4.0);
			cairo_move_to(cairo, HOLES_TO_POINT(-delta / 2.0) + 10.0, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(-delta / 2.0), 0);
			cairo_move_to(cairo, HOLES_TO_POINT(delta / 2.0) - 10.0, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(delta / 2.0), 0);
			cairo_stroke(cairo);
			break;
		case LAYER_SELECT:
			cairo_set_line_width(cairo, 4.0);
			cairo_set_source_rgba(cairo, 1.0, 0.0, 0.0, 1.0);
			cairo_rectangle(cairo,
				HOLES_TO_POINT(-delta / 2.0) + 10.0, -3.5,
				HOLES_TO_POINT(delta) - 20.0, 7.0);
			cairo_stroke(cairo);
			break;
		case LAYER_PREVIEW:
			cairo_move_to(cairo, HOLES_TO_POINT(-delta / 2.0), 0);
			cairo_line_to(cairo, HOLES_TO_POINT(delta / 2.0), 0);
			cairo_set_line_width(cairo, 4.0);
			cairo_set_source_rgba(cairo, 0.7, 0.7, 0.7, 0.7);
			cairo_stroke(cairo);
			break;
		default:
			break;
	}
	return TRUE;
}

