#include "../../../raylib-cpp/include/raylib-cpp.hpp"
#include "raylib.h"

#define NUM_TEXTURES 7 // Currently we have 7 generation algorithms
const raylib::Color BG_COLOR{3, 1, 146};
const int screenWidth = 800;
const int screenHeight = 450;

auto checkedImage = raylib::Image::GenChecked(screenWidth, screenHeight, 10, 10, BG_COLOR, BG_COLOR.Fade(0.9f));
raylib::Texture2D backgroundTexture;

int main(void)
{
  InitWindow(screenWidth, screenHeight, "raylib [textures] example - procedural images generation");

  SetTargetFPS(60);

  // Has to be defined inside main, WHY?
  backgroundTexture.LoadFromImage(checkedImage);

  while (!WindowShouldClose())
  {
    BeginDrawing();

    ::ClearBackground(raylib::Color::RayWhite);

    ::DrawTexture(backgroundTexture, 0, 0, WHITE);

    EndDrawing();
  }

  return 0;
}
