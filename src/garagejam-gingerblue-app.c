/* $Id$

   Copyright (C) 2023  Aamot Broadcast
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 0.0.1 (2023-04-30)
   Website: http://www.gingerblue.org/

 */

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include "garagejam.h"
#include "garagejam-gingerblue-config.h"
#include "garagejam-gingerblue-main.h"
#include "garagejam-gingerblue-main-loop.h"
#include "garagejam-gingerblue-studio-config.h"

int main_app(gint argc, gchar **argv)
{
	GingerblueData *garagejam_config;
	GtkWindow *garagejam_window;
	gtk_init(&argc, &argv);
	garagejam_config =
	    main_config(garagejam_window, "studios.gingerblue.org");
	garagejam_window = garagejam_main_loop(garagejam_config);
	gtk_widget_show_all(garagejam_window);
	gst_init(&argc, &argc);
	gtk_main();
	return (0);
}
