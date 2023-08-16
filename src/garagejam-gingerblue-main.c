/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09))
   Website: http://www.garagejam.org/

 */

#include <config.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gst/gst.h>
#include <gst/player/player.h>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <champlain/champlain.h>
#include <champlain-gtk/champlain-gtk.h>
#include <string.h>
#include <glib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <dirent.h>
#include <vorbis/vorbisfile.h>
#include "garagejam.h"
#include "garagejam-gingerblue-chord.h"
#include "garagejam-gingerblue-config.h"
#include "garagejam-gingerblue-main.h"
#include "garagejam-gingerblue-main-loop.h"
#include "garagejam-gingerblue-record.h"
#include "garagejam-gingerblue-studio-config.h"
#include "garagejam-gingerblue-studio-location.h"
#include "garagejam-gingerblue-studio-stream.h"

GingerblueData *Gingerblue;

static void gb_assistant_entry_changed(GtkEditable *, GtkAssistant *,
				       GstElement *);
static void gb_assistant_button_toggled(GtkCheckButton *, GtkAssistant *);
static void gb_assistant_button_clicked(GtkButton *, GtkAssistant *);
static void gb_assistant_cancel(GtkAssistant *, gpointer);
static void gb_assistant_close(GtkAssistant *, gpointer);
static void gb_assistant_apply(GtkAssistant *, gpointer);

typedef struct {
	GtkWidget *widget;
	gint index;
	const gchar *title;
	GtkAssistantPageType type;
	gboolean complete;
} PageInfo;

typedef struct {
	gchar *title;
	gchar *location;
} PlaylistEntry;

GtkWidget *musician_entry, *musician_label;
GtkWidget *song_entry, *song_label;
GtkWidget *instrument_entry, *instrument_label;
GtkWidget *label_entry, *label_label;
GtkWidget *line_entry, *line_label;
GtkWidget *computer_entry, *computer_label;
GtkWidget *recording_entry, *recording_label;
GtkWidget *studio_entry, *studio_label;
GtkWidget *stream_entry, *stream_label;
GtkWidget *album_entry, *album_label;
GtkWidget *summary_entry, *summary_label;

GMainLoop *main_loops;

GstPlayer *player;

GstTagList *tag_list;

gchar xspfbuffer[8192];

GError *error = NULL;

#define MAX_PATH_LENGTH 1024

void write_xspf_header(FILE *file) {
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">\n");
    fprintf(file, "  <title>GNOME Playlist</title>\n");
    fprintf(file, "  <trackList>\n");
}

void write_xspf_track(FILE *file, const char *title, const char *artist, const char *album, const char *location) {
    fprintf(file, "    <track>\n");
    fprintf(file, "      <title>%s</title>\n", title);
    fprintf(file, "      <creator>%s</creator>\n", artist);
    fprintf(file, "      <album>%s</album>\n", album);
    fprintf(file, "      <location>%s</location>\n", location);
    fprintf(file, "    </track>\n");
}

void write_xspf_footer(FILE *file) {
    fprintf(file, "  </trackList>\n");
    fprintf(file, "</playlist>\n");
}

void process_ogg_file(const char *filename, FILE *xspf_file) {
    OggVorbis_File vf;
    if (ov_fopen(filename, &vf) != 0) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    vorbis_info *info = ov_info(&vf, -1);

    char title[MAX_PATH_LENGTH];
    char artist[MAX_PATH_LENGTH];
    char album[MAX_PATH_LENGTH];
    strncpy(title, filename, sizeof(title));
    strncpy(artist, gtk_entry_get_text(GTK_ENTRY(musician_entry)), sizeof(artist));
    strncpy(album, gtk_entry_get_text(GTK_ENTRY(album_entry)), sizeof(album));

    vorbis_comment *vc = ov_comment(&vf, -1);
    for (int i = 0; i < vc->comments; i++) {
        if (strstr(vc->user_comments[i], "TITLE=") == vc->user_comments[i]) {
            strncpy(title, vc->user_comments[i] + 6, sizeof(title));
        }
        if (strstr(vc->user_comments[i], "ARTIST=") == vc->user_comments[i]) {
            strncpy(artist, vc->user_comments[i] + 7, sizeof(artist));
        }
        if (strstr(vc->user_comments[i], "ALBUM=") == vc->user_comments[i]) {
            strncpy(album, vc->user_comments[i] + 6, sizeof(album));
        }
    }

    write_xspf_track(xspf_file, title, artist, album, filename);

    ov_clear(&vf);
}

// Signal handler for playlist entry selection
void on_playlist_entry_selected(GtkListBox *listbox, GtkListBoxRow *row,
				gpointer user_data)
{
	GstPlayer *player;
	PlaylistEntry *entry =
	    (PlaylistEntry *) g_object_get_data(G_OBJECT(row),
						"playlist_entry");
	if (entry) {
		player =
		    gst_player_new(NULL,
				   gst_player_g_main_context_signal_dispatcher_new
				   (NULL));
		gst_player_set_uri(GST_PLAYER(player),
				   g_strconcat("file://",
					       g_get_user_special_dir
					       (G_USER_DIRECTORY_MUSIC),
					       "/", entry->title, NULL));
		gst_player_play(GST_PLAYER(player));
		g_print("Playing: %s\n",
			g_strconcat("file://",
				    g_get_user_special_dir
				    (G_USER_DIRECTORY_MUSIC), "/",
				    entry->title, NULL));
		// Add your playback logic here
	}
}

gboolean parse_xspf_file(const gchar *filepath, GList **playlist_entries)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(filepath);
	if (doc == NULL) {
		g_print("Failed to parse %s\n", filepath);
		return FALSE;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		g_print("Empty document\n");
		xmlFreeDoc(doc);
		return FALSE;
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if (xmlStrcmp(cur->name, (const xmlChar *) "trackList") ==
		    0) {
			xmlNodePtr track = cur->xmlChildrenNode;
			while (track != NULL) {
				if (xmlStrcmp
				    (track->name,
				     (const xmlChar *) "track") == 0) {
					xmlNodePtr locationNode =
					    track->xmlChildrenNode;
					gchar *location = NULL;
					while (locationNode != NULL) {
						if (xmlStrcmp
						    (locationNode->name,
						     (const xmlChar *)
						     "location") == 0) {
							location =
							    (gchar *)
							    xmlNodeGetContent
							    (locationNode);
							break;
						}
						locationNode =
						    locationNode->next;
					}

					if (location) {
						PlaylistEntry *entry =
						    g_new0(PlaylistEntry,
							   1);
						entry->title =
						    g_filename_display_basename
						    (location);
						entry->location =
						    g_strdup(location);
						*playlist_entries =
						    g_list_prepend
						    (*playlist_entries,
						     entry);
					}
				}
				track = track->next;
			}
		}
		cur = cur->next;
	}

	xmlFreeDoc(doc);
	return TRUE;
}

static void gb_assistant_entry_changed(GtkEditable *editable,
				       GtkAssistant *assistant,
				       GstElement *pipeline)
{
	return;
}

static void gb_assistant_button_toggled(GtkCheckButton *checkbutton,
					GtkAssistant *assistant)
{
	return;
}

static void gb_assistant_button_clicked(GtkButton *button,
					GtkAssistant *assistant)
{
	GstElement *src, *conv, *enc, *muxer, *sink, *recorder;
	gchar *filename = NULL;
	GDateTime *datestamp = g_date_time_new_now_utc();
	GstElementFactory *factory;
	GTimeVal *timeval;
	gst_element_send_event(recorder, gst_event_new_eos());
	recorder = gst_pipeline_new("record_pipe");
	/*
	   FIXME: Line #59 from https://github.com/GStreamer/gst-plugins-base/blob/master/tools/gst-device-monitor.c
	   element = gst_device_create_element (device, NULL);
	   if (!element)
	   return NULL;
	   factory = gst_element_get_factory (element);
	   if (!factory) {
	   gst_object_unref (element);
	   return NULL;
	   }
	   src = gst_element_factory_create(factory, NULL);
	 */
	src = gst_element_factory_make("autoaudiosrc", "auto_source");
	conv = gst_element_factory_make("audioconvert", "convert");
	enc = gst_element_factory_make("vorbisenc", "vorbis_enc");
	muxer = gst_element_factory_make("oggmux", "oggmux");
	sink = gst_element_factory_make("filesink", "sink");
	filename =
	    g_strconcat(g_get_user_special_dir(G_USER_DIRECTORY_MUSIC),
			"/", gtk_entry_get_text(GTK_ENTRY(musician_entry)),
			"_-_", gtk_entry_get_text(GTK_ENTRY(song_entry)),
			"_[", g_date_time_format_iso8601(datestamp), "]",
			".ogg", NULL);
	g_object_set(G_OBJECT(sink), "location",
		     g_strconcat(g_get_user_special_dir
				 (G_USER_DIRECTORY_MUSIC), "/",
				 gtk_entry_get_text(GTK_ENTRY
						    (musician_entry)),
				 "_-_",
				 gtk_entry_get_text(GTK_ENTRY(song_entry)),
				 ".ogg", NULL), NULL);
	g_object_set(G_OBJECT(enc), "quality", 1.0);
	gst_bin_add_many(GST_BIN(recorder), src, conv, enc, muxer, sink,
			 NULL);
	gst_element_link_many(src, conv, enc, muxer, sink, NULL);
	gst_element_set_state(recorder, GST_STATE_PLAYING);
	datestamp = g_date_time_new_now_utc();
	gst_tag_setter_add_tags(GST_TAG_SETTER(enc),
				GST_TAG_MERGE_APPEND,
				GST_TAG_TITLE, g_get_real_name(),
				GST_TAG_ARTIST, g_get_real_name(),
				GST_TAG_ALBUM, "Voicegram",
				GST_TAG_COMMENT, "GNOME 45",
				GST_TAG_DATE,
				g_date_time_format_iso8601(datestamp),
				NULL);
	g_date_time_unref(datestamp);
	main_loops = g_main_loop_new(NULL, TRUE);
	g_main_loop_run(main_loops);
	gst_element_set_state(recorder, GST_STATE_NULL);
	g_main_loop_unref(main_loops);
	gst_object_unref(GST_OBJECT(recorder));
	g_date_time_unref(datestamp);
}

static void gb_assistant_cancel(GtkAssistant *assistant, gpointer data)
{
	if (!main_loops) {
		g_error("Quit more loops than there are.");
	} else {
		GMainLoop *loop = main_loops;
		g_main_loop_quit(loop);
		gtk_main_quit();
	}
}

static void gb_assistant_close(GtkAssistant *assistant, gpointer data)
{
	FILE *fp = NULL;
	FILE *file = NULL;

	long file_size;
	GDateTime *datestamp = g_date_time_new_now_utc();
	gchar *filename =
	    g_strconcat(g_get_user_special_dir(G_USER_DIRECTORY_MUSIC),
			"/",
			gtk_entry_get_text(GTK_ENTRY(computer_entry)),
			"_-_",
			gtk_entry_get_text(GTK_ENTRY(song_entry)), "_[",
			g_date_time_format_iso8601(datestamp), "]",
			".jam", NULL);
	fp = fopen(filename, "w");
	fprintf(fp, "<?xml version='1.0' encoding='UTF-8'?>\n");
	fprintf(fp, "<gingerblue version='%s'>\n", VERSION);
	fprintf(fp, "  <musician>%s</musician>\n",
		gtk_entry_get_text(GTK_ENTRY(musician_entry)));
	fprintf(fp, "  <song>%s</song>\n",
		gtk_entry_get_text(GTK_ENTRY(song_entry)));
	fprintf(fp, "  <instrument>%s</instrument>\n",
		gtk_entry_get_text(GTK_ENTRY(instrument_entry)));
	fprintf(fp, "  <line>%s</line>\n",
		gtk_entry_get_text(GTK_ENTRY(line_entry)));
	fprintf(fp, "  <label>%s</label>\n",
		gtk_entry_get_text(GTK_ENTRY(label_entry)));
	fprintf(fp, "  <station>%s</station>\n",
		gtk_entry_get_text(GTK_ENTRY(computer_entry)));
	fprintf(fp, "  <filename>%s</filename>\n",
		gtk_entry_get_text(GTK_ENTRY(recording_entry)));
	fprintf(fp, "  <album>%s</album>\n",
		gtk_entry_get_text(GTK_ENTRY(album_entry)));
	fprintf(fp, "  <studio>%s</studio>\n",
		gtk_entry_get_text(GTK_ENTRY(studio_entry)));
	fprintf(fp, "</gingerblue>\n");
	fclose(fp);
	g_date_time_unref(datestamp);
	/* main_studio_stream(filename, */
	/* 		   gtk_entry_get_text(GTK_ENTRY(studio_entry))); */
	gst_element_send_event(data, gst_event_new_eos());
}

static void gb_assistant_apply(GtkAssistant *assistant, gpointer data)
{
	GingerblueData *garagejam_config;
	GtkWindow *garagejam_window;
	FILE *file;
	/* gtk_init (&argc, &argv); */
	garagejam_config =
	    main_config(GTK_WIDGET(garagejam_window),
			gtk_entry_get_text(GTK_ENTRY(studio_entry)));
	garagejam_window = garagejam_main_loop(garagejam_config);
	gtk_widget_show_all(garagejam_window);
	/* gst_init(&argc, &argc); */
	/* gtk_main(); */
	gst_element_send_event(data, gst_event_new_eos());
	return;
}

GtkAssistantPageFunc gb_assistant_cb(GtkAssistant *assistant,
				     GDateTime *datestamp)
{
	/* gtk_assistant_next_page(assistant); */
}

void free_playlist_entry(gpointer data)
{
	PlaylistEntry *entry = (PlaylistEntry *) data;
	if (entry) {
		g_free(entry->title);
		g_free(entry->location);
		g_free(entry);
	}
}


int main(int argc, char **argv)
{
	GSocketConnectable *addr;
	GDateTime *datestamp;
	GingerblueData *data;
	GingerblueChord *garagejam_chord;
	GstElement *src, *conv, *enc, *muxer, *sink, *pipeline;
	GtkWidget *introduction;
	GtkEntryBuffer *default_recording_title;
	GtkWidget *entry, *label, *button, *progress, *hbox;
	GtkWidget *summary_label, *summary_entry;
	GtkWidget *garagejam_main;
	guint i;
	GtkWidget *musicianpage;
	GtkWidget *songpage;
	GtkWidget *instrumentpage;
	GtkWidget *recordpage;
	GtkWidget *window;
	GtkWidget *frame;
	GtkWidget *input;
	GtkWidget *main_window;
	GtkWidget *mixer;
	GtkWidget *control;
	GtkWidget *soundboard;
	GtkWidget *toolbar;
	GtkWidget *input_record;
	GtkWidget *input_pause;
	GtkWidget *input_break;
	GtkWidget *input_stop;
	GtkWidget *input_volume;
	gdouble input_volume_value;
	gint64 real_time;
	gchar *album;
	GtkWidget *list;
	GList *playlist_entries = NULL;
	PageInfo page[11] = {
		{ NULL, -1, "GarageJam Setup", GTK_ASSISTANT_PAGE_INTRO,
		 TRUE },
		{ NULL, -1, "Musician", GTK_ASSISTANT_PAGE_CONTENT, TRUE },
		{ NULL, -1, "Song", GTK_ASSISTANT_PAGE_CONTENT, TRUE },
		{ NULL, -1, "Instrument", GTK_ASSISTANT_PAGE_CONTENT,
		 TRUE },
		{ NULL, -1, "Input Line", GTK_ASSISTANT_PAGE_CONTENT,
		 TRUE },
		{ NULL, -1, "Label", GTK_ASSISTANT_PAGE_CONTENT, TRUE },
		{ NULL, -1, "Computer", GTK_ASSISTANT_PAGE_CONTENT, TRUE },
		{ NULL, -1, "Recording", GTK_ASSISTANT_PAGE_CONTENT,
		 TRUE },
		{ NULL, -1, "Studio", GTK_ASSISTANT_PAGE_CONTENT, TRUE },
		{ NULL, -1, "Album", GTK_ASSISTANT_PAGE_CONTENT, TRUE },
		{ NULL, -1, "Connect", GTK_ASSISTANT_PAGE_CONFIRM, TRUE },
	};
	FILE *xspf = NULL;
	datestamp = g_date_time_new_now_utc();
	gchar *filename =
	    g_strconcat(g_get_user_special_dir(G_USER_DIRECTORY_MUSIC),
			"/",
			gtk_entry_get_text(GTK_ENTRY(musician_entry)),
			"_-_",
			gtk_entry_get_text(GTK_ENTRY(song_entry)), "_[",
			g_date_time_format_iso8601(datestamp), "]",
			".ogg", NULL);
	gtk_init(&argc, &argv);
	gst_init(&argc, &argc);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	introduction = gtk_assistant_new();
	gtk_widget_set_size_request(GTK_WIDGET(introduction), 640, 480);
	gtk_window_set_title(GTK_WINDOW(introduction), "GNOME GarageJam");
	// Connect signal handler for playlist entry selection
	g_signal_connect(G_OBJECT(introduction), "destroy",
			 G_CALLBACK(gtk_main_quit), NULL);
	page[0].widget =
	    gtk_label_new(_
			  ("Welcome to GarageJam!\n\nRecord respectfully around others.\n\nClick Next to setup a music recording session!\n\nClick Cancel to stop the music recording session.\n\nClick Cancel twice to exit GarageJam."));
	page[1].widget = gtk_box_new(FALSE, 5);
	musician_label = gtk_label_new(_("Musician:"));
	musician_entry = gtk_entry_new();
	if (g_strcmp0(musician_entry, NULL) != 0)
		gtk_entry_set_text(GTK_ENTRY(musician_entry),
				   g_get_real_name());
	else
		gtk_entry_set_text(GTK_ENTRY(musician_entry),
				   gtk_entry_get_text(GTK_ENTRY
						      (musician_entry)));
	gtk_box_pack_start(GTK_BOX(page[1].widget),
			   GTK_WIDGET(musician_label), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[1].widget),
			   GTK_WIDGET(musician_entry), FALSE, FALSE, 5);
	page[2].widget = gtk_box_new(FALSE, 5);
	song_label = gtk_label_new(_("Song:"));
	song_entry = gtk_entry_new();
	if (g_strcmp0(song_entry, NULL) != 0)
		gtk_entry_set_text(GTK_ENTRY(song_entry),
				   g_strconcat(gtk_entry_get_text
					       (song_entry),
					       g_date_time_format_iso8601
					       (datestamp), NULL));
	else
		gtk_entry_set_text(GTK_ENTRY(song_entry),
				   gtk_entry_get_text(GTK_ENTRY
						      (song_entry)));
	gtk_box_pack_start(GTK_BOX(page[2].widget), GTK_WIDGET(song_label),
			   FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[2].widget), GTK_WIDGET(song_entry),
			   FALSE, FALSE, 5);
	page[3].widget = gtk_box_new(FALSE, 5);
	instrument_label = gtk_label_new(_("Instrument:"));
	instrument_entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(instrument_entry), _("Guitar"));
	gtk_box_pack_start(GTK_BOX(page[3].widget),
			   GTK_WIDGET(instrument_label), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[3].widget),
			   GTK_WIDGET(instrument_entry), FALSE, FALSE, 5);
	page[4].widget = gtk_box_new(FALSE, 5);
	line_label = gtk_label_new(_("Line Input:"));
	line_entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(line_entry), _("Mic"));
	gtk_box_pack_start(GTK_BOX(page[4].widget), GTK_WIDGET(line_label),
			   FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[4].widget), GTK_WIDGET(line_entry),
			   FALSE, FALSE, 5);
	page[5].widget = gtk_box_new(FALSE, 5);
	label_label = gtk_label_new(_("Label:"));
	label_entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(label_entry), _("GNOME"));
	gtk_box_pack_start(GTK_BOX(page[5].widget),
			   GTK_WIDGET(label_label), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[5].widget),
			   GTK_WIDGET(label_entry), FALSE, FALSE, 5);
	page[6].widget = gtk_box_new(FALSE, 5);
	computer_label = gtk_label_new(_("Computer:"));
	computer_entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(computer_entry),
			   _(g_get_host_name()));
	addr = g_network_address_new(_(g_get_host_name()), 12348);
	gtk_entry_set_text(GTK_ENTRY(computer_entry),
			   g_network_address_get_hostname(addr));
	gtk_box_pack_start(GTK_BOX(page[6].widget),
			   GTK_WIDGET(computer_label), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[6].widget),
			   GTK_WIDGET(computer_entry), FALSE, FALSE, 5);
	recording_label = gtk_button_new_with_label("Recording");
	recording_entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(recording_entry),
			   g_strconcat(g_get_user_special_dir
				       (G_USER_DIRECTORY_MUSIC), "/",
				       gtk_entry_get_text(GTK_ENTRY
							  (musician_entry)),
				       "_-_",
				       gtk_entry_get_text(GTK_ENTRY
							  (song_entry)),
				       ".ogg", NULL));
	g_signal_connect(G_OBJECT(recording_label), "clicked",
			 G_CALLBACK(gb_record_cb),
			 g_strconcat(g_get_user_special_dir
				     (G_USER_DIRECTORY_MUSIC), "/",
				     gtk_entry_get_text(GTK_ENTRY
							(musician_entry)),
				     "_-_",
				     gtk_entry_get_text(GTK_ENTRY
							(song_entry)),
				     ".ogg", NULL));
	page[7].widget = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(page[7].widget),
			   g_strconcat(g_get_user_special_dir
				       (G_USER_DIRECTORY_MUSIC), "/",
				       gtk_entry_get_text(GTK_ENTRY
							  (musician_entry)),
				       "_-_",
				       gtk_entry_get_text(GTK_ENTRY
							  (song_entry)),
				       ".ogg", NULL));
	gtk_box_pack_start(GTK_BOX(page[7].widget),
			   GTK_WIDGET(recording_label), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[7].widget),
			   GTK_WIDGET(recording_entry), FALSE, FALSE, 5);
	studio_label = gtk_button_new_with_label("Broadcasting");
	studio_entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(studio_entry),
			   g_strconcat("https://www.gingerblue.org/api/",
				       gtk_entry_get_text(GTK_ENTRY
							  (label_entry)),
				       "/",
				       gtk_entry_get_text(GTK_ENTRY
							  (computer_entry)),
				       NULL));
	g_signal_connect(G_OBJECT(studio_label), "clicked",
			 G_CALLBACK(gb_assistant_apply),
			 gtk_entry_get_text(GTK_ENTRY(studio_entry)));
	g_signal_connect(G_OBJECT(studio_entry), "clicked",
			 G_CALLBACK(gb_assistant_apply),
			 gtk_entry_get_text(GTK_ENTRY(studio_entry)));
	page[8].widget = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(page[8].widget),
			   gtk_entry_get_text(GTK_ENTRY(studio_entry)));
	gtk_box_pack_start(GTK_BOX(page[8].widget),
			   GTK_WIDGET(studio_label), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[8].widget),
			   GTK_WIDGET(studio_entry), FALSE, FALSE, 5);
	album_label = gtk_label_new("Album");
	album_entry = gtk_entry_new();
	g_signal_connect(G_OBJECT(album_label), "clicked",
			 G_CALLBACK(gb_assistant_apply),
			 gtk_entry_get_text(GTK_ENTRY(album_entry)));
	album =
	    g_strconcat(g_get_user_special_dir(G_USER_DIRECTORY_MUSIC),
			"/", gtk_entry_get_text(GTK_ENTRY(label_entry)),
			NULL);
	gtk_entry_set_text(GTK_ENTRY(album_entry), (gchar *) album);
	page[9].widget = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(page[9].widget), album);
	g_signal_connect(GTK_BUTTON(album_entry), "clicked",
			 G_CALLBACK(gb_assistant_apply),
			 GTK_ENTRY(album_entry));
	g_signal_connect(GTK_BOX(page[9].widget), "clicked",
			 G_CALLBACK(gb_assistant_apply),
			 GTK_ENTRY(album_entry));
	g_signal_connect(G_OBJECT(album_label), "clicked",
			 G_CALLBACK(gb_assistant_apply), album_entry);
	gtk_box_pack_start(GTK_BOX(page[9].widget),
			   GTK_WIDGET(album_label), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[9].widget),
			   GTK_WIDGET(album_entry), FALSE, FALSE, 5);
	stream_label = gtk_button_new_with_label("Protocol");
	stream_entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(stream_entry), "Torrent");
	g_signal_connect(G_OBJECT(stream_entry), "clicked",
			 G_CALLBACK(gb_assistant_apply),
			 gtk_entry_get_text(GTK_ENTRY(stream_entry)));
	gchar *record_attachment =
	    g_strconcat(gtk_entry_get_text(GTK_ENTRY(recording_entry)),
			NULL);
	gchar *subject =
	    g_strconcat(gtk_entry_get_text(GTK_ENTRY(label_entry)), "/",
			gtk_entry_get_text(GTK_ENTRY(computer_entry)),
			NULL);
	page[10].widget =
	    gtk_link_button_new_with_label(g_strconcat
					   ("https://www.gingerblue.org/api/",
					    subject, NULL),
					   "Connect GarageJam to Gingerblue Recording Studio API");
	gtk_entry_set_text(GTK_ENTRY(page[10].widget), "Click Apply");
	g_signal_connect(GTK_BUTTON(stream_entry), "clicked",
			 G_CALLBACK(gb_assistant_apply),
			 gtk_entry_get_text(GTK_ENTRY(studio_entry)));
	g_signal_connect(G_OBJECT(stream_label), "clicked",
			 G_CALLBACK(gb_assistant_apply),
			 gtk_entry_get_text(GTK_ENTRY(stream_entry)));
	gtk_box_pack_start(GTK_BOX(page[10].widget),
			   GTK_WIDGET(stream_label), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(page[10].widget),
			   GTK_WIDGET(stream_entry), FALSE, FALSE, 5);
	for (i = 0; i < 11; i++) {
		page[i].index =
		    gtk_assistant_append_page(GTK_ASSISTANT(introduction),
					      GTK_WIDGET(page[i].widget));
		gtk_assistant_set_page_title(GTK_ASSISTANT(introduction),
					     GTK_WIDGET(page[i].widget),
					     page[i].title);
		gtk_assistant_set_page_type(GTK_ASSISTANT(introduction),
					    GTK_WIDGET(page[i].widget),
					    page[i].type);
		gtk_assistant_set_page_complete(GTK_ASSISTANT
						(introduction),
						GTK_WIDGET(page[i].widget),
						page[i].complete);
	}
	g_signal_connect(G_OBJECT(entry), "changed",
			 G_CALLBACK(gb_assistant_entry_changed), pipeline);
	g_signal_connect(G_OBJECT(introduction), "cancel",
			 G_CALLBACK(gb_assistant_cancel), main_loops);
	g_signal_connect(G_OBJECT(introduction), "close",
			 G_CALLBACK(gb_assistant_close), pipeline);
	g_signal_connect(G_OBJECT(introduction), "apply",
			 G_CALLBACK(gb_assistant_close), pipeline);
/* musicianpage = gtk_entry_new (); */
	/* real_time = g_get_real_time(); */
	/* gtk_assistant_insert_page (introduction, */
	/*                         musicianpage, */
	/*                         0); */
	/* gtk_assistant_set_page_title (introduction, */
	/*                            musicianpage, */
	/*                            "Musician Setup"); */
	/* gtk_assistant_set_page_type (introduction, */
	/*                           musicianpage, */
	/*                           GTK_ASSISTANT_PAGE_INTRO); */
	/* songpage = gtk_entry_new (); */
	/* gtk_entry_set_text (songpage, g_strconcat(g_get_home_dir(), _("/Music/"), g_get_real_name(), " - Song.garagejam", NULL)); */
	/* real_time = g_get_real_time(); */
	/* gtk_assistant_insert_page (introduction, */
	/*                         songpage, */
	/*                         1); */
	/* gtk_assistant_set_page_title (introduction, */
	/*                            songpage, */
	/*                            "Song Setup"); */
	/* gtk_assistant_set_page_type (introduction, */
	/*                           songpage, */
	/*                           GTK_ASSISTANT_PAGE_CONTENT); */
	/* gtk_assistant_next_page(introduction); */
	/* instrumentpage = gtk_entry_new (); */
	/* gtk_entry_set_text (instrumentpage, "Guitar"); */
	/* gtk_assistant_set_page_type (introduction, */
	/*                           instrumentpage, */
	/*                           GTK_ASSISTANT_PAGE_CONTENT); */
	/* gtk_assistant_insert_page (introduction, */
	/*                         instrumentpage, */
	/*                         2); */
	/* gtk_assistant_set_page_title (introduction, */
	/*                            instrumentpage, */
	/*                            "Instrument Setup"); */
	/* recordpage = gtk_entry_new (); */
	/* gtk_entry_set_text (recordpage, "Microphone Line"); */
	/* gtk_assistant_set_page_type (introduction, */
	/*                           recordpage, */
	/*                           GTK_ASSISTANT_PAGE_SUMMARY); */
	/* gtk_assistant_insert_page (introduction, */
	/*                         recordpage, */
	/*                         3); */
	/* gtk_assistant_set_page_title (introduction, */
	/*                            recordpage, */
	/*                            "Recording Setup"); */
	/* gtk_assistant_set_page_complete (introduction, recordpage, 1); */
	/* gtk_assistant_set_forward_page_func (introduction, */
	/*                                   gb_assistant_cb, */
	/*                                   NULL, */
	/*                                   NULL); */
	/* gtk_assistant_commit (introduction); */
	gtk_widget_show_all(GTK_WIDGET(introduction));
	gst_init(&argc, &argv);
	gst_init(NULL, NULL);
	pipeline = gst_pipeline_new("record_pipe");

	src = gst_element_factory_make("autoaudiosrc", "auto_source");
	conv = gst_element_factory_make("audioconvert", "convert");
	enc = gst_element_factory_make("vorbisenc", "vorbis_enc");
	muxer = gst_element_factory_make("oggmux", "oggmux");
	sink = gst_element_factory_make("filesink", "sink");
	filename =
	    g_strconcat(g_get_user_special_dir(G_USER_DIRECTORY_MUSIC),
			"/", gtk_entry_get_text(GTK_ENTRY(musician_entry)),
			"_-_", gtk_entry_get_text(GTK_ENTRY(song_entry)),
			"_[", g_date_time_format_iso8601(datestamp), "]",
			".ogg", NULL);
	g_object_set(G_OBJECT(sink), "location",
		     g_strconcat(g_get_user_special_dir
				 (G_USER_DIRECTORY_MUSIC), "/",
				 gtk_entry_get_text(GTK_ENTRY
						    (musician_entry)),
				 "_-_",
				 gtk_entry_get_text(GTK_ENTRY(song_entry)),
				 ".ogg", NULL), NULL);
	gst_bin_add_many(GST_BIN(pipeline), src, conv, enc, muxer, sink,
			 NULL);
	gst_element_link_many(src, conv, enc, muxer, sink, NULL);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	gtk_widget_set_size_request(GTK_WIDGET(window), 800, 600);
	gtk_window_set_title(GTK_WINDOW(window),
			     g_strconcat(g_get_user_special_dir
					 (G_USER_DIRECTORY_MUSIC), "/",
					 gtk_entry_get_text(GTK_ENTRY
							    (label_entry)),
					 ".xspf", NULL));
	// Create the list
	list = gtk_list_box_new();
	gtk_container_add(GTK_CONTAINER(window), list);

	    const char *ogg_dir = g_strdup_printf("%s",
						  g_get_user_special_dir
						  (G_USER_DIRECTORY_MUSIC));
	    
	    if (ogg_dir == NULL) {
	      fprintf(stderr, "Error getting G_USER_DIRECTORY_MUSIC variable.\n");
	      return 1;
	    }

    char xspf_path[MAX_PATH_LENGTH];
    snprintf(xspf_path, sizeof(xspf_path), "%s/GNOME.xspf", ogg_dir);

    FILE *xspf_file = fopen(xspf_path, "w");
    if (xspf_file == NULL) {
        fprintf(stderr, "Error opening XSPF file for writing.\n");
        return 1;
    }

    write_xspf_header(xspf_file);

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(ogg_dir)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG && strstr(ent->d_name, ".ogg") != NULL) {
                char file_path[MAX_PATH_LENGTH];
                snprintf(file_path, sizeof(file_path), "%s/%s", ogg_dir, ent->d_name);
                process_ogg_file(file_path, xspf_file);
            }
        }
        closedir(dir);
    } else {
        fprintf(stderr, "Error opening directory.\n");
        return 1;
    }

    write_xspf_footer(xspf_file);

    fclose(xspf_file);

    // Get the user's music directory
    gchar *music_dir =
      g_strdup_printf("%s",
		      g_get_user_special_dir
		      (G_USER_DIRECTORY_MUSIC));
    gchar *playlist_file =
      g_build_filename(music_dir, "GNOME.xspf", NULL);
    
    // Parse the playlist file
    parse_xspf_file(playlist_file, &playlist_entries);
    
    // Add playlist entries to the list
    // Add playlist entries to the list
    GList *iter;
    for (iter = playlist_entries; iter != NULL;
	 iter = g_list_next(iter)) {
      PlaylistEntry *entry = (PlaylistEntry *) iter->data;
      GtkWidget *label = gtk_label_new(entry->title);
		// Create a list box row
		GtkListBoxRow *row =
		    GTK_LIST_BOX_ROW(gtk_list_box_row_new());
		// Set user data for the row
		g_object_set_data(G_OBJECT(row), "playlist_entry", entry);
		gtk_container_add(GTK_CONTAINER(row), label);
		gtk_list_box_insert(GTK_LIST_BOX(list), GTK_WIDGET(row),
				    -1);
		g_signal_connect(GTK_LIST_BOX(list), "row-selected",
				 G_CALLBACK(on_playlist_entry_selected),
				 NULL);
	}
	gtk_widget_show_all(GTK_WIDGET(window));
	main_loops = g_main_loop_new(NULL, TRUE);
	g_main_loop_run(main_loops);

	gst_element_set_state(pipeline, GST_STATE_NULL);
	g_main_loop_unref(main_loops);
	gst_object_unref(GST_OBJECT(pipeline));

	/* player = play_new ("http://stream.radionorwegian.com/56.ogg", garagejam_data->volume); */
	/* input_volume_value = gb_window_set_volume(GTK_VOLUME_BUTTON (input_volume), 0.00);   *\/ */
	/* g_signal_connect (GTK_BUTTON (input_record), "clicked", G_CALLBACK (gb_window_new_record), garagejam_data->volume); */
	/* g_signal_connect (GTK_BUTTON (input_pause), "clicked", G_CALLBACK (gb_window_pause_record), garagejam_data->volume); */
	/* g_signal_connect (GTK_BUTTON (input_break), "clicked", G_CALLBACK (gb_window_break_record), garagejam_data->volume); */
	/* g_signal_connect (GTK_VOLUME_BUTTON (input_volume), "value-changed", G_CALLBACK (gb_window_pause_record), garagejam_data->volume); */
	/* g_signal_connect (GTK_VOLUME_BUTTON (input_volume), "value-changed", G_CALLBACK (gb_window_store_volume), garagejam_data->volume);   */
	g_signal_connect(GTK_WINDOW(introduction), "destroy",
			 G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(GTK_WINDOW(introduction), "destroy",
			 G_CALLBACK(gtk_main_quit), NULL);

	/* g_free (garagejam_data); */

	g_date_time_unref(datestamp);
	gtk_main();
	g_list_free_full(playlist_entries,
			 (GDestroyNotify) free_playlist_entry);
	g_free(playlist_file);
	g_free(music_dir);
	return (0);
}
