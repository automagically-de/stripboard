#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#define SCALE 0.2
#define DPI 90.0

#define HOLES_TO_POINT(x) ((x) * 72.0)
#define HOLE_TO_POINT(x) (((x) + 2) * 72.0 + 36.0)
#define AREA_TO_POINT(x) HOLES_TO_POINT((x) + 2)

extern guint32 g_holes_x, g_holes_y;

#define ON_BOARD(x, y) \
	(((x) >= 0) && ((y) >= 0) && ((x) < g_holes_x) && ((y) < g_holes_y))

#endif /* _MAIN_H */
