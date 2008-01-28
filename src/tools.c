#include "main.h"
#include "tools.h"
#include "misc.h"
#include "gui.h"
#include "object.h"

static ToolID g_selected_tool = TOOL_SELECT;

gboolean tools_select(ToolID id)
{
	g_selected_tool = id;
	if(g_selected_tool != TOOL_SELECT)
	{
		gui_set_selection(NULL);
		gui_set_properties_widget(NULL);
	}
	return TRUE;
}

gboolean tools_click(gint32 x, gint32 y, gint32 hx, gint32 hy)
{
	static gint32 ox = -1, oy = -1;
	Object *o;

	switch(g_selected_tool)
	{
		case TOOL_SELECT:
			o = object_select(x, y, hx, hy);
			gui_set_properties_widget(NULL);
			if(o != NULL)
			{
				gui_set_selection(layers_render_custom(o, LAYER_SELECT));
				if(o->type->properties)
					gui_set_properties_widget(o->type->properties(o));
			}
			else
			{
				gui_set_selection(NULL);
				o = object_get_board();
				if(o != NULL)
					gui_set_properties_widget(o->type->properties(o));
			}
			break;
		case TOOL_DELETE:
			o = object_select(x, y, hx, hy);
			if(o != NULL)
				object_remove(o);
			break;
		case TOOL_SWAP_POLARITY:
			o = object_select(x, y, hx, hy);
			if(o != NULL)
				object_swap_polarity(o);
			break;
		default:
			ox = x;
			oy = y;
			break;
	}
	return TRUE;
}

gboolean tools_drag_n_drop(gint32 x1, gint32 y1, gint32 x2, gint32 y2)
{
	Object *o = NULL;
	gint32 ax, ay, bx, by;

	ax = gui_px2hole(x1);
	ay = gui_px2hole(y1);
	bx = gui_px2hole(x2);
	by = gui_px2hole(y2);

	switch(g_selected_tool)
	{
		case TOOL_ADD_WIRE:
			o = object_wire_new(ax, ay, bx, by, 0x20FF40FF);
			break;
		case TOOL_ADD_RESISTOR:
			o = object_resistor_new(ax, ay, bx, by, 220);
			break;
		case TOOL_ADD_ELCAP:
			o = object_elcap_new(ax, ay, bx, by, 0x101080FF, 8, 5);
			break;
		case TOOL_ADD_LED:
			o = object_led_new(ax, ay, bx, by, 0);
			break;
		case TOOL_ADD_ICSOCKET:
			o = object_icsocket_new(ax, ay, bx, by);
			break;
		default:
			break;
	}
	if(o != NULL)
		object_add(o);

	return TRUE;
}

gboolean tools_handle_motion(gint32 x1, gint32 y1, gint32 x2, gint32 y2)
{
	RsvgHandle *svg = NULL;
	Object *o = NULL;
	gint32 ax, ay, bx, by;
	gboolean onboard;

	ax = gui_px2hole(x1);
	ay = gui_px2hole(y1);
	bx = gui_px2hole(x2);
	by = gui_px2hole(y2);
	onboard = ON_BOARD(ax, ay) && ON_BOARD(bx, by);

	switch(g_selected_tool)
	{
		case TOOL_SELECT:
			svg = layers_render_selection_frame(
				gui_px2point(x1), gui_px2point(y1),
				gui_px2point(x2), gui_px2point(y2));
			break;
		case TOOL_SWAP_POLARITY:
		case TOOL_DELETE:
			break;
		case TOOL_ADD_WIRE:
			if(onboard)
				o = object_wire_new(ax, ay, bx, by, 0);
			break;
		case TOOL_ADD_RESISTOR:
			if(onboard)
				o = object_resistor_new(ax, ay, bx, by, 220);
			break;
		case TOOL_ADD_ELCAP:
			if(onboard)
				o = object_elcap_new(ax, ay, bx, by, 0x101080FF, 8, 5);
			break;
		case TOOL_ADD_LED:
			if(onboard)
				o = object_led_new(ax, ay, bx, by, 0);
			break;
		case TOOL_ADD_ICSOCKET:
			if(onboard)
				o = object_icsocket_new(ax, ay, bx, by);
			break;
		default:
			break;
	}

	if(o != NULL)
	{
		svg = layers_render_custom(o, LAYER_PREVIEW);
		object_delete(o);
	}

	gui_set_preview(svg);

	return TRUE;
}
