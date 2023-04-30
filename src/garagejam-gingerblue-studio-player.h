/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/

 */

/* $Id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2014-2019  Aamot Software
 *
 * Author: Ole Aamot <ole@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef GINGERBLUE_STUDIO_PLAYER_PLAYER_H
#define GINGERBLUE_STUDIO_PLAYER_PLAYER_H

#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/player/player.h>

typedef struct
{
  gchar **uris;
  guint num_uris;
  gint cur_idx;

  GstPlayer *player;
  GstState desired_state;

  gboolean repeat;

  GMainLoop *loop;
} GstPlay;

void play_uri (GstPlayer *player, const gchar * next_uri);

void garagejam_studio_player_new (GstPlayer * player, const gchar * next_uri);

void garagejam_studio_player_new (GstPlayer * player, const gchar * next_uri);

void garagejam_studio_player_quit (GstPlayer *player);

void garagejam_studio_player_pause (GstPlayer *player);

void garagejam_studio_player_stop (GstPlayer *player);

void garagejam_studio_player_play (GtkWidget *widget, gpointer *recording_entry);

static gdouble get_volume (GtkWidget *widget, GstPlay *play);

#endif /* GINGERBLUE_STUDIO_PLAYER_H */
