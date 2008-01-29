#include "main.h"
#include "layers.h"
#include "object.h"
#include "gui.h"

#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

static Layer *layers = NULL;

typedef struct {
	guint8 *data;
	guint32 length;
} StreamData;

static cairo_status_t stream_write_cb(void *closure,
	const unsigned char *data, unsigned int length);
gboolean layers_render(cairo_t *cairo, LayerID layerid, GSList *objects);

gboolean layers_init(void)
{
	int i;

	layers = g_new0(Layer, NUM_LAYERS);

	for(i = 0; i < NUM_LAYERS; i ++)
	{
		layers[i].id = i;
		layers[i].opacity = 1.0;
	}
	return TRUE;
}

void layers_cleanup(void)
{
	g_free(layers);
}

Layer *layers_get(guint32 id)
{
	g_return_val_if_fail(id < NUM_LAYERS, NULL);
	return &(layers[id]);
}

gboolean layers_set_opacity(LayerID id, gdouble opacity)
{
	layers[id].opacity = opacity;
	gui_redraw();
	return TRUE;
}

gboolean layers_update(GSList *objects)
{
	gint32 i;
	cairo_surface_t *surface;
	cairo_t *cairo;
	GError *error;
	StreamData *stream;
	gboolean retval = TRUE;

	for(i = 0; i < NUM_LAYERS; i ++)
	{
		/* objects to cairo */
		stream = g_new0(StreamData, 1);
		surface = cairo_svg_surface_create_for_stream(stream_write_cb,
			stream,
			HOLES_TO_POINT(g_holes_x + 4), HOLES_TO_POINT(g_holes_y + 4));
		cairo = cairo_create(surface);
		layers_render(cairo, i, objects);
		cairo_destroy(cairo);
		cairo_surface_destroy(surface);

		/* cairo to rsvg */
		if(layers[i].svg != NULL)
			rsvg_handle_free(layers[i].svg);
		layers[i].svg = rsvg_handle_new();
		error = NULL;
		rsvg_handle_write(layers[i].svg, stream->data, stream->length, &error);
		if(!rsvg_handle_close(layers[i].svg, &error))
		{
			if(error != NULL)
				g_warning("RSVG Error: %s", error->message);
			rsvg_handle_free(layers[i].svg);
			retval = FALSE;
		}

		/* clean up */
		if(stream->data)
			g_free(stream->data);
		g_free(stream);
	}
	return retval;
}

gboolean layers_render(cairo_t *cairo, LayerID layerid, GSList *objects)
{
	GSList *item;

	for(item = objects; item != NULL; item = item->next)
	{
		cairo_save(cairo);
		object_render(cairo, layerid, (Object *)item->data);
		cairo_restore(cairo);
	}

	return TRUE;
}

RsvgHandle *layers_render_selection_frame(gdouble x1, gdouble y1, gdouble x2,
	gdouble y2)
{
	StreamData *stream;
	cairo_surface_t *surface;
	cairo_t *cairo;
	RsvgHandle *svg;
	GError *error;
	gdouble dashes[] = { 50.0, 10.0, 10.0, 10.0 };

	/* object to cairo */
	stream = g_new0(StreamData, 1);
	surface = cairo_svg_surface_create_for_stream(stream_write_cb, stream,
		HOLES_TO_POINT(g_holes_x + 4), HOLES_TO_POINT(g_holes_y + 4));
	cairo = cairo_create(surface);

	/* draw selection */
	cairo_rectangle(cairo,
		MIN(x1, x2), MIN(y1, y2),
		ABS(x1 - x2), ABS(y1 - y2));
	cairo_set_dash(cairo, dashes, sizeof(dashes) / sizeof(dashes[0]), 0.0);
	cairo_set_source_rgba(cairo, 0.4, 0.4, 0.4, 0.9);
	cairo_set_line_width(cairo, 4.0);
	cairo_stroke(cairo);

	cairo_destroy(cairo);
	cairo_surface_destroy(surface);

	/* cairo to rsvg */
	svg = rsvg_handle_new();
	error = NULL;
	rsvg_handle_write(svg, stream->data, stream->length, &error);
	if(!rsvg_handle_close(svg, &error))
	{
		if(error != NULL)
			g_warning("RSVG Error: %s", error->message);
		rsvg_handle_free(svg);
		svg = NULL;
	}

	/* clean up */
	if(stream->data)
		g_free(stream->data);
	g_free(stream);

	return svg;
}

RsvgHandle *layers_render_custom(gpointer o, LayerID id)
{
	StreamData *stream;
	cairo_surface_t *surface;
	cairo_t *cairo;
	RsvgHandle *svg;
	GError *error;

	/* object to cairo */
	stream = g_new0(StreamData, 1);
	surface = cairo_svg_surface_create_for_stream(stream_write_cb, stream,
		HOLES_TO_POINT(g_holes_x + 4), HOLES_TO_POINT(g_holes_y + 4));
	cairo = cairo_create(surface);
	object_render(cairo, id, (Object *)o);
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);

	/* cairo to rsvg */
	svg = rsvg_handle_new();
	error = NULL;
	rsvg_handle_write(svg, stream->data, stream->length, &error);
	if(!rsvg_handle_close(svg, &error))
	{
		if(error != NULL)
			g_warning("RSVG Error: %s", error->message);
		rsvg_handle_free(svg);
		svg = NULL;
	}

	/* clean up */
	if(stream->data)
		g_free(stream->data);
	g_free(stream);

	return svg;
}

gboolean layers_export_svg(const gchar *filename)
{
	cairo_surface_t *surface;
	cairo_t *cairo;
	gint32 i;

	surface = cairo_svg_surface_create(filename,
		HOLES_TO_POINT(g_holes_x + 4),
		HOLES_TO_POINT(g_holes_y + 4));
	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
	{
		g_warning("Export SVG: surface status: %d",
			cairo_surface_status(surface));
		cairo_surface_destroy(surface);
		return FALSE;
	}

	cairo = cairo_create(surface);
	for(i = 0; i < NUM_LAYERS; i ++)
	{
		if(layers[i].svg)
		{
			rsvg_handle_render_cairo(layers[i].svg, cairo);
		}
	}
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);

	return TRUE;
}

static cairo_status_t stream_write_cb(void *closure,
	const unsigned char *data, unsigned int length)
{
	StreamData *stream = (StreamData *)closure;

	stream->data = g_realloc(stream->data, stream->length + length);
	memcpy(stream->data + stream->length, data, length);
	stream->length += length;

	return CAIRO_STATUS_SUCCESS;
}
