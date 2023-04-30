/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/
 
 */

#ifndef _GINGERBLUE_H_
#define _GINGERBLUE_H_ 1

#include <gtk/gtk.h>

#define GINGERBLUE_STUDIO_PLAYER_TRUE 1
#define GINGERBLUE_STUDIO_PLAYER_FALSE 0

typedef struct _GingerblueData GingerblueData;

struct _GingerblueData {
	GtkWidget *knob;
	gint player_status;
	gchar *line;
	gint jack;
	gchar *label;
	gboolean lpf;
	gboolean hpf;
	gchar *musician;
	gchar *musical_instrument;
	gchar *version;
	gchar *volume;
	GingerblueData *next;
	GingerblueData *prev;
	GtkWidget *window;
	GMainLoop *player_loop;
};

void gb_window_break_record (GtkButton *record, GtkVolumeButton *volume);
void gb_window_pause_record (GtkButton *record, GtkVolumeButton *volume);
GingerblueData *gb_window_new_record (GtkButton *record, GtkVolumeButton *volume);
GingerblueData *gb_window_store_volume (GtkButton *record, GtkVolumeButton *volume);
gdouble gb_window_set_volume (GtkVolumeButton *volume, gdouble value);
gdouble gb_window_new_volume (GtkVolumeButton *volume, gchar *msg);
gdouble gb_window_get_volume (GtkVolumeButton *volume);

gint gb_exit (void);

#endif /* _GINGERBLUE_H_ */
