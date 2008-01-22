#ifndef _OBJECT_ICSOCKET_H
#define _OBJECT_ICSOCKET_H

typedef struct {
	gint32 x1;
	gint32 y1;
	gint32 x2;
	gint32 y2;
} ObjectICSocket;

Object *object_icsocket_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2);

#endif /* _OBJECT_ICSOCKET_H */
