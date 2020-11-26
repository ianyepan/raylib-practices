#include "../../../raylib-cpp/include/raylib-cpp.hpp"
#include "raylib.h"

#include <array>

namespace
{
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 400;
} // namespace

int main()
{
  raylib::Window window{SCREEN_WIDTH, SCREEN_HEIGHT, "Zombie Running"};

  ::SetTargetFPS(120);

  raylib::Texture2D zombie{"../assets/zombie.png"};
  // std::array<float, 11> frameRightBounds{0, 187, 399, 631, 880, 1118, 1424, 1766, 2069, 2327, 2612};
  std::array<float, 7> frameRightBounds{880, 1118, 1424, 1766, 2069, 2327, 2612};
  int frameAmount = (int)frameRightBounds.size() - 1;
  float frameTopBound = 0;
  float frameLowBound = zombie.GetHeight();
  int frameIndex = 0;

  float timer = 0.0f;

  while (!window.ShouldClose())
  {
    ::BeginDrawing();
    ::ClearBackground(::RAYWHITE);

    timer += ::GetFrameTime();
    if (timer >= 0.1f)
    {
      timer = 0.0f;
      ++frameIndex;
      frameIndex %= frameAmount;
    }

    // if (::IsMouseButtonPressed(::MOUSE_LEFT_BUTTON) || ::IsKeyPressed(::KEY_RIGHT) || ::IsKeyPressed(::KEY_ENTER) ||
    //     IsKeyPressed(::KEY_SPACE))
    // {
    //   ++frameIndex;
    //   frameIndex %= frameAmount;
    // }

    zombie.Draw(raylib::Rectangle{frameRightBounds[frameIndex], frameTopBound,
                                  frameRightBounds[frameIndex + 1] - frameRightBounds[frameIndex], frameLowBound},
                raylib::Vector2{10, 10}, ::RAYWHITE);

    ::EndDrawing();
  }

  return 0;
}
