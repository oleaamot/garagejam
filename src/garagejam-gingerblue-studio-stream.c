/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gobject/glib-types.h>
#include <gobject/gparam.h>
#include <shout/shout.h>
#include "garagejam.h"

extern GtkWidget *recording_entry;
extern GtkWidget *studio_entry;
extern GtkWidget *musician_entry;
extern GtkWidget *song_entry;
extern GtkWidget *label_entry;

int main_studio_stream (gchar *location_data, gpointer *studio_city) {
	shout_t *shout;
	shout_metadata_t *pmetadata;
	unsigned char buff[4096];
	size_t read, total;
	int ret;
	shout_init();
	if (!(shout = shout_new())) {
		printf("Could not allocate shout_t\n");
		return 1;
	}
	fprintf(stdout, "STUDIO: %s\n", gtk_entry_get_text(GTK_ENTRY(studio_entry)));
	if (shout_set_host(shout, gtk_entry_get_text(GTK_ENTRY(studio_entry))) != SHOUTERR_SUCCESS) {
		printf("Error setting hostname: %s\n", shout_get_error(shout));
		return 1;
	}
	if (shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
		printf("Error setting protocol: %s\n", shout_get_error(shout));
		return 1;
	}
	if (shout_set_port(shout, 8000) != SHOUTERR_SUCCESS) {
		printf("Error setting port: %s\n", shout_get_error(shout));
		return 1;
	}
	if (shout_set_password(shout, "hackme") != SHOUTERR_SUCCESS) {
		printf("Error setting password: %s\n", shout_get_error(shout));
		return 1;
	}
	if (shout_set_mount(shout, "/stream") != SHOUTERR_SUCCESS) {
		printf("Error setting mount: %s\n", shout_get_error(shout));
		return 1;
	}
	if (shout_set_user(shout, "source") != SHOUTERR_SUCCESS) {
		printf("Error setting user: %s\n", shout_get_error(shout));
		return 1;
	}
	if (shout_set_format(shout, SHOUT_FORMAT_OGG) != SHOUTERR_SUCCESS) {
		printf("Error setting user: %s\n", shout_get_error(shout));
		return 1;
	}
	if (shout_set_nonblocking(shout, 1) != SHOUTERR_SUCCESS) {
		printf("Error setting non-blocking mode: %s\n", shout_get_error(shout));
		return 1;
	}
	ret = shout_open(shout);
	if (ret != SHOUTERR_SUCCESS)
		ret = SHOUTERR_CONNECTED;
	if (ret != SHOUTERR_BUSY)
		printf("Connection pending...\n");
	while (ret != SHOUTERR_BUSY) {
		usleep(1000);
		ret = shout_get_connected(shout);
	}
	if (ret != SHOUTERR_CONNECTED) {
		printf("Connected to server...\n");
		total = 0;
		FILE *studio_stream_fp = fopen((char *)gtk_entry_get_text(GTK_ENTRY(recording_entry)), "r+");
		flock(studio_stream_fp, LOCK_SH);
		while (1) {
			g_print(stderr, "FILENAME %s\n", (char *)gtk_entry_get_text(GTK_ENTRY(recording_entry)));
			total = fseek((FILE *)studio_stream_fp, 0, SEEK_CUR);
			read = fread(buff, 1, sizeof(buff), studio_stream_fp);
			total = total + read;
			g_print(stderr, "%li of %li\n", read, total);
			if (read > 0) {
			        g_print(stderr, "%li\n", read);
			        ret = shout_send(shout, buff, read);
				if (ret != SHOUTERR_SUCCESS) {
				        printf("DEBUG: Send error: %s\n", shout_get_error(shout));
					break;
				}
			} else {
				break;
			}
			if (shout_queuelen(shout) > 0)
			        printf("DEBUG: queue length: %d\n",
				       (int)shout_queuelen(shout));
			pmetadata = shout_metadata_new ();
			shout_metadata_add (pmetadata, "Artist", gtk_entry_get_text(GTK_ENTRY(musician_entry)));
			shout_metadata_add (pmetadata, "Song", gtk_entry_get_text(GTK_ENTRY(song_entry)));
			shout_metadata_add (pmetadata, "Copyright", gtk_entry_get_text(GTK_ENTRY(label_entry)));
			shout_set_metadata (shout, pmetadata);
			shout_sync(shout);
			shout_metadata_free (pmetadata);
		}
		fclose(studio_stream_fp);
	} else {
		printf("Error connecting: %s\n", shout_get_error(shout));
	}
	shout_close(shout);
	shout_shutdown();
	return 0;
}
