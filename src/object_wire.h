#ifndef _OBJECT_WIRE_H
#define _OBJECT_WIRE_H

typedef struct {
	guint32 color;
} ObjectWire;

Object *object_wire_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2,
	guint32 color);

#define object_wire_new_default(x1, y1, x2, y2) \
	object_wire_new(x1, y1, x2, y2, 0x20FF40FF)

#endif /* _OBJECT_WIRE_H */
