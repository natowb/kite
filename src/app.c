#include "app.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define COLOR_PURPLE_ACCENT ((Color){0xC4, 0x4D, 0xFF, 0xFF})
#define COLOR_DARK_BACKGROUND ((Color){0x2d, 0x2d, 0x2d, 0xff})
#define COLOR_BUTTON_NORMAL RED
#define COLOR_BUTTON_HOVERED WHITE

typedef enum { SCREEN_HOME = 0, SCREEN_PREVIEW } Screens;

void center_camera(Camera2D *camera, Texture2D *texture) {

  int width = GetScreenWidth();
  int height = GetScreenHeight();

  camera->target.x = (float)texture->width / 2;
  camera->target.y = (float)texture->height / 2;

  camera->offset.x = (float)width / 2;
  camera->offset.y = (float)height / 2;

  float zoom_factor_x = (float)width / texture->width;
  float zoom_factor_y = (float)height / texture->height;

  camera->zoom = zoom_factor_x > zoom_factor_y ? zoom_factor_y : zoom_factor_x;
}

// WARNING: WE EXPECT path to be valid here.
// NOTE: fix this shit.
void load_image(app_ctx *ctx, const char *path) {

  ctx->screen_id = SCREEN_PREVIEW;
  loaded_texture_t ld_texture = {0};
  ld_texture.path = strdup(path);
  ld_texture.texture = LoadTexture(path);
  PUSH_DYNAMIC_ARRAY(&ctx->assets.textures, ld_texture);

  ctx->assets.index = ctx->assets.textures.size - 1;
  ctx->texture = &ctx->assets.textures.data[ctx->assets.index].texture;

  center_camera(&ctx->camera, ctx->texture);
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

void handle_dropped_files(app_ctx *ctx) {
  if (IsFileDropped()) {

    FilePathList files = LoadDroppedFiles();
    for (uint32_t i = 0; i < files.count; i++) {
      if (validate_image_path(files.paths[i])) {
        load_image(ctx, files.paths[i]);
      } else {
        // TODO: show popup message
      }
    }

    UnloadDroppedFiles(files);
  }
}

void app_reload(app_ctx *ctx) {
  ctx->reload_count++;
  printf("HOT!!: Reload Count: %d\n", ctx->reload_count);
}

void app_start(app_ctx *ctx) {

  ctx->assets.index = 0;
  INIT_DYNAMIC_ARRAY(&ctx->assets.textures, 2);

  printf("Application has Started\n");
  ctx->font = LoadFont("./resources/PeaberryMono.ttf");
  if (ctx->argc > 1) {
    const char *path = ctx->argv[1];
    if (validate_image_path(path)) {
      load_image(ctx, path);
    }
  }
}

void draw_texture_item(app_ctx *ctx, const char *text, Rectangle bounds, size_t index) {
  // Check if the mouse is hovering over the draw_button
  bool is_hovered = CheckCollisionPointRec(GetMousePosition(), bounds);

  if (is_hovered) {
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      ctx->texture = &ctx->assets.textures.data[index].texture;
      center_camera(&ctx->camera, ctx->texture);
    }
    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, RED);
  } else {
    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, GREEN);
  }

  // Start with an estimated font size based on the button height
  int font_size = bounds.height * 0.5;
  Vector2 text_size = MeasureTextEx(ctx->font, text, font_size, 0);

  // Adjust font size to fit text within button bounds, reducing if necessary
  while ((text_size.x > bounds.width || text_size.y > bounds.height) &&
         font_size > 1) {
    font_size--; // Decrease font size until it fits
    text_size = MeasureTextEx(ctx->font, text, font_size, 0);
  }

  // Calculate position to center the text within the button
  float text_x = bounds.x + (bounds.width - text_size.x) / 2;
  float text_y = bounds.y + (bounds.height - text_size.y) / 2;

  // Draw the text at the calculated position
  DrawTextEx(ctx->font, text, (Vector2){text_x, text_y}, font_size, 0, WHITE);
}

void draw_footer(app_ctx *ctx) {

  float width = (float)GetScreenWidth();
  float height = (float)GetScreenHeight();

  int footer_height = 50;
  int footer_width = width;
  Vector2 footer_position = {
      .x = 0,
      .y = height - footer_height,
  };

  Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), ctx->camera);
  int font_size = 16;
  const char *file_name_text =
      TextFormat("File: %s / Size: %d bytes / Mouse Position: (%.0f, %.0f)",
                 ctx->file_name, ctx->file_bytes_count, mouse.x, mouse.y);
  Vector2 text_size = MeasureTextEx(ctx->font, file_name_text, font_size, 0);
  DrawRectangle(footer_position.x, footer_position.y, footer_width,
                footer_height, COLOR_DARK_BACKGROUND);
  DrawTextEx(
      ctx->font, file_name_text,
      (Vector2){.x = 10,
                .y = height - ((float)footer_height / 2) - text_size.y / 2},
      font_size, 0, WHITE);
}

void app_update(app_ctx *ctx) {
  BeginDrawing();
  ClearBackground(COLOR_DARK_BACKGROUND);

  switch (ctx->screen_id) {
  case SCREEN_HOME: {
    handle_dropped_files(ctx);

    static const char *message = "Drag & Drop image for preview";
    const Vector2 message_size = MeasureTextEx(ctx->font, message, 48, 0);
    Vector2 position = {0};
    position.x = ((float)GetScreenWidth() / 2) - message_size.x / 2;
    position.y = ((float)GetScreenHeight() / 2) - message_size.y / 2;
    DrawTextEx(ctx->font, message, position, 48, 0, PURPLE);
    break;
  }
  case SCREEN_PREVIEW: {
    if (IsKeyReleased(KEY_SPACE) || IsWindowResized()) {
      center_camera(&ctx->camera, ctx->texture);
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {

      Vector2 mouseWorldPos =
          GetScreenToWorld2D(GetMousePosition(), ctx->camera);

      ctx->camera.offset = GetMousePosition();

      ctx->camera.target = mouseWorldPos;

      float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
      if (wheel < 0)
        scaleFactor = 1.0f / scaleFactor;
      ctx->camera.zoom = Clamp(ctx->camera.zoom * scaleFactor, 0.125f, 64.0f);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 delta = GetMouseDelta();
      delta = Vector2Scale(delta, -1.0f / ctx->camera.zoom);
      ctx->camera.target = Vector2Add(ctx->camera.target, delta);
    }

    handle_dropped_files(ctx);

    Rectangle source = {
        .x = 0,
        .y = 0,
        .width = ctx->texture->width,
        .height = ctx->texture->height,
    };

    Rectangle dest = {
        .x = 0,
        .y = 0,
        .width = ctx->texture->width,
        .height = ctx->texture->height,
    };

    BeginMode2D(ctx->camera);
    DrawTexturePro(*ctx->texture, source, dest, Vector2Zero(), 0, WHITE);
    EndMode2D();

    for (size_t i = 0; i < ctx->assets.textures.size; i++) {
      draw_texture_item(ctx, GetFileName(ctx->assets.textures.data[i].path),
                  (Rectangle){.x = 5, .y = 60 * i, .width = 200, .height = 50}, i);
    }

    draw_footer(ctx);
    break;
  }
  }

  EndDrawing();
}

void app_end(app_ctx *ctx) {
  printf("Application has Ended!\n");

  for (size_t i = 0; i < ctx->assets.textures.size; i++) {

    free(ctx->assets.textures.data[i].path);
    UnloadTexture(ctx->assets.textures.data[i].texture);
  }

  FREE_DYNAMIC_ARRAY(&ctx->assets.textures);
  UnloadFont(ctx->font);
}
