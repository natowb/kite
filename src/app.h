#ifndef APP_H
#define APP_H

#include <raylib.h>
typedef struct {
  int reload_count;
  Texture2D texture;
  char file_name[256];
  int file_bytes_count;
  Font font;
  Camera2D camera;
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
