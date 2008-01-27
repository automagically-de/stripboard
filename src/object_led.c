#include "main.h"
#include "layers.h"
#include "object.h"
#include "object_led.h"
#include "misc.h"

gboolean object_led_draw(cairo_t *cairo, LayerID layerid, Object *o);

ObjectType object_led = {
	"LED",
	NULL,
	NULL,
	object_led_draw,
	object_select_line,
	NULL
};

Object *object_led_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2,
	guint32 color)
{
	ObjectLED *led;

	led = g_new0(ObjectLED, 1);
	led->color = color;

	return object_create(&object_led, led, x1, y1, x2, y2);
}

gboolean object_led_draw(cairo_t *cairo, LayerID layerid, Object *o)
{
	cairo_pattern_t *pat;
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
			cairo_set_source_rgba(cairo, 0, 0, 0, 0.8);
			cairo_set_line_width(cairo, 4.0);
			cairo_move_to(cairo, 35, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(delta / 2.0), 0);
			cairo_move_to(cairo, -35, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(-delta / 2.0), 0);
			cairo_stroke(cairo);
			break;
		case LAYER_PARTS:
			/* inner filling */
			pat = cairo_pattern_create_radial(-10, -10, 0, -10, -10, 50);
			cairo_pattern_add_color_stop_rgba(pat, 0, 0.2, 1.0, 0.1, 0.7);
			cairo_pattern_add_color_stop_rgba(pat, 10, 0.8, 1.0, 0.2, 0.7);
			cairo_set_source(cairo, pat);
			cairo_arc(cairo, 0, 0, 50, 0.6, -0.6);
			cairo_close_path(cairo);
			cairo_fill_preserve(cairo);
			cairo_pattern_destroy(pat);
			/* outer ring */
			cairo_set_line_width(cairo, 6.0);
			cairo_set_source_rgba(cairo, 0.2, 1.0, 0.1, 1.0);
			cairo_stroke(cairo);
			break;
		case LAYER_SELECT:
			cairo_set_line_width(cairo, 4.0);
			cairo_arc(cairo, 0, 0, 55, 0.6, -0.6);
			cairo_close_path(cairo);
			cairo_set_source_rgba(cairo, 1.0, 0.2, 0.2, 1.0);
			cairo_stroke(cairo);
			break;
		case LAYER_PREVIEW:
			cairo_arc(cairo, 0, 0, 55, 0.6, -0.6);
			cairo_close_path(cairo);
			cairo_move_to(cairo, 35, 0);
			cairo_line_to(cairo, HOLES_TO_POINT(delta / 2.0), 0);
			cairo_move_to(cairo, -35, 0);
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
