/* $Id$
   
   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/
   
*/

#include <string.h>
#include <gst/gst.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static gboolean message_cb (GstBus * bus, GstMessage * message, gpointer user_data);
static GstPadProbeReturn unlink_cb(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
void stopRecording();
void startRecording();
int sigintHandler(int unused);
int gb_record_cb (gchar *path);

int garagejam_record_begin();
int garagejam_record_end();

typedef struct _GingerblueRecord {
  gboolean recording_found;
} GingerblueRecord;
