#include "../../../raylib-cpp/include/raylib-cpp.hpp"
#include "raylib.h"

// Compile command:  g++ basic_window.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c++17

const int WIDTH = 800;
const int HEIGHT = 450;
int main()
{
  raylib::Window window(WIDTH, HEIGHT, "Raylib-CPP basic window");

  SetTargetFPS(60);

  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    raylib::Color::RayWhite.ClearBackground();

    DrawText("Congrats! You created your first window!", 100, 200, 30, LIGHTGRAY);

    EndDrawing();
  }

  return 0;
}
