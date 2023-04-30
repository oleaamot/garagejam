#define GINGERBLUE_STORAGE 2000

typedef struct _GingerblueStorage {
  gboolean storage_allocated;
  gboolean storagemap;
} GingerblueStorage;

void garagejam_storage_set(GingerblueStorage *storage, int storage_allocated, int storagemap);
