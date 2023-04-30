/* $Id$

   Copyright (C) 2023  Aamot Broadcast
   Author(s): Ole Aamot <ole@gnome.org>
   License: GNU GPL version 3
   Version: 0.0.1 (2023-04-30)
   Website: http://www.gingerblue.org/
   
*/

#include <gst/gst.h>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "garagejam.h"

GingerblueData *
gb_file_parse_volume (GingerblueData *data, xmlDocPtr doc, xmlNodePtr cur) {
        GingerblueData *gbdata = (GingerblueData *)data;
        xmlNodePtr sub;
	gbdata->version = (gchar *)xmlGetProp (cur, (const xmlChar *)"version");
	gbdata->volume = (gchar *)xmlGetProp (cur, (const xmlChar *)"volume");
	sub = cur->xmlChildrenNode;
	while (sub != NULL) {
		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "line"))) {
			gbdata->line = (gchar *) xmlNodeListGetString(doc, sub->xmlChildrenNode, 1);
		}
		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "musician"))) {
			gbdata->musician = (gchar *) xmlNodeListGetString(doc, sub->xmlChildrenNode, 1);
		}
		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "musical_instrument"))) {
			gbdata->musical_instrument = (gchar *) xmlNodeListGetString(doc, sub->xmlChildrenNode, 1);
		}
		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "volume"))) {
			gbdata->volume = (gchar *) xmlNodeListGetString(doc, sub->xmlChildrenNode, 1);
		}
		sub = sub->next;
	}
	return (GingerblueData *)gbdata;
}

GingerblueData *
gb_file_config_load (GingerblueData *head, gchar *filename) {
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	GingerblueData *curr = NULL;
	gchar *version;
	g_print ("%s\n", filename);
	g_return_val_if_fail (filename != NULL, NULL);
	doc = xmlReadFile (filename, NULL, 0);
	if (doc == NULL) {
		perror("xmlParseFile");
		xmlFreeDoc (doc);
		return NULL;
	}
	cur = xmlDocGetRootElement (doc);
	if (cur == NULL) {
	        fprintf (stderr, _("Empty document\n"));
		xmlFreeDoc (doc);
		return NULL;
	}
	if (xmlStrcmp(cur->name, (const xmlChar *) "garagejam")) {
	        fprintf(stderr, _("Document of wrong type, root node != garagejam\n"));
		xmlFreeDoc (doc);
		return NULL;
	}
	version = (gchar *) xmlGetProp (cur, (const xmlChar *)"version");
	g_print (_("Valid GNOME Gingerblue %s XML document %s\n"), version, filename);
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
	  g_print (_("Parsing GNOME Gingerblue %s XML document %s\n"), version, filename);
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "line"))) {
		  g_print (_("Found Line\n"));
			curr = g_new0(GingerblueData, 1);
			curr->line = (gchar *) xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			g_print ("%s\n", curr->line);
			// curr = gb_file_parse_volume (curr, doc, cur);
			curr->next = head;
			head = curr;
			/* mem_volume = head */
			/* volumes = g_list_append (garagejam_volumes, (GingerblueData *)mem_volume); */
			g_print ("Done with parsing Line\n");
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "musician"))) {
			g_print (_("Found Musician\n"));
			curr = g_new0(GingerblueData, 1);
			curr->musician = (gchar *) xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			g_print ("%s\n", curr->musician);
			// curr = gb_file_parse_volume (curr, doc, cur);
			curr->next = head;
			head = curr;
			/* mem_volume = head */
			/* volumes = g_list_append (garagejam_volumes, (GingerblueData *)mem_volume); */
			g_print (_("Done with parsing Musician\n"));
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "musical_instrument"))) {
			g_print (_("Found Musical Instrument\n"));
			curr = g_new0(GingerblueData, 1);
			curr->musical_instrument = (gchar *) xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			g_print ("%s\n", curr->musical_instrument);
			// curr = gb_file_parse_volume (curr, doc, cur);
			curr->next = head;
			head = curr;
			/* mem_volume = head */
			/* volumes = g_list_append (garagejam_volumes, (GingerblueData *)mem_volume); */
			g_print (_("Done with parsing Musical Instrument\n"));
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "volume"))) {
			g_print (_("Found Volume\n"));
			curr = g_new0(GingerblueData, 1);
			curr->volume = (gchar *) xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			g_print ("%s\n", curr->volume);
			// curr = gb_file_parse_volume (curr, doc, cur);
			curr->next = head;
			head = curr;
			/* mem_volume = head */
			/* volumes = g_list_append (garagejam_volumes, (GingerblueData *)mem_volume); */
			g_print (_("Done with parsing Volume\n"));
		}
		cur = cur->next;
	}
	g_print (_("Finished parsing XML document\n"));
	xmlFreeDoc (doc);
	return curr;
}

/* int main (int argc, char **argv) */
/* { */
/* 	GingerblueData *data = NULL; */
/* 	data = gb_file_config_load (data, "garagejam.xml"); */
/* 	free (data); */
/* 	return (0); */
/* } */
