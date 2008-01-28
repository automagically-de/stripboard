#include "main.h"
#include "object.h"

void actions_New(void)
{
	object_remove_all();
}

void actions_Save(void)
{
	static GtkWidget *fc = NULL;
	gchar *filename;

	if(fc == NULL)
	{
		fc = gtk_file_chooser_dialog_new("save the board layout to file",
			NULL, GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);
	}

	if(gtk_dialog_run(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
		object_save_to_file(filename);
	}

	gtk_widget_hide(fc);
}
