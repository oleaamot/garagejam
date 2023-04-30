/* $Id$

   Copyright (C) 2023  Aamot Broadcast
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 0.0.1 (2023-04-30)
   Website: http://www.gingerblue.org/

 */

#include <gtk/gtk.h>
#include <gst/gst.h>
#include "garagejam.h"
#include "garagejam-gingerblue-studio-config.h"

extern GtkWidget *computer_entry;
extern GtkWidget *studio_entry;

GtkWidget *garagejam_main_loop (GingerblueData *garagejam) {
	GingerblueData *Gingerblue = garagejam;
	Gingerblue->window = main_studio_config (gtk_entry_get_text(GTK_ENTRY(studio_entry)), gtk_entry_get_text(GTK_ENTRY(computer_entry)));
	gtk_window_set_title (Gingerblue->window, g_strconcat(gtk_entry_get_text(GTK_ENTRY(computer_entry)), " on ", gtk_entry_get_text(GTK_ENTRY(studio_entry)), NULL));
        gtk_widget_show_all (Gingerblue->window);
}
