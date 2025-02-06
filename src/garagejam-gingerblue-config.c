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
	GtkWidget *garagejam;
	garagejam = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	return (GtkWidget *) garagejam;
}
