#ifndef _ACTIONS_H
#define _ACTIONS_H

#include <gtk/gtk.h>

/* GtkActionEntry:
 * name, stock id, label, accelerator, tooltip, callback */

static GtkActionEntry action_entries[] =
{
	{ "FileMenuAction", NULL, "_File" },
	{ "QuitAction", GTK_STOCK_QUIT,
		"_Quit", "<control>Q",
		"Quit",
		G_CALLBACK(gtk_main_quit) }
};

static guint32 action_num_entries = G_N_ELEMENTS(action_entries);

#include "tools.h"

/* GtkRadioActionEntry:
 * name, stock id, label, accelerator, tooltip, value */

static GtkRadioActionEntry action_radio_entries[] =
{
	{ "ToolSelectAction", GTK_STOCK_INDEX, "_Select", NULL,
		"select object", TOOL_SELECT },
	{ "ToolDeleteAction", GTK_STOCK_DELETE, "_Delete", NULL,
		"delete object", TOOL_DELETE },
	{ "ToolSwapPolAction", GTK_STOCK_REFRESH, "Swap _Polarity", NULL,
		"swap polarity", TOOL_SWAP_POLARITY },
	{ "ToolAddWireAction", GTK_STOCK_ADD, "Add _Wire", NULL,
		"add wire", TOOL_ADD_WIRE },
	{ "ToolAddResAction", GTK_STOCK_ADD, "Add _Resistor", NULL,
		"add resistor", TOOL_ADD_RESISTOR },
	{ "ToolAddElCapAction", GTK_STOCK_ADD, "Add _Electrolytic Capacitor", NULL,
		"add electrolytic capacitor", TOOL_ADD_ELCAP },
	{ "ToolAddLEDAction", GTK_STOCK_ADD, "Add _LED", NULL,
		"add LED", TOOL_ADD_LED },
	{ "ToolAddICSocketAction", GTK_STOCK_ADD, "Add _IC Socket", NULL,
		"add IC socket", TOOL_ADD_ICSOCKET }
};

static guint32 action_num_radio_entries = G_N_ELEMENTS(action_radio_entries);

#endif /* _ACTIONS_H */
