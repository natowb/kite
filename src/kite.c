
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define COLOR_PURPLE_ACCENT ((Color){0xC4, 0x4D, 0xFF, 0xFF})

static enum { SCREEN_HOME = 0, SCREEN_PREVIEW } Screens;
static Texture2D texture;
static Camera2D camera = {0};
static Font font = {0};
static bool has_texture = false;

void center_camera(Camera2D *camera, Texture2D texture) {

  int width = GetScreenWidth();
  int height = GetScreenHeight();

  camera->target.x = (float)texture.width / 2;
  camera->target.y = (float)texture.height / 2;

  camera->offset.x = (float)width / 2;
  camera->offset.y = (float)height / 2;

  float zoomFactorX = (float)width / texture.width;
  float zoomFactorY = (float)height / texture.height;

  camera->zoom = zoomFactorX > zoomFactorY ? zoomFactorY : zoomFactorX;
}

// WARNING: WE EXPECT path to be valid here.
// NOTE: fix this shit.
void load_image(const char *path) {
  Screens = SCREEN_PREVIEW;
  UnloadTexture(texture);
  texture = LoadTexture(path);
  center_camera(&camera, texture);
}

bool validate_image_path(const char *file_path) {

  const char *ext = GetFileExtension(file_path);
  printf("FILE: %s\n", file_path);
  printf("EXT: %s\n", ext);

  if (strcmp(".png", ext) == 0) {
    return true;
  }

  if (strcmp(".jpg", ext) == 0) {
    return true;
  }

  if (strcmp(".jpeg", ext) == 0) {
    return true;
  }

  return false;
}

void handle_dropped_files() {
  if (IsFileDropped()) {

    const char *valid_path = NULL;

    FilePathList files = LoadDroppedFiles();
    for (int i = 0; i < files.count; i++) {
      if (validate_image_path(files.paths[i])) {
        valid_path = files.paths[i];
        break;
      }
    }

    if (!valid_path) {
      printf("ERROR: No valid image provided\n");
      UnloadDroppedFiles(files);
    } else {
      Screens = SCREEN_PREVIEW;
      load_image(valid_path);
      UnloadDroppedFiles(files);
    }
  }
}

void screen_home() {
  handle_dropped_files();

  static const char *message = "Drag and Drop image to preview";
  const Vector2 message_size = MeasureTextEx(font, message, 48, 0);
  Vector2 position = {0};
  position.x = ((float)GetScreenWidth() / 2) - message_size.x / 2;
  position.y = ((float)GetScreenHeight() / 2) - message_size.y / 2;
  DrawTextEx(font, message, position, 48, 0, WHITE);
}

void screen_preview() {
  if (IsKeyReleased(KEY_SPACE) || IsWindowResized()) {
    center_camera(&camera, texture);
  }

  float wheel = GetMouseWheelMove();
  if (wheel != 0) {

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

    camera.offset = GetMousePosition();

    camera.target = mouseWorldPos;

    float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
    if (wheel < 0)
      scaleFactor = 1.0f / scaleFactor;
    camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f);
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f / camera.zoom);
    camera.target = Vector2Add(camera.target, delta);
  }
  handle_dropped_files();

  BeginMode2D(camera);
  DrawTexture(texture, 0, 0, WHITE);
  EndMode2D();
}

int main(int argc, char **argv) {

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
  const int width = 1280;
  const int height = 720;

  SetTraceLogLevel(LOG_ERROR);
  InitWindow(width, height, "Kite");
  font = LoadFont("./resources/PeaberryMono.ttf");

  if (argc > 1) {
    const char *path = argv[1];
    if (validate_image_path(path)) {
      load_image(path);
    }
  }
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground((Color){0x0D, 0x0B, 0x0B, 0xFF});

    switch (Screens) {
    case SCREEN_HOME: {
      screen_home();
      break;
    }
    case SCREEN_PREVIEW: {
      screen_preview();
      break;
    }
    }

    EndDrawing();
  }

  UnloadFont(font);
  CloseWindow();
  return 0;
}
