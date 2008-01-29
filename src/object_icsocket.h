#ifndef _OBJECT_ICSOCKET_H
#define _OBJECT_ICSOCKET_H

typedef struct {
	gpointer reserved;
} ObjectICSocket;

Object *object_icsocket_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2);

#define object_icsocket_new_default(x1, y1, x2, y2) \
	object_icsocket_new(x1, y1, x2, y2)

#endif /* _OBJECT_ICSOCKET_H */
