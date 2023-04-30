/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/
 
 */
 
 #include <gst/gst.h>
 #include <gtk/gtk.h>
 #include <glib/gstdio.h>
 #include <glib/gi18n.h>

GtkWidget *gb_song_new (gchar *title) {
	GtkWidget *window;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), title);
	return (window);
}
GtkWidget *gb_song_quit (gchar *title) {
	GtkWidget *window;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), title);
	return (window);
}
