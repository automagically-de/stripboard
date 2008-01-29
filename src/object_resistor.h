#ifndef _OBJECT_RESISTOR_H
#define _OBJECT_RESISTOR_H

typedef struct {
	gdouble r;
	guint32 tolerance;
} ObjectResistor;

Object *object_resistor_new(guint32 x1, guint32 y1, guint32 x2,
	guint32 y2, gdouble r);

#define object_resistor_new_default(x1, y1, x2, y2) \
	object_resistor_new(x1, y1, x2, y2, 220)

#endif /* _OBJECT_RESISTOR_H */
