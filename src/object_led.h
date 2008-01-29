#ifndef _OBJECT_LED_H
#define _OBJECT_LED_H

typedef struct {
	guint32 color;
} ObjectLED;

Object *object_led_new(guint32 x1, guint32 y1, guint32 x2, guint32 y2,
	guint32 color);

#define object_led_new_default(x1, y1, x2, y2) \
	object_led_new(x1, y1, x2, y2, 0x10FF10FF)

#endif /* _OBJECT_LED_H */
