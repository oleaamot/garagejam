#define GINGERBLUE_CONTAINER 4000

typedef struct _GingerblueContainer {
  gboolean container_active;
  (void *) load_container();
  (void *) save_container();
  (void *) show_container();
  (void *) hide_container();
  GtkWidget *widget;
} GingerblueContainer;
