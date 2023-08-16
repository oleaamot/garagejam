/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/

 */

#include <config.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkcontainer.h>
#include <gtk/gtkwindow.h>

#include <gst/gst.h>
#include "garagejam.h"
#include "garagejam-gingerblue-main-loop.h"
#include "garagejam-gingerblue-studio-config.h"
#include "garagejam-gingerblue-studio-stream.h"
#include "garagejam-gingerblue-studio-location.h"

extern GtkWidget *computer_entry;
extern GtkWidget *studio_entry;
extern GtkWidget *recording_entry;
extern GtkWidget *album_entry;

void studio_location_selected(GtkWidget *widget, gpointer *data)
{
	g_print("Selected studios\n");
}

GtkWidget *main_config(GtkWidget *widget, gpointer *location_data)
{
	GingerblueData *Gingerblue;
	GtkButton *AddStudioButton;
	GtkButton *NewStudioButton;
	GtkBox *Studio;
	GtkListBox *Location;
	GtkListBoxRow *Computer;
	GtkWidget *Studios;
	GtkWidget *StudioLabel;
	GtkContainer *Container;
	GtkWindow *garagejam;
	garagejam = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(garagejam),
			     g_strconcat(_("Recording ("),
					 gtk_entry_get_text(GTK_ENTRY
							    (computer_entry)),
					 _(") on "),
					 gtk_entry_get_text(GTK_ENTRY
							    (studio_entry)),
					 _(" ("), PACKAGE_STRING, ")",
					 NULL));
	AddStudioButton = gtk_button_new_with_label(_("Add Studio"));
	NewStudioButton = gtk_button_new_with_label(_("New Studio"));
	Studio = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	Location = gtk_list_box_new();
	Computer = gtk_list_box_row_new();
	Studios = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(Computer), Studios);
	StudioLabel =
	    gtk_label_new(gtk_entry_get_text(GTK_ENTRY(computer_entry)));
	gtk_container_add(GTK_CONTAINER(garagejam), GTK_WIDGET(Studio));
	gtk_container_add(GTK_CONTAINER(Location), Computer);
	gtk_box_pack_start(GTK_BOX(Studio), GTK_BUTTON(NewStudioButton),
			   TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(Studios), StudioLabel, TRUE, TRUE, 0);
	g_signal_connect(GTK_BUTTON(AddStudioButton), "clicked",
			 G_CALLBACK(main_studio_config),
			 gtk_entry_get_text(GTK_ENTRY(computer_entry)));
	gtk_box_pack_start(GTK_BOX(Studio), GTK_LIST_BOX(Location), TRUE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(Studio), GTK_BUTTON(AddStudioButton),
			   TRUE, TRUE, 0);
	fprintf(stdout, "%s\n",
		gtk_entry_get_text(GTK_ENTRY
				   (gtk_list_box_get_selected_row
				    (GTK_LIST_BOX(Location)))));
	g_signal_connect(GTK_LIST_BOX(Location), "row-selected",
			 G_CALLBACK(studio_location_selected),
			 gtk_list_box_get_selected_row(GTK_LIST_BOX
						       (Location)));
	g_signal_connect(GTK_BUTTON(NewStudioButton), "clicked",
			 G_CALLBACK(studio_location_selected),
			 gtk_entry_get_text(GTK_ENTRY(computer_entry)));
	gtk_widget_show_all(GTK_WIDGET(garagejam));
	return (GtkWidget *) garagejam;
}
