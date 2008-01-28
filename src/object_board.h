#ifndef _OBJECT_BOARD_H
#define _OBJECT_BOARD_H

#include "object.h"

typedef struct {
	gpointer reserved;
} ObjectBoard;

Object *object_board_new(guint32 holes_x, guint32 holes_y);

#endif /* _OBJECT_BOARD_H */
