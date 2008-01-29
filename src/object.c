#include "main.h"
#include "object.h"
#include "misc.h"
#include "gui.h"
#include "object_board.h"
#include "property.h"

static GSList *objects = NULL;
static GSList *objects_selected = NULL;

gboolean object_init(void)
{
	return TRUE;
}

void object_cleanup(void)
{
}

gboolean object_save_to_file(const gchar *filename)
{
	GSList *oitem, *pitem;
	FILE *f;
	Object *o;
	Property *p;
	gchar *s, *t1, *t2;

	f = fopen(filename, "w");
	if(f == NULL)
	{
		g_warning("failed to open %s: %s (%d)", filename,
			strerror(errno), errno);
		return FALSE;
	}

	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<stripboard>\n", f);
	for(oitem = objects; oitem != NULL; oitem = oitem->next)
	{
		o = (Object *)oitem->data;
		s = g_markup_printf_escaped(
			"\t<object type=\"%s\">\n"
			"\t\t<coordinates x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" />\n",
			o->type->title, o->x1, o->y1, o->x2, o->y2);
		fputs(s, f);
		g_free(s);
		/* properties */
		for(pitem = o->properties; pitem != NULL; pitem = pitem->next)
		{
			p = (Property *)pitem->data;
			t1 = property_type_to_str(p->priv->type);
			t2 = property_var_to_str(p->priv->type, p->var);
			s = g_markup_printf_escaped(
				"\t\t<property type=\"%s\" title=\"%s\" value=\"%s\" />\n",
				t1, p->title, t2);
			fputs(s, f);
			g_free(s);
			g_free(t1);
			g_free(t2);
		}
		fputs("\t</object>\n", f);
	}
	fputs("</stripboard>\n", f);

	fclose(f);

	return TRUE;
}

static void parse_start_elem(GMarkupParseContext *context, const gchar *elem,
	const gchar **attribute_names, const gchar **attribute_values,
	gpointer user_data, GError **error)
{
	Object *o;

	if(strcmp(elem, "object") == 0)
	{
		if(strcmp(attribute_values[0], "board") != 0)
		{
			o = object_wire_new(0, 0, 1, 1, 0x0);
		}
		else
			o = (Object *)g_slist_nth_data(objects, 0);
		*((Object **)user_data) = o;
	}
	else if(strcmp(elem, "coordinates") == 0)
	{
		o = *((Object **)user_data);
		if(o != NULL)
		{
			o->x1 = atoi(attribute_values[0]);
			o->y1 = atoi(attribute_values[1]);
			o->x2 = atoi(attribute_values[2]);
			o->y2 = atoi(attribute_values[3]);
		}
	}
	else if(strcmp(elem, "property") == 0)
	{
		o = *((Object **)user_data);
		if(o != NULL)
			property_set_from_str(o, attribute_values[0], attribute_values[1],
				attribute_values[2]);
	} /* "property" */
}

static void parse_end_elem(GMarkupParseContext *context, const gchar *elem,
	gpointer user_data, GError **error)
{
	Object *o;

	if(strcmp(elem, "object") == 0)
	{
		o = *((Object **)user_data);
		if(o != NULL)
		{
			/* add object to list -- but not the board */
			if(strcmp(o->type->title, "board") != 0)
				object_add(o);
			*((Object **)user_data) = NULL;
		}
	}
	else if(strcmp(elem, "stripboard") == 0)
	{
		*error = g_error_new(g_quark_from_string("stripboard"), 0xDEADBEEF,
			"finished reading xml data");
	}
}

gboolean object_load_from_file(const gchar *filename)
{
	FILE *f;
	gint32 r, l, c;
	gchar buffer[2048];
	GMarkupParseContext *ctx;
	static const GMarkupParser parser = { parse_start_elem, parse_end_elem,
		NULL, /* text */ NULL, /* passthrough */ NULL  /* error */ };
	GError *error;
	Object *o = NULL;
	gboolean finished_parsing = FALSE;

	/* remove objects */
	object_remove_all();

	f = fopen(filename, "r");
	if(f == NULL)
	{
		g_warning("failed to open file '%s' for reading: %s (%d)",
			filename, strerror(errno), errno);
		return FALSE;
	}

	ctx = g_markup_parse_context_new(&parser, 0, &o, NULL);

	while((!finished_parsing) && (!feof(f)))
	{
		fgets(buffer, 2048, f);
		r = strlen(buffer);

		/* skip empty lines */
		if(r <= 0)
			continue;

		error = NULL;
		if(g_markup_parse_context_parse(ctx, buffer, r, &error) != TRUE)
		{
			if(error->code != 0xDEADBEEF)
			{
				g_markup_parse_context_get_position(ctx, &l, &c);
				g_warning("XML parse failed for file '%s' "
					"at line %d, char %d: %s (%d)",
					filename, l, c, error->message, error->code);
				g_error_free(error);
				g_markup_parse_context_free(ctx);
				fclose(f);
				return FALSE;
			}
			else
			{
				g_error_free(error);
				finished_parsing = TRUE;
			}
		}
	}
	fclose(f);

	/* update global variables */
	o = (Object *)g_slist_nth_data(objects, 0);
	g_holes_x = o->x2;
	g_holes_y = o->y2;
	gui_set_drawing_area_size();

	g_markup_parse_context_free(ctx);
	return FALSE;
}

gboolean object_render(cairo_t *cairo, LayerID layerid, Object *object)
{
	ObjectType *otype;

	otype = object->type;

	if(otype == NULL)
		return FALSE;

	if(otype->render)
		return otype->render(cairo, layerid, object);

	return TRUE;
}

Object *object_create(ObjectType *type, gpointer data,
	gint32 x1, gint32 y1, gint32 x2, gint32 y2)
{
	Object *object;

	object = g_new0(Object, 1);
	object->type = type;
	object->data = data;

	object->x1 = x1;
	object->y1 = y1;
	object->x2 = x2;
	object->y2 = y2;

	return object;
}

gboolean object_add(Object *object)
{
	objects = g_slist_append(objects, object);
	layers_update(objects);

	return TRUE;
}

gboolean object_remove_full(Object *object, gboolean delete)
{
	GSList *item;
	Object *o;

	for(item = objects; item != NULL; item = item->next)
	{
		if(item->data == object)
		{
			o = (Object *)item->data;
			objects = g_slist_remove(objects, o);
			if(delete)
				object_delete(o);
			break;
		}
	}

	layers_update(objects);
	gui_redraw();
	return TRUE;
}

gboolean object_remove(Object *object)
{
	return object_remove_full(object, TRUE);
}

gboolean object_remove_all(void)
{
	GSList *item = objects->next;
	Object *o;

	while(item != NULL)
	{
		o = (Object *)item->data;
		item = item->next;
		object_remove_full(o, TRUE);
	}
	return FALSE;
}

void object_delete(Object *object)
{
	if(object->data)
		g_free(object->data);
	g_free(object);
}

gboolean object_swap_polarity(Object *object)
{
	gint32 s1, s2;

	s1 = object->x1;
	s2 = object->y1;
	object->x1 = object->x2;
	object->y1 = object->y2;
	object->x2 = s1;
	object->y2 = s2;

	layers_update(objects);
	gui_redraw();

	return FALSE;
}

gboolean object_redraw(Object *object)
{
	layers_update(objects);
	gui_redraw();
	return TRUE;
}

static void clear_selection(void)
{
	g_slist_free(objects_selected);
	objects_selected = NULL;
}

static gboolean add_to_selection(gpointer o)
{
	if(g_slist_find(objects_selected, o))
		return FALSE;
	else
		objects_selected = g_slist_append(objects_selected, o);
	return TRUE;
}

Object *object_select(gdouble x, gdouble y, gint32 hx, gint32 hy)
{
	GSList *item;

	clear_selection();

	for(item = objects; item != NULL; item = item->next)
	{
		Object *o = (Object *)item->data;
		if(o->type->select && o->type->select(o, x, y, hx, hy))
		{
			add_to_selection(o);
			return o;
		}
	}
	return NULL;
}

Object *object_get_selected(void)
{
	if(objects_selected)
		return (Object *)objects_selected->data;
	return NULL;
}

Object *object_get_board(void)
{
	if(objects)
		return (Object *)objects->data;
	else
		return NULL;
}

gboolean object_select_line(Object *o, gdouble x, gdouble y,
	gint32 hx, gint32 hy)
{
	gdouble ax, ay, bx, by;

	if(o->x1 < o->x2)
	{
		ax = HOLE_TO_POINT(o->x1);
		ay = HOLE_TO_POINT(o->y1);
		bx = HOLE_TO_POINT(o->x2);
		by = HOLE_TO_POINT(o->y2);
	}
	else
	{
		bx = HOLE_TO_POINT(o->x1);
		by = HOLE_TO_POINT(o->y1);
		ax = HOLE_TO_POINT(o->x2);
		ay = HOLE_TO_POINT(o->y2);
	}

	if(((ax - 36)> x) || (x > (bx + 36)) ||
		((MIN(ay, by) -36) > y) || (y > (MAX(ay, by) + 36)))
		return FALSE;

	if(misc_delta_p(ax, ay, bx, by, x, y) > 36)
		return FALSE;

	return TRUE;
}
