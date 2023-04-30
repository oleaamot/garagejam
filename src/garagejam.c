/* $Id$

   Copyright (C) 2023 Aamot Broadcast
   Author(s): Ole Aamot <ole@aamot.org>
   License: GNU GPL version 3
   Version: 0.0.1 (2023-04-30)
   Website: http://www.garagejam.org/

 */

#include <glib/gi18n.h>
#include <gst/gst.h>
#include <gtk/gtk.h>
#include "garagejam.h"
#include "garagejam-gingerblue-file.h"

gint
gb_exit (void) {
	gst_deinit();
	gtk_main_quit();
}

void
gb_window_break_record (GtkButton *record, GtkVolumeButton *volume) {
	/* gtk_button_set_label(GTK_BUTTON (cue), "Continue Recording"); */
	/* g_signal_connect (GTK_BUTTON (cue), "clicked", G_CALLBACK (gb_window_new_record), garagejam_data->volume); */
}

void
gb_window_pause_record (GtkButton *record, GtkVolumeButton *volume) {
	/* gtk_button_set_label(GTK_BUTTON (cue), "Continue Recording"); */
	/* g_signal_connect (GTK_BUTTON (cue), "clicked", G_CALLBACK (gb_window_new_record), garagejam_data->volume); */
}

GingerblueData *
gb_window_new_record (GtkButton *record, GtkVolumeButton *volume) {
	/* gtk_button_set_label(GTK_BUTTON (record), "Stop Recording"); */
}

GingerblueData *
gb_window_store_volume (GtkButton *record, GtkVolumeButton *volume) {
	/* gtk_button_set_label(GTK_BUTTON (record), "Stop Recording"); */
}

gdouble
gb_window_set_volume (GtkVolumeButton *volume, gdouble value) {
	gtk_scale_button_set_value (GTK_SCALE_BUTTON (volume), (gdouble) value);
}

gdouble
gb_window_new_volume (GtkVolumeButton *volume, gchar *msg) {
	g_print ("New volume: %0.2f\n", (gdouble) gtk_scale_button_get_value (GTK_SCALE_BUTTON (volume)));
	return (gdouble) gtk_scale_button_get_value (GTK_SCALE_BUTTON (volume));
}

gdouble
gb_window_get_volume (GtkVolumeButton *volume) {
	return (gdouble) gtk_scale_button_get_value (GTK_SCALE_BUTTON (volume));
}
