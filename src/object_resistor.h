#ifndef _OBJECT_RESISTOR_H
#define _OBJECT_RESISTOR_H

typedef struct {
	gint32 x1;
	gint32 y1;
	gint32 x2;
	gint32 y2;
	gdouble r;
	guint32 tolerance;
} ObjectResistor;

Object *object_resistor_new(guint32 x1, guint32 y1, guint32 x2,
	guint32 y2, gdouble r);

#endif /* _OBJECT_RESISTOR_H */
