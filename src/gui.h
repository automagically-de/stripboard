#ifndef _GUI_H
#define _GUI_H

#include <glib.h>
#include <librsvg/rsvg.h>

gboolean gui_init(int *argcp, char ***argvp);
void gui_set_drawing_area_size(void);
void gui_redraw(void);
gboolean gui_set_selection(RsvgHandle *svg);
gboolean gui_set_preview(RsvgHandle *svg);
gboolean gui_set_properties_widget(GtkWidget *widget);

/* these depend on "g_scale" global, so wrapped in functions */
gint32 gui_px2hole(gint32 px);
gint32 gui_hole2px(gint32 hole);
gdouble gui_px2point(gint32 px);

#endif /* _GUI_H */
