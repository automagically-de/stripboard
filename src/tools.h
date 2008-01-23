#ifndef _TOOLS_H
#define _TOOLS_H

typedef enum {
	TOOL_SELECT,
	TOOL_DELETE,
	TOOL_SWAP_POLARITY,

	TOOL_ADD_WIRE,
	TOOL_ADD_RESISTOR,
	TOOL_ADD_ELCAP,
	TOOL_ADD_LED,
	TOOL_ADD_ICSOCKET,
	TOOL_ADD_DIODE
} ToolID;

gboolean tools_select(ToolID id);
gboolean tools_click(gint32 x, gint32 y, gint32 hx, gint32 hy);
gboolean tools_drag_n_drop(gint32 x1, gint32 y1, gint32 x2, gint32 y2);
gboolean tools_handle_motion(gint32 x1, gint32 y1, gint32 x2, gint32 y2);

#endif /* _TOOLS_H */
