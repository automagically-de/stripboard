#ifndef _LAYERS_H
#define _LAYERS_H

#include <librsvg/rsvg.h>

typedef enum {
	LAYER_WIRES_BOTTOM,
	LAYER_STRIPS,
	LAYER_BOARD,
	LAYER_WIRES,
	LAYER_PARTS,
	NUM_LAYERS,
	/* layers normally not rendered -- so not counted, too */
	LAYER_SELECT,
	LAYER_PREVIEW
} LayerID;

typedef struct {
	LayerID id;
	GSList *objects;
	RsvgHandle *svg;
	gdouble opacity;
} Layer;

gboolean layers_init(void);
void layers_cleanup(void);
RsvgHandle *layers_render_selection_frame(gdouble x1, gdouble y1, gdouble x2,
	gdouble y2);
RsvgHandle *layers_render_custom(gpointer o, LayerID id);
gboolean layers_update(GSList *objects);
Layer *layers_get(guint32 id);
gboolean layers_set_opacity(LayerID id, gdouble opacity);

#endif /* _LAYERS_H */
