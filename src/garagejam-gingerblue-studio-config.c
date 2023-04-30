/* $Id$

   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/

 */

#include <gtk/gtk.h>
#include <gst/gst.h>
#include "garagejam.h"

GtkWidget *main_studio_config (gchar *location_data, gchar *studio_city) {
       GingerblueData *Gingerblue;
       GtkVBox *Locations;
       GtkListBox *Location;
       GtkContainer *Container;
       GtkWidget *Computer;
       GtkWidget *StudioLabel;
       Computer = gtk_list_box_row_new();
       StudioLabel = gtk_label_new (location_data);
       Locations = gtk_box_new (ATK_STATE_VERTICAL, 1);
       Location = gtk_list_box_new ();
       gtk_container_add (GTK_CONTAINER (Computer), Locations);
       gtk_box_pack_start (GTK_BOX (Location), StudioLabel, TRUE, TRUE, 0);
       gtk_container_add (GTK_CONTAINER (Location), GTK_LIST_BOX (Computer));
       gtk_container_add (GTK_CONTAINER (Container), GTK_BOX (Locations));
       gtk_container_add (GTK_CONTAINER (Container), GTK_LIST_BOX (Location));
       gtk_widget_show_all (GTK_WIDGET (Container));
       return (GtkWidget *) Gingerblue;
}
