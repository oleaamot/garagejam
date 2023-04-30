#ifndef GINGERBLUE_STUDIO_LOCATION_H
#define GINGERBLUE_STUDIO_LOCATION_H 1

#include <geoclue.h>
#include <champlain/champlain.h>

static gchar *print_location (GClueSimple *simple, ChamplainView *view);
static gboolean on_location_timeout (gpointer user_data);
static void on_client_active_notify (GClueClient *client, GParamSpec *pspec, gpointer user_data);
static void on_simple_ready (GObject *source_object, GAsyncResult *res, gpointer user_data);
int studio_location_navigate (ChamplainView *view, gpointer *data);

#endif /* GINGERBLUE_STUDIO_LOCATION_H */
