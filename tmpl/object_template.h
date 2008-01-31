#ifndef _OBJECT_%TEMPLATE%_H
#define _OBJECT_%TEMPLATE%_H

Object *object_%template%_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2);

#define object_%template%_new_default(x1, y1, x2, y2) \
	object_%template%_new(x1, y1, x2, y2)

#endif /* _OBJECT_%TEMPLATE%_H */
