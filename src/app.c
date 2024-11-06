#include "app.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define COLOR_PURPLE_ACCENT ((Color){0xC4, 0x4D, 0xFF, 0xFF})

typedef enum { SCREEN_HOME = 0, SCREEN_PREVIEW } Screens;

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
void load_image(app_ctx *ctx, const char *path) {
  ctx->screen_id = SCREEN_PREVIEW;
  UnloadTexture(ctx->texture);
  ctx->texture = LoadTexture(path);

  printf("path => %s\n", path);
  printf("GetFileName() => %s\n", GetFileName(path));
  printf("GetFileLength() => %d\n", GetFileLength(path));



  // FIXME: make this a standard length and not a magic number 
  const char* file_name = GetFileName(path);
  size_t file_name_len = strlen(file_name);
  if (file_name_len > 256) {
      file_name_len = 256;
  }
  memcpy(&ctx->file_name, file_name, file_name_len);
  ctx->file_name[file_name_len] = '\0';
  ctx->file_bytes_count = GetFileLength(path);


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

    const char *valid_path = NULL;

    FilePathList files = LoadDroppedFiles();
    for (uint32_t i = 0; i < files.count; i++) {
      if (validate_image_path(files.paths[i])) {
        valid_path = files.paths[i];
        break;
      }
    }

    if (!valid_path) {
      printf("ERROR: No valid image provided\n");
      UnloadDroppedFiles(files);
    } else {
      ctx->screen_id = SCREEN_PREVIEW;
      load_image(ctx, valid_path);
      UnloadDroppedFiles(files);
    }
  }
}

void app_reload(app_ctx *ctx) {
  ctx->reload_count++;
  printf("HOT!!: Reload Count: %d\n", ctx->reload_count);
}

void app_start(app_ctx *ctx) {
  printf("Application has Started\n");
  ctx->font = LoadFont("./resources/PeaberryMono.ttf");
  if (ctx->argc > 1) {
    const char *path = ctx->argv[1];
    if (validate_image_path(path)) {
      load_image(ctx, path);
    }
  }
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
  const char* file_name_text = TextFormat("File: %s / Size: %d bytes / Mouse Position: (%.0f, %.0f)", ctx->file_name, ctx->file_bytes_count, mouse.x, mouse.y);
  Vector2 text_size = MeasureTextEx(ctx->font, file_name_text, font_size, 0);
  DrawRectangle(footer_position.x, footer_position.y, footer_width, footer_height, (Color){0x2d, 0x2d, 0x2d, 0xff});
  DrawTextEx(ctx->font, file_name_text, (Vector2){.x = 10, .y = height-((float)footer_height/2)-text_size.y/2}, font_size, 0, WHITE);
}

void app_update(app_ctx *ctx) {
  BeginDrawing();
  ClearBackground((Color){0x0D, 0x0B, 0x0B, 0xFF});

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
        .width = ctx->texture.width,
        .height = ctx->texture.height,
      };

  Rectangle dest = {
        .x = 0,
        .y = 0,
        .width = ctx->texture.width,
        .height = ctx->texture.height,
      };



      BeginMode2D(ctx->camera);
      DrawTexturePro(ctx->texture, source, dest, Vector2Zero(), 0, WHITE);
      EndMode2D();

      draw_footer(ctx);
      break;
    }
  }

  EndDrawing();
}

void app_end(app_ctx *ctx) {
  printf("Application has Ended!\n");

  UnloadFont(ctx->font);
}
