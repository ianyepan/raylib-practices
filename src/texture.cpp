#include "../../../raylib-cpp/include/raylib-cpp.hpp"
#include "raylib.h"

const raylib::Color BG_COLOR{3, 1, 146};
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

auto checkedImage = raylib::Image::GenChecked(SCREEN_WIDTH, SCREEN_HEIGHT, 10, 10, BG_COLOR, BG_COLOR.Fade(0.9f));
raylib::Texture2D backgroundTexture;

int main(void)
{
  raylib::Window window{SCREEN_WIDTH, SCREEN_HEIGHT, "Texture practice"};

  ::SetTargetFPS(60);

  // Has to be defined inside main, WHY?
  backgroundTexture.LoadFromImage(checkedImage);

  raylib::Texture2D zombie{"../images/zombie.png"};

  while (!window.ShouldClose())
  {
    ::BeginDrawing();

    ::ClearBackground(raylib::Color::RayWhite);

    ::DrawTexture(backgroundTexture, 0, 0, WHITE);

    zombie.Draw(raylib::Vector2{0, 0}, raylib::Color::RayWhite);

    ::EndDrawing();
  }

  return 0;
}
