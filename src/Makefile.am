bin_PROGRAMS = stripboard

stripboard_SOURCES = \
	main.c main.h \
	misc.c misc.h \
	gui.c gui.h \
	actions.c actions.h \
	layers.c layers.h \
	tools.c tools.h \
	property.c property.h \
	object.c object.h \
	object_board.c object_board.h \
	object_wire.c object_wire.h \
	object_resistor.c object_resistor.h \
	object_elcap.c object_elcap.h \
	object_led.c object_led.h \
	object_icsocket.c object_icsocket.h

stripboard_CFLAGS = \
	-I$(top_srcdir) -I$(top_builddir) \
	-Wall -ansi -pedantic \
	@GLIB_CFLAGS@ @GTK_CFLAGS@ \
	@CAIRO_CFLAGS@ @RSVG_CFLAGS@

stripboard_LDFLAGS = \
	@GLIB_LIBS@ @GTK_LIBS@ \
	@CAIRO_LIBS@ @RSVG_LIBS@

UI_FILES = stripboard-ui.xml
uidir = $(pkgdatadir)/ui
ui_DATA = ${UI_FILES}
EXTRA_DIST = ${UI_FILES}

