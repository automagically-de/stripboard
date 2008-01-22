#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "main.h"
#include "gui.h"
#include "actions.h"
#include "layers.h"
#include "misc.h"
#include "tools.h"
#include "object.h"

#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

#define HOLES_TO_PX(x) (int)((x) * DPI * g_scale)
#define PX_TO_HOLE(x) ((int)((x) / DPI / g_scale) - 2)
#define HOLE_TO_PX(x) HOLES_TO_PX((x) + 2)

#define PX_TO_POINT(x) ((x) / DPI / g_scale * 72.0)

#define SW_OFF_X ((int)gtk_adjustment_get_value(\
	gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(sw))))
#define SW_OFF_Y ((int)gtk_adjustment_get_value(\
	gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(sw))))

static gdouble g_scale = SCALE;
static gint32 g_drag_ox = -1;
static gint32 g_drag_oy = -1;
static GtkWidget *draw, *sw, *propbox;
static RsvgHandle *g_selection_svg = NULL, *g_preview_svg = NULL;

RsvgHandle *svg = NULL;

RsvgHandle *load_svg(const gchar *filename);
RsvgHandle *dump_svg(guint8 *data, guint32 length);
static gboolean expose_event_cb (GtkWidget *widget, GdkEventExpose *event,
	gpointer data);
static gboolean button_press_cb(GtkWidget *widget, GdkEventButton *event,
	gpointer data);
static gboolean button_release_cb(GtkWidget *widget, GdkEventButton *event,
	gpointer data);
static gboolean scroll_cb(GtkWidget *widget, GdkEventScroll *event,
	gpointer data);
static gboolean motion_notify_cb(GtkWidget *widget, GdkEventMotion *event,
	gpointer data);
static gboolean layer_opacity_changed_cb(GtkRange *range, gpointer data);
static void radio_clicked_cb(GtkAction *action, GtkRadioAction *current,
	gpointer data);

gboolean gui_init(int *argcp, char ***argvp)
{
	GtkWidget *window, *vbox, *hpaned, *nb, *label, *nbpage, *tb, *scale;
	GtkWidget *menubar;
	GtkUIManager *ui;
	GtkActionGroup *ag;
	Object *board;
	GError *error = NULL;
	gint32 i;

	gtk_init(argcp, argvp);
	rsvg_init();

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
	gtk_window_set_title(GTK_WINDOW(window), "Stripboard");

	g_signal_connect(G_OBJECT(window), "delete-event",
		G_CALLBACK(gtk_main_quit), NULL);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/* main menu */

	ui = gtk_ui_manager_new();

	ag = gtk_action_group_new("MainActions");
	gtk_action_group_set_translation_domain(ag, "stripboard");
	gtk_action_group_add_actions(ag, action_entries, action_num_entries, NULL);
	gtk_action_group_add_radio_actions(ag,
		action_radio_entries, action_num_radio_entries,
		-1, G_CALLBACK(radio_clicked_cb), NULL);

	gtk_ui_manager_insert_action_group(ui, ag, 0);
	gtk_ui_manager_add_ui_from_file(ui,
		DATA_DIR "/ui/stripboard-ui.xml", &error);
	if(error != NULL)
	{
		g_warning("building menus failed: %s", error->message);
		g_error_free(error);
	}
	gtk_ui_manager_ensure_update(ui);
	menubar = gtk_ui_manager_get_widget(ui, "/MainMenu");
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	gtk_window_add_accel_group(GTK_WINDOW(window),
		gtk_ui_manager_get_accel_group(ui));

	/* main view */

	hpaned = gtk_hpaned_new();
	gtk_paned_set_position(GTK_PANED(hpaned), 180);
	gtk_box_pack_start(GTK_BOX(vbox), hpaned, TRUE, TRUE, 0);

	/** notebook **/

	nb = gtk_notebook_new();
	gtk_paned_add1(GTK_PANED(hpaned), nb);

	/*** tool bar ***/

	nbpage = gtk_vbox_new(FALSE, 0);
	label = gtk_label_new_with_mnemonic("_tools");
	gtk_notebook_append_page(GTK_NOTEBOOK(nb), nbpage, label);

	tb = gtk_ui_manager_get_widget(ui, "/ToolsBar");
	gtk_box_pack_start(GTK_BOX(nbpage), tb, TRUE, TRUE, 0);
	gtk_toolbar_set_orientation(GTK_TOOLBAR(tb), GTK_ORIENTATION_VERTICAL);
	gtk_toolbar_set_style(GTK_TOOLBAR(tb), GTK_TOOLBAR_BOTH_HORIZ);

	/**** properties box ****/
	propbox = gtk_frame_new("properties");
	gtk_box_pack_end(GTK_BOX(nbpage), propbox, FALSE, FALSE, 0);

	/*** layer menu ***/

	nbpage = gtk_vbox_new(FALSE, 0);
	label = gtk_label_new_with_mnemonic("_layers");
	gtk_notebook_append_page(GTK_NOTEBOOK(nb), nbpage, label);

	for(i = 0; i < NUM_LAYERS; i ++)
	{
		scale = gtk_hscale_new_with_range(0.0, 1.0, 0.05);
		gtk_range_set_value(GTK_RANGE(scale), 1.0);
		gtk_box_pack_start(GTK_BOX(nbpage), scale, FALSE, FALSE, 0);
		g_signal_connect(G_OBJECT(scale), "value-changed",
			G_CALLBACK(layer_opacity_changed_cb), GINT_TO_POINTER(i));
	}

	/** drawing area **/

	sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_add2(GTK_PANED(hpaned), sw);

	draw = gtk_layout_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(sw), draw);
	gui_set_drawing_area_size();

	gtk_widget_set_events(draw,
		GDK_EXPOSURE_MASK |
		GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_PRESS_MASK |
		GDK_POINTER_MOTION_MASK |
		GDK_SCROLL_MASK);
	g_signal_connect(G_OBJECT(draw), "expose-event",
		G_CALLBACK(expose_event_cb), NULL);
	g_signal_connect(G_OBJECT(draw), "scroll-event",
		G_CALLBACK(scroll_cb), NULL);
	g_signal_connect(G_OBJECT(draw), "button-press-event",
		G_CALLBACK(button_press_cb), NULL);
	g_signal_connect(G_OBJECT(draw), "button-release-event",
		G_CALLBACK(button_release_cb), NULL);
	g_signal_connect(G_OBJECT(draw), "motion-notify-event",
		G_CALLBACK(motion_notify_cb), NULL);

	gtk_widget_show_all(window);

	board = object_board_new(15, 10);
	object_add(board);

	return TRUE;
}

void gui_redraw(void)
{
	gtk_widget_queue_draw_area(draw, 0, 0,
		draw->allocation.width, draw->allocation.height);
}

void gui_set_drawing_area_size(void)
{
	gtk_layout_set_size(GTK_LAYOUT(draw),
		HOLES_TO_PX(g_holes_x + 4),
		HOLES_TO_PX(g_holes_y + 4));
}

gboolean gui_set_selection(RsvgHandle *svg)
{
	if(g_selection_svg)
		rsvg_handle_free(g_selection_svg);
	g_selection_svg = svg;

	gui_redraw();
	return TRUE;
}

gboolean gui_set_preview(RsvgHandle *svg)
{
	if(g_preview_svg)
		rsvg_handle_free(g_preview_svg);
	g_preview_svg = svg;

	gui_redraw();
	return TRUE;
}

gboolean gui_set_properties_widget(GtkWidget *widget)
{
	GList *children;

	/* remove children */
	children = gtk_container_get_children(GTK_CONTAINER(propbox));
	if(g_list_length(children) > 1)
		gtk_container_remove(GTK_CONTAINER(propbox), GTK_WIDGET(children->data));

	if(widget)
		gtk_container_add(GTK_CONTAINER(propbox), widget);
	return TRUE;
}

static void radio_clicked_cb(GtkAction *action, GtkRadioAction *current,
	gpointer data)
{
	tools_select(gtk_radio_action_get_current_value(current));
}

static gboolean expose_event_cb (GtkWidget *widget, GdkEventExpose *event,
	gpointer data)
{
	cairo_t *cairo = NULL;
#if 0
	cairo_pattern_t *pat;
#endif
	Layer *layer;
	gint32 i;

	cairo = gdk_cairo_create(GTK_LAYOUT(widget)->bin_window);
	cairo_scale(cairo, g_scale, g_scale);

	for(i = 0; i < NUM_LAYERS; i ++)
	{
		layer = layers_get(i);
		if(layer->svg)
		{
#if 0
			pat = cairo_pattern_create_rgba(0, 0, 0, layer->opacity);
			cairo_mask(cairo, pat);
			cairo_new_path(cairo);
#endif
#if 0
			cairo_push_group(cairo);
#endif
			rsvg_handle_render_cairo(layer->svg, cairo);
#if 0
			cairo_pop_group_to_source(cairo);
			cairo_paint_with_alpha(cairo, layer->opacity);
#endif
#if 0
			cairo_pattern_destroy(pat);
#endif
		}
	}

	if(g_selection_svg != NULL)
		rsvg_handle_render_cairo(g_selection_svg, cairo);

	if(g_preview_svg != NULL)
		rsvg_handle_render_cairo(g_preview_svg, cairo);

	cairo_destroy(cairo);

	return TRUE;
}

static gboolean button_release_cb(GtkWidget *widget, GdkEventButton *event,
	gpointer data)
{
	gint32 ax, ay, bx, by;

	ax = PX_TO_HOLE(g_drag_ox);
	ay = PX_TO_HOLE(g_drag_oy);
	bx = PX_TO_HOLE(event->x);
	by = PX_TO_HOLE(event->y);

	if(ON_BOARD(bx, by))
	{
		if(ON_BOARD(ax, ay) &&
			((ax != bx) || (ay != by)))
			/* drag and drop */
			tools_drag_n_drop(g_drag_ox, g_drag_oy, event->x, event->y);
		else
			/* click */
			tools_click(PX_TO_POINT(event->x), PX_TO_POINT(event->y), bx, by);
		gui_redraw();
	}

	/* reset drag origin */
	g_drag_ox = -1;
	g_drag_oy = -1;

	gui_set_preview(NULL);

	return TRUE;
}

static gboolean button_press_cb(GtkWidget *widget, GdkEventButton *event,
	gpointer data)
{
	g_drag_ox = event->x;
	g_drag_oy = event->y;
	return TRUE;
}

static gboolean scroll_cb(GtkWidget *widget, GdkEventScroll *event,
	gpointer data)
{
	switch(event->direction)
	{
		case GDK_SCROLL_DOWN:
			if(g_scale > 0.05)
				g_scale /= 1.5;
			break;
		case GDK_SCROLL_UP:
			if(g_scale < 2.00)
				g_scale *= 1.5;
			break;
		default:
			break;
	}
	gui_set_drawing_area_size();
	gui_redraw();
	return TRUE;
}

static gboolean motion_notify_cb(GtkWidget *widget, GdkEventMotion *event,
	gpointer data)
{
	/* only handle motion if mouse is pressed */
	if(g_drag_ox >= 0)
	{
		if((g_drag_ox != event->x) || (g_drag_oy != event->y))
			tools_handle_motion(g_drag_ox, g_drag_oy, event->x, event->y);
		else
			gui_set_preview(NULL);
	}

	return TRUE;
}

static gboolean layer_opacity_changed_cb(GtkRange *range, gpointer data)
{
	layers_set_opacity(GPOINTER_TO_INT(data), gtk_range_get_value(range));
	return TRUE;
}

gint32 gui_px2hole(gint32 px)
{
	return PX_TO_HOLE(px);
}

gint32 gui_hole2px(gint32 hole)
{
	return HOLE_TO_PX(hole);
}

gdouble gui_px2point(gint32 px)
{
	return PX_TO_POINT(px);
}
