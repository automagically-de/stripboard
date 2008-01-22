#ifndef _OBJECT_ELCAP_H
#define _OBJECT_ELCAP_H

typedef struct {
	gint32 x1;
	gint32 y1;
	gint32 x2;
	gint32 y2;
	guint32 color;
	gdouble diameter;
	gdouble dpin;
} ObjectElCap;

Object *object_elcap_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2,
	guint32 color, gdouble diameter, gdouble dpin);

#endif /* _OBJECT_ELCAP_H */
