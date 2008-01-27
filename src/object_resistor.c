#include "main.h"
#include "layers.h"
#include "object.h"
#include "object_resistor.h"
#include "property.h"
#include "misc.h"

gboolean object_resistor_draw(cairo_t *cairo, LayerID layerid, Object *o);
GtkWidget *object_resistor_properties(Object *o);

static gdouble g_res_r = 51.0;

ObjectType object_resistor = {
	"resistor",
	NULL,
	NULL,
	object_resistor_draw,
	object_select_line,
	object_resistor_properties
};

static guint32 ring_colors[] = {
	0x000000FF, /* 0: black */
	0x7F3F00FF, /* 1: brown */
	0xFF0000FF, /* 2: red */
	0xBF3F00FF, /* 3: orange */
	0xFFFF00FF, /* 4: yellow */
	0x00FF00FF, /* 5: green */
	0x0000FFFF, /* 6: blue */
	0x7F007FFF, /* 7: purple */
	0x7F7F7FFF, /* 8: gray */
	0xFFFFFFFF, /* 9: white */
};

static guint32 multiplier_colors[] = {
	0x3F3F3FFF, /* -2, 0.01: silver */
	0xEFB218FF, /* -1, 0.1 : gold */
	0x000000FF, /*  0, 1   : black */
	0x7F3F00FF, /*  1, 10  : brown */
	0xFF0000FF, /*  2, 100 : red */
	0xBF3F00FF, /*  3, 1000: orange */
	0xFFFF00FF, /*  4, 10k : yellow */
	0x00FF00FF, /*  5, 100k: green */
	0x0000FFFF, /*  6, 1M  : blue */
	0x7F007FFF, /*  7, 10M : purple */
	0x7F7F7FFF, /*  8, 100M: gray */
	0xFFFFFFFF, /*  9, 1G  : white */
};

Object *object_resistor_new(guint32 x1, guint32 y1, guint32 x2,
	guint32 y2, gdouble r)
{
	ObjectResistor *resistor;

	resistor = g_new0(ObjectResistor, 1);
	if(r < 0)
		resistor->r = g_res_r;
	else
		resistor->r = r;

	return object_create(&object_resistor, resistor, x1, y1, x2, y2);
}

static gboolean get_rings_3(gdouble r, guint32 *r1, guint32 *r2, guint32 *rm)
{
	gint32 x = 2;
	gdouble m = 100;
	/* multiplier */
	while((r * m >= 100))
	{
		x --;
		m /= 10.0;
	}
	*rm = -x + 2;
	*r1 = (guint32)(r * m) / 10;
	*r2 = (guint32)(r * m) % 10;

	return TRUE;
}

gboolean object_resistor_draw(cairo_t *cairo, LayerID layerid, Object *o)
{
    ObjectResistor *resistor = (ObjectResistor *)o->data;
	gdouble cx, cy, angle, delta;
	guint32 r1, r2, rm;

	cx = MIN(o->x1, o->x2) + ABS((gdouble)(o->x2 - o->x1) / 2.0);
	cy = MIN(o->y1, o->y2) + ABS((gdouble)(o->y2 - o->y1) / 2.0);
	angle = misc_angle(o->x1, o->y1, o->x2, o->y2);
	delta = misc_delta(o->x1, o->y1, o->x2, o->y2);

	cairo_translate(cairo, HOLE_TO_POINT(cx), HOLE_TO_POINT(cy));
	cairo_rotate(cairo, angle);

#define RES_R_CAPS 0.75
#define RES_LENGTH 6.40

	switch(layerid)
	{
		case LAYER_WIRES:
			cairo_set_source_rgba(cairo, 0, 0, 0, 0.8);
			cairo_set_line_width(cairo, 4.0);
			cairo_move_to(cairo, MM2PT(RES_LENGTH / 2), 0);
			cairo_line_to(cairo, HOLES_TO_POINT(delta / 2.0), 0);
			cairo_move_to(cairo, -MM2PT(RES_LENGTH / 2), 0);
			cairo_line_to(cairo, HOLES_TO_POINT(-delta / 2.0), 0);
			cairo_stroke(cairo);
			break;
		case LAYER_PARTS:
			cairo_arc(cairo,
				MM2PT(RES_LENGTH / 2 - RES_R_CAPS), 0,
				MM2PT(RES_R_CAPS),
				G_PI + 0.7, G_PI - 0.7);
			cairo_arc(cairo,
				-MM2PT(RES_LENGTH / 2 - RES_R_CAPS), 0,
				MM2PT(RES_R_CAPS),
				0.7, -0.7);
			cairo_close_path(cairo);
			cairo_set_source_rgba(cairo, 1.0, 0.9, 0.7, 1.0);
			cairo_fill_preserve(cairo);
			cairo_set_line_width(cairo, 2.0);
			cairo_set_source_rgba(cairo, 0, 0, 0, 1.0);
			cairo_stroke(cairo);
			/* rings */
			get_rings_3(resistor->r, &r1, &r2, &rm);
#define SET_RING_COLOR_CUSTOM(i, array) \
	cairo_set_source_rgba(cairo, \
		((array[i] & 0xFF000000) >> 24) / 255.0, \
		((array[i] & 0x00FF0000) >> 16) / 255.0, \
		((array[i] & 0x0000FF00) >> 8) / 255.0, \
		((array[i] & 0x000000FF)) / 255.0)
#define SET_RING_COLOR(i) SET_RING_COLOR_CUSTOM(i, ring_colors)
#define RING_OFF_Y 14.0

			cairo_set_line_width(cairo, 10.0);
			cairo_move_to(cairo, -21.0, RING_OFF_Y);
			cairo_line_to(cairo, -21.0, -RING_OFF_Y);
			SET_RING_COLOR(r1);
			cairo_stroke(cairo);
			cairo_move_to(cairo, -7.0, RING_OFF_Y);
			cairo_line_to(cairo, -7.0, -RING_OFF_Y);
			SET_RING_COLOR(r2);
			cairo_stroke(cairo);
			cairo_move_to(cairo, 7.0, RING_OFF_Y);
			cairo_line_to(cairo, 7.0, -RING_OFF_Y);
			SET_RING_COLOR_CUSTOM(rm, multiplier_colors);
			cairo_stroke(cairo);
			cairo_move_to(cairo, 35.0, RING_OFF_Y);
			cairo_line_to(cairo, 35.0, -RING_OFF_Y);
			SET_RING_COLOR_CUSTOM(1, multiplier_colors);
			cairo_stroke(cairo);
			break;
		case LAYER_SELECT:
			cairo_arc(cairo,
				MM2PT(RES_LENGTH / 2 - RES_R_CAPS), 0,
				MM2PT(RES_R_CAPS),
				G_PI + 0.7, G_PI - 0.7);
			cairo_arc(cairo,
				-MM2PT(RES_LENGTH / 2 - RES_R_CAPS), 0,
				MM2PT(RES_R_CAPS),
				0.7, -0.7);
			cairo_close_path(cairo);
			cairo_set_source_rgba(cairo, 1.0, 0.2, 0.2, 1.0);
			cairo_set_line_width(cairo, 4.0);
			cairo_stroke(cairo);
			break;
		case LAYER_PREVIEW:
			cairo_arc(cairo,
				MM2PT(RES_LENGTH / 2 - RES_R_CAPS), 0,
				MM2PT(RES_R_CAPS),
				G_PI + 0.7, G_PI - 0.7);
			cairo_arc(cairo,
				-MM2PT(RES_LENGTH / 2 - RES_R_CAPS), 0,
				MM2PT(RES_R_CAPS),
				0.7, -0.7);
			cairo_close_path(cairo);
			cairo_move_to(cairo, MM2PT(RES_LENGTH / 2), 0);
			cairo_line_to(cairo, HOLES_TO_POINT(delta / 2.0), 0);
			cairo_move_to(cairo, -MM2PT(RES_LENGTH / 2), 0);
			cairo_line_to(cairo, HOLES_TO_POINT(-delta / 2.0), 0);
			cairo_set_source_rgba(cairo, 0.7, 0.7, 0.7, 0.7);
			cairo_set_line_width(cairo, 4.0);
			cairo_stroke(cairo);
			break;
		default:
			break;
	}
	return TRUE;
}

GtkWidget *object_resistor_properties(Object *o)
{
	static PropertyPrivate *p_r;
	static GtkWidget *table = NULL, *w;
	ObjectResistor *res = (ObjectResistor *)o->data;

	if(table == NULL)
	{
		/* initialize widget */
		table = gtk_table_new(1, 2, FALSE);
		w = gtk_label_new("R:");
		gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 0, 1);

		p_r = property_new_double(100, 0, 99.*1000.*1000.*1000., 1.0);
		w = property_get_widget(p_r);
		gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 0, 1);

		gtk_widget_show_all(table);
		g_object_ref(G_OBJECT(table));
	}

	property_update_handler(p_r, &(res->r));

	return table;
}
