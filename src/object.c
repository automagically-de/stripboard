#include "main.h"
#include "object.h"
#include "misc.h"
#include "gui.h"
#include "object_board.h"

static GSList *objects = NULL;
static GSList *objects_selected = NULL;

gboolean object_init(void)
{
	return TRUE;
}

void object_cleanup(void)
{
}

gboolean object_render(cairo_t *cairo, LayerID layerid, Object *object)
{
	ObjectType *otype;

	otype = object->type;

	if(otype == NULL)
		return FALSE;

	if(otype->render)
		return otype->render(cairo, layerid, object->data);

	return TRUE;
}

Object *object_create(ObjectType *type, gpointer data)
{
	Object *object;

	object = g_new0(Object, 1);
	object->type = type;
	object->data = data;

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
	ObjectGeneric2P *gen_o = (ObjectGeneric2P *)object->data;
	gint32 s1, s2;

	s1 = gen_o->x1;
	s2 = gen_o->y1;
	gen_o->x1 = gen_o->x2;
	gen_o->y1 = gen_o->y2;
	gen_o->x2 = s1;
	gen_o->y2 = s2;

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
		if(o->type->select && o->type->select(o->data, x, y, hx, hy))
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

gboolean object_select_line(gpointer data, gdouble x, gdouble y,
	gint32 hx, gint32 hy)
{
	ObjectGeneric2P *o;
	gdouble ax, ay, bx, by;

	o = (ObjectGeneric2P *)data;

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
