#include "app.h"
#include <dlfcn.h>
#include <raylib.h>

#ifdef HOTRELOAD

#include <stdio.h>
const char *lib_file_name = "libapp.so";
void *lib = NULL;

app_reload_t app_reload = NULL;
app_start_t app_start = NULL;
app_update_t app_update = NULL;
app_end_t app_end = NULL;

#endif

app_ctx ctx = {0};

bool reload_app() {
#ifdef HOTRELOAD
  if (lib != NULL) {
    dlclose(lib);
  }

  lib = dlopen(lib_file_name, RTLD_NOW);

  if (lib == NULL) {
    fprintf(stderr, "Error: could not load %s: %s", lib_file_name, dlerror());
    return false;
  }

  app_reload = dlsym(lib, "app_reload");
  app_start = dlsym(lib, "app_start");
  app_update = dlsym(lib, "app_update");
  app_end = dlsym(lib, "app_end");
  if (!app_reload) {
    fprintf(stderr, "Error: could not load %s: %s", "app_reload", dlerror());
    return false;
  }
  if (!app_start) {
    fprintf(stderr, "Error: could not load %s: %s", "app_start", dlerror());
    return false;
  }

  if (!app_update) {
    fprintf(stderr, "Error: could not load %s: %s", "app_update", dlerror());
    return false;
  }
  if (!app_end) {
    fprintf(stderr, "Error: could not load %s: %s", "app_end", dlerror());
    return false;
  }

#endif
  return true;
}

int main(void) {

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
  const int width = 1280;
  const int height = 720;

  InitWindow(width, height, "Kite");

#ifdef HOTRELOAD
  if (!reload_app()) {
    CloseWindow();
    return 1;
  }
#endif

  app_start(&ctx);

  while (!WindowShouldClose()) {

#ifdef HOTRELOAD
    if (IsKeyPressed(KEY_H)) {
      if (!reload_app())
        return 1;

      app_reload(&ctx);
    }
#endif

    app_update(&ctx);
  }

  app_end(&ctx);
  CloseWindow();

  return 0;
}
