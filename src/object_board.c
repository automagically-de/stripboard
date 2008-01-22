#include "main.h"
#include "layers.h"
#include "object.h"
#include "object_board.h"
#include "property.h"

gboolean object_board_draw(cairo_t *cairo, LayerID layerid, gpointer object);
GtkWidget *object_board_properties(gpointer data);

ObjectType object_board = {
	"board",
	NULL,
	NULL,
	object_board_draw,
	NULL,
	object_board_properties
};

Object *object_board_new(guint32 holes_x, guint32 holes_y)
{
	ObjectBoard *oboard;

	oboard = g_new0(ObjectBoard, 1);
	oboard->holes_x = holes_x;
	oboard->holes_y = holes_y;

	return object_create(&object_board, oboard);
}

gboolean object_board_draw(cairo_t *cairo, LayerID layerid, gpointer object)
{
	ObjectBoard *oboard = (ObjectBoard *)object;
	gint x, y;

	switch(layerid)
	{
		case LAYER_STRIPS:
			for(y = 0; y < oboard->holes_y; y ++)
			{
				cairo_set_source_rgba(cairo, 1.0, 0.6, 0.1, 1.0);
				cairo_rectangle(cairo,
					AREA_TO_POINT(0) + 10.0,
					AREA_TO_POINT(y) + 10.0,
					HOLES_TO_POINT(oboard->holes_x) - 20.0,
					HOLES_TO_POINT(1)- 20.0);
				cairo_fill(cairo);
			}
			break;

		case LAYER_BOARD:
			cairo_set_source_rgba(cairo, 0.8, 0.9, 0.1, 0.6);
			cairo_set_line_width(cairo, 3.0);
			cairo_rectangle(cairo,
				AREA_TO_POINT(0),
				AREA_TO_POINT(0),
				HOLES_TO_POINT(oboard->holes_x),
				HOLES_TO_POINT(oboard->holes_y));
			cairo_fill(cairo);

			cairo_set_source_rgba(cairo, 0.9, 0.9, 0.9, 1.0);
			for(y = 0; y < oboard->holes_y; y ++)
				for(x = 0; x < oboard->holes_x; x ++)
				{
					cairo_arc(cairo, HOLE_TO_POINT(x), HOLE_TO_POINT(y),
						15.0, 0, G_PI * 2);
					cairo_fill(cairo);
					cairo_stroke(cairo);
				}
			break;

		default:
			break;
	}
	return TRUE;
}

struct BoardUpdateContainer {
	guint32 *var;
	guint32 *global_var;
};

static void board_spin_changed_cb(GtkSpinButton *spinbutton, gpointer data)
{
	struct BoardUpdateContainer *v = (struct BoardUpdateContainer *)data;

	*(v->var) = gtk_spin_button_get_value_as_int(spinbutton);
	*(v->global_var) = *(v->var);
	object_redraw(NULL);
}

GtkWidget *object_board_properties(gpointer data)
{
	static PropertyPrivate *p_w, *p_h;
	static GtkWidget *table = NULL, *w;
	ObjectBoard *board = (ObjectBoard *)data;
	static struct BoardUpdateContainer *uc1, *uc2;

	if(table == NULL)
	{
		table = gtk_table_new(2, 2, FALSE);

		uc1 = g_new0(struct BoardUpdateContainer, 1);
		uc2 = g_new0(struct BoardUpdateContainer, 1);

		w = gtk_label_new("width:");
		gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 0, 1);
		w = gtk_spin_button_new_with_range(2, 100, 1);
		p_w = property_new_custom(w, "value-changed",
			G_CALLBACK(board_spin_changed_cb));
		gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 0, 1);

		w = gtk_label_new("height:");
		gtk_table_attach_defaults(GTK_TABLE(table), w, 0, 1, 1, 2);
		w = gtk_spin_button_new_with_range(2, 100, 1);
		p_h = property_new_custom(w, "value-changed",
			G_CALLBACK(board_spin_changed_cb));
		gtk_table_attach_defaults(GTK_TABLE(table), w, 1, 2, 1, 2);

		gtk_widget_show_all(table);
		g_object_ref(G_OBJECT(table));
	}

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(p_w->widget), board->holes_x);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(p_h->widget), board->holes_y);
	uc1->var = &(board->holes_x);
	uc1->global_var = &g_holes_x;
	property_update_handler(p_w, uc1);
	uc2->var = &(board->holes_y);
	uc2->global_var = &g_holes_y;
	property_update_handler(p_h, uc2);

	return table;
}
