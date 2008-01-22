#include "main.h"
#include "layers.h"
#include "gui.h"
#include "object.h"
#include "object_icsocket.h"

gboolean object_icsocket_isvalid(gint32 x1, gint32 y1, gint32 x2, gint32 y2);
gboolean object_icsocket_draw(cairo_t *cairo, LayerID layerid, gpointer
	object);
gboolean object_icsocket_select(gpointer data, gdouble x, gdouble y,
	gint32 hx, gint32 hy);

ObjectType object_icsocket = {
	"IC socket",
	NULL,
	NULL,
	object_icsocket_draw,
	object_icsocket_select,
};

Object *object_icsocket_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2)
{
	ObjectICSocket *ic;

	if(object_icsocket_isvalid(x1, y1, x2, y2))
	{
		ic = g_new0(ObjectICSocket, 1);
		ic->x1 = x1;
		ic->x2 = x2;
		ic->y1 = y1;
		ic->y2 = y2;

		return object_create(&object_icsocket, ic);
	}
	else
		return NULL;
}

gboolean object_icsocket_isvalid(gint32 x1, gint32 y1, gint32 x2, gint32 y2)
{
	return (ABS(x1 - x2) > 1) && (ABS(y1 - y2) > 1);
}

static void draw_frame(cairo_t *cairo, ObjectICSocket *ic)
{
	gdouble ox, oy;

	ox = (ic->x1 - ic->x2) / ABS(ic->x1 - ic->x2) * 36.0;
	oy = (ic->y1 - ic->y2) / ABS(ic->y1 - ic->y2) * 36.0;

	if((ox * oy) < 0)
	{
		cairo_move_to(cairo, HOLE_TO_POINT(ic->x1) + ox,
			HOLE_TO_POINT(ic->y1) + oy);
		cairo_line_to(cairo, HOLE_TO_POINT(ic->x2) - ox,
			HOLE_TO_POINT(ic->y1) + oy);
		cairo_line_to(cairo, HOLE_TO_POINT(ic->x2) - ox,
			HOLE_TO_POINT(ic->y2) - oy);
		cairo_line_to(cairo, HOLE_TO_POINT(ic->x1) + ox,
			HOLE_TO_POINT(ic->y2) - oy);
		cairo_arc(cairo, HOLE_TO_POINT(ic->x1)+ox,
			HOLE_TO_POINT(ic->y1 + (ic->y2 - ic->y1) / 2.0), 36.0,
			(oy < 0) ? G_PI / 2: -G_PI / 2, (oy < 0) ? G_PI * 1.5: G_PI / 2);
	}
	else
	{
		cairo_move_to(cairo, HOLE_TO_POINT(ic->x1) + ox,
			HOLE_TO_POINT(ic->y1) + oy);
		cairo_line_to(cairo, HOLE_TO_POINT(ic->x1) + ox,
			HOLE_TO_POINT(ic->y2) - oy);
		cairo_line_to(cairo, HOLE_TO_POINT(ic->x2) - ox,
			HOLE_TO_POINT(ic->y2) - oy);
		cairo_line_to(cairo, HOLE_TO_POINT(ic->x2) - ox,
			HOLE_TO_POINT(ic->y1) + oy);
		cairo_arc(cairo, HOLE_TO_POINT(ic->x1 + (ic->x2 - ic->x1) / 2.0),
			HOLE_TO_POINT(ic->y1) + oy, 36.0,
			(ox > 0) ? G_PI : 0, (ox > 0) ? G_PI * 2: G_PI);
	}
	cairo_close_path(cairo);
}

gboolean object_icsocket_draw(cairo_t *cairo, LayerID layerid, gpointer
	object)
{
	ObjectICSocket *ic = (ObjectICSocket *)object;
	gint32 i;

	switch(layerid)
	{
		case LAYER_PARTS:
			draw_frame(cairo, ic);
			cairo_new_sub_path(cairo);
			cairo_rectangle(cairo,
				AREA_TO_POINT(MIN(ic->x1, ic->x2) + 1),
				AREA_TO_POINT(MIN(ic->y1, ic->y2) + 1),
				HOLES_TO_POINT(ABS(ic->x1 - ic->x2) - 1),
				HOLES_TO_POINT(ABS(ic->y1 - ic->y2) - 1));
			cairo_set_fill_rule(cairo, CAIRO_FILL_RULE_EVEN_ODD);
			cairo_set_source_rgba(cairo, 0.2, 0.2, 0.2, 0.8);
			cairo_fill_preserve(cairo);
			cairo_set_source_rgba(cairo, 0.0, 0.0, 0.0, 1.0);
			cairo_set_line_width(cairo, 4.0);
			cairo_stroke(cairo);
			if(((ic->x1 - ic->x2) * (ic->y1 - ic->y2)) < 0)
			{
				/* horizontal */
				for(i = MIN(ic->x1, ic->x2); i <= MAX(ic->x1, ic->x2); i ++)
				{
					cairo_new_sub_path(cairo);
					cairo_arc(cairo, HOLE_TO_POINT(i), HOLE_TO_POINT(ic->y1),
						16, 0, G_PI * 2);
					cairo_new_sub_path(cairo);
					cairo_arc(cairo, HOLE_TO_POINT(i), HOLE_TO_POINT(ic->y2),
						16, 0, G_PI * 2);
				}
			}
			else
			{
				/* vertical */
				for(i = MIN(ic->y1, ic->y2); i <= MAX(ic->y1, ic->y2); i ++)
				{
					cairo_new_sub_path(cairo);
					cairo_arc(cairo, HOLE_TO_POINT(ic->x1), HOLE_TO_POINT(i),
						16, 0, G_PI * 2);
					cairo_new_sub_path(cairo);
					cairo_arc(cairo, HOLE_TO_POINT(ic->x2), HOLE_TO_POINT(i),
						16, 0, G_PI * 2);
				}
			}
			cairo_set_line_width(cairo, 10.0);
			cairo_set_source_rgba(cairo, 0.7, 0.7, 0.7, 1.0);
			cairo_stroke(cairo);
			break;
		case LAYER_SELECT:
			draw_frame(cairo, ic);
			cairo_set_source_rgba(cairo, 1.0, 0.0, 0.0, 1.0);
			cairo_set_line_width(cairo, 4.0);
			cairo_stroke(cairo);
			break;
		case LAYER_PREVIEW:
			draw_frame(cairo, ic);
			cairo_set_source_rgba(cairo, 0.4, 0.4, 0.4, 0.8);
			cairo_set_line_width(cairo, 4.0);
			cairo_stroke(cairo);
			break;
		default:
			break;
	}

	return TRUE;
}

gboolean object_icsocket_select(gpointer data, gdouble x, gdouble y,
	gint32 hx, gint32 hy)
{
	ObjectICSocket *ic = (ObjectICSocket *)data;

	return  (hx >= MIN(ic->x1, ic->x2)) &&
			(hy >= MIN(ic->y1, ic->y2)) &&
			(hx <= MAX(ic->x1, ic->x2)) &&
			(hy <= MAX(ic->y1, ic->y2));
}
