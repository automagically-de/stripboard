#ifndef _OBJECT_LED_H
#define _OBJECT_LED_H

typedef struct {
	gint32 x1;
	gint32 y1;
	gint32 x2;
	gint32 y2;
	guint32 color;
} ObjectLED;

Object *object_led_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2,
	guint32 color);

#endif /* _OBJECT_LED_H */
