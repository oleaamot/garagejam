/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/
 
 */

#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <gst/gst.h>
#include <gtk/gtk.h>
#include "garagejam.h"

GtkWidget *knob (GingerblueData *data, GtkWidget *line, gint jack, gchar *label, gboolean lpf, gboolean hpf) {
	GtkWidget *knob;
	knob = gtk_volume_button_new ();
	return (knob);
}
