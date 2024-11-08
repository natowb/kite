#ifndef APP_H
#define APP_H
#include "utils.h"
#include <stdint.h>
#include <raylib.h>

#define STRING_MAX_LENGTH 256 


typedef struct {
  char* path;
  Texture2D texture;
} loaded_texture_t;

DEFINE_DYNAMIC_ARRAY(loaded_texture_t)

typedef struct {
  da_loaded_texture_t textures;
  uint32_t index;
} Assets;



typedef struct {
  int reload_count;
  char file_name[256];
  int file_bytes_count;
  Font font;
  Camera2D camera;
  Assets assets;
  Texture2D *texture;
  int screen_id;
  int argc;
  char **argv;
} app_ctx;

#ifdef HOTRELOAD
typedef void (*app_reload_t)(app_ctx *);
typedef void (*app_start_t)(app_ctx *);
typedef void (*app_update_t)(app_ctx *);
typedef void (*app_end_t)(app_ctx *);

#else
// NOTE: app_reload is only for use in development while using hot reload.
void app_reload(app_ctx *ctx);
void app_start(app_ctx *ctx);
void app_update(app_ctx *ctx);
void app_end(app_ctx *ctx);
#endif

#endif // APP_H
