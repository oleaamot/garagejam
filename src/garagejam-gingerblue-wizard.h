#include "garagejam-gingerblue-storage.h"

#define GINGERBLUE_WIZARD 10000

void garagejam_wizard_new (GINGERBLUE_WIZARD);

typedef struct _GingerblueWizard {
  gboolean wizard_run;
  GtkWindow *window;
  GingerblueStorage *storage;
  GtkFile *metadata;
  char *stream;
} GingerblueWizard;
