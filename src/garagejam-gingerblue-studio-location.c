/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/

 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gobject/glib-types.h>
#include <gobject/gparam.h>
#include <champlain/champlain.h>
#include <champlain-gtk/champlain-gtk.h>
#include <geoclue.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <string.h>
#include "garagejam.h"

extern GtkWidget *recording_entry;
extern GtkWidget *studio_entry;
extern GtkWidget *musician_entry;
extern GtkWidget *song_entry;
extern GtkWidget *label_entry;

/* Commandline options */
static gint timeout = 30;	/* seconds */
static GClueAccuracyLevel accuracy_level = GCLUE_ACCURACY_LEVEL_EXACT;
static gint time_threshold = 0;

static GOptionEntry entries[] = {
	{ "timeout",
	 't',
	 0,
	 G_OPTION_ARG_INT,
	 &timeout,
	 N_("Exit after T seconds. Default: 30"),
	 "T" },
	{ "time-threshold",
	 'i',
	 0,
	 G_OPTION_ARG_INT,
	 &time_threshold,
	 N_("Only report location update after T seconds. "
	    "Default: 0 (report new location without any delay)"),
	 "T" },
	{ "accuracy-level",
	 'a',
	 0,
	 G_OPTION_ARG_INT,
	 &accuracy_level,
	 N_("Request accuracy level A. "
	    "Country = 1, "
	    "City = 4, " "Neighborhood = 5, " "Street = 6, " "Exact = 8."),
	 "A" },
	{ NULL }
};

GClueSimple *simple = NULL;
GClueClient *client = NULL;
GMainLoop *main_loop;

static gchar *print_location(GClueSimple *simple, ChamplainView *view)
{
	GClueLocation *location;
	gdouble altitude, speed, heading;
	GVariant *timestamp;
	GTimeVal tv = { 0 };
	const char *desc;
	gchar *geostring;
	location = gclue_simple_get_location(simple);
	g_print("\nNew location:\n");
	g_print("Latitude:    %f°\nLongitude:   %f°\n",
		gclue_location_get_latitude(location),
		gclue_location_get_longitude(location));

	champlain_view_center_on(CHAMPLAIN_VIEW(view),
				 gclue_location_get_latitude(location),
				 gclue_location_get_longitude(location));

	altitude = gclue_location_get_altitude(location);
	if (altitude != -G_MAXDOUBLE)
		g_print("Altitude:    %f meters\n", altitude);
	speed = gclue_location_get_speed(location);
	if (speed >= 0)
		g_print("Speed:       %f meters/second\n", speed);
	heading = gclue_location_get_heading(location);
	if (heading >= 0)
		g_print("Heading:     %f°\n", heading);

	desc = gclue_location_get_description(location);
	if (strlen(desc) > 0)
		g_print("Description: %s\n", desc);

	timestamp = gclue_location_get_timestamp(location);
	if (timestamp) {
		GDateTime *date_time;
		gchar *str;

		g_variant_get(timestamp, "(tt)", &tv.tv_sec, &tv.tv_usec);

		date_time = g_date_time_new_from_timeval_local(&tv);
		str = g_date_time_format
		    (date_time, "%c (%s seconds since the Epoch)");
		g_date_time_unref(date_time);

		g_print("Timestamp:   %s\n", str);
		g_free(str);
	}
	geostring =
	    g_strconcat("  <glat>", gclue_location_get_latitude(location),
			"</glat>", "  <glon>",
			gclue_location_get_longitude(location), "</glon>",
			"  <galt>", gclue_location_get_altitude(location),
			"</galt>", NULL);
	return geostring;
}

static gboolean on_location_timeout(gpointer user_data)
{
	g_clear_object(&client);
	g_clear_object(&simple);
	g_main_loop_quit(main_loop);

	return FALSE;
}

static void
on_client_active_notify(GClueClient *client,
			GParamSpec *pspec, gpointer user_data)
{
	if (gclue_client_get_active(client))
		return;

	g_print("Geolocation disabled. Quitting..\n");
	on_location_timeout(NULL);
}

static void
on_simple_ready(GObject *source_object,
		GAsyncResult *res, gpointer user_data)
{
	GError *error = NULL;

	simple = gclue_simple_new_finish(res, &error);
	if (error != NULL) {
		g_critical("Failed to connect to GeoClue2 service: %s",
			   error->message);

		exit(-1);
	}
	client = gclue_simple_get_client(simple);
	if (client) {
		g_object_ref(client);
		g_print("Client object: %s\n",
			g_dbus_proxy_get_object_path(G_DBUS_PROXY
						     (client)));

		g_signal_connect(client,
				 "notify::active",
				 G_CALLBACK(on_client_active_notify),
				 NULL);
	}
	print_location(simple, user_data);

	g_signal_connect(simple,
			 "notify::location",
			 G_CALLBACK(print_location), user_data);
}


gchar *studio_location_navigate(ChamplainView *view, gpointer *data)
{
	GClueLocation *location;
	GClueSimple *simple;
	gdouble altitude, speed, heading;
	GVariant *timestamp;
	GTimeVal tv = { 0 };
	/*	gclue_simple_new("garagejam",
			 accuracy_level,
			 &time_threshold,
			 on_simple_ready, CHAMPLAIN_VIEW(view)); */
	gtk_entry_set_text(GTK_ENTRY(data), print_location(simple, NULL));
	return print_location(simple, NULL);
}
