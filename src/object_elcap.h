#ifndef _OBJECT_ELCAP_H
#define _OBJECT_ELCAP_H

typedef struct {
	guint32 color;
	gdouble diameter;
	gdouble dpin;
} ObjectElCap;

Object *object_elcap_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2,
	guint32 color, gdouble diameter, gdouble dpin);

#endif /* _OBJECT_ELCAP_H */
