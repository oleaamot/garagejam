/* $Id$
   
   Copyright (C) 2020-2022 Aamot Software
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 6.2.0 (2022-07-09)
   Website: http://www.garagejam.org/
   
*/

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

GingerblueData *gb_file_config_load (GingerblueData *head, gchar *filename);

static void gb_file_parse_volume (GingerblueData *data, xmlDocPtr doc, xmlNodePtr cur);

