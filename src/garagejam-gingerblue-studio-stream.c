/* $Id$

   Copyright (C) 2023 Aamot Broadcast
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 0.5.0 (2023-08-07)
   Website: http://www.garagejam.org/

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/file.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gobject/glib-types.h>
#include <gobject/gparam.h>
#include <gst/player/player.h>
#include "garagejam.h"

#define SERVER_IP "178.255.144.178"	// Replace with your server IP
#define SERVER_PORT 12348	// Replace with your server port

extern GtkWidget *recording_entry;
extern GtkWidget *studio_entry;
extern GtkWidget *musician_entry;
extern GtkWidget *song_entry;
extern GtkWidget *label_entry;
extern gchar xspfbuffer[8196];
int main_studio_stream(gchar *location_data, gpointer *studio_city)
{

	GstPlayer *player;
	int sockfd;
	struct sockaddr_in server_addr;

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Socket creation error");
		return 1;
	}

	// Configure server address
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
		perror("Invalid address or address family");
		return 1;
	}

	// Connect to the server
	if (connect
	    (sockfd, (struct sockaddr *) &server_addr,
	     sizeof(server_addr)) < 0) {
		perror("Connection failed");
		return 1;
	}

	// Simulated audio data (replace with actual audio capture)
	const char *audio_data = g_strdup(xspfbuffer);
	size_t audio_len = strlen(audio_data);

	// Send audio data to server
	ssize_t bytes_sent = send(sockfd, audio_data, audio_len, 0);
	if (bytes_sent < 0) {
		perror("Send error");
		close(sockfd);
		return 1;
	}

	printf("Sent %zd bytes of audio data\n", bytes_sent);

	// Close the socket
	close(sockfd);

	player =
	    gst_player_new(NULL,
			   gst_player_g_main_context_signal_dispatcher_new
			   (NULL));
	gst_player_set_uri(GST_PLAYER(player),
			   g_strconcat("file://",
				       gtk_entry_get_text(GTK_ENTRY
							  (recording_entry)),
				       NULL));
	/* gst_player_play(GST_PLAYER(player)); */

	/* g_free (garagejam_data); */

	/* g_date_time_unref (datestamp); */

	return 0;
}
