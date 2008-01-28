#include "main.h"
#include "gui.h"
#include "layers.h"
#include "object.h"

#include <locale.h>

guint32 g_holes_x = 15;
guint32 g_holes_y = 10;

int main(int argc, char *argv[])
{
	rsvg_set_default_dpi(DPI);
	object_init();
	layers_init();
	gui_init(&argc, &argv);

	setlocale(LC_NUMERIC, "C");

	gtk_main();

	object_cleanup();

	return EXIT_SUCCESS;
}
