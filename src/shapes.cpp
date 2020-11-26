#include "../../../raylib-cpp/include/raylib-cpp.hpp"
#include "raylib.h"

#include <iostream>
#include <ostream>

// Compile command:  g++ shapes.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c++17

const int WIDTH = 900;
const int HEIGHT = 600;

void DrawPulsingSquare(float posX, float posY, raylib::Color color)
{
  static float length = 80;
  static float sizeChange = 3.0f;

  if (length > 100)
  {
    sizeChange = -1.0f;
  }
  else if (length < 80)
  {
    sizeChange = 3.0f;
  }

  length += sizeChange;

  raylib::Rectangle rec{posX - length / 2, posY - length / 2, length, length};
  rec.DrawGradientH(raylib::Color{200, 200, 200}, raylib::Color{0, 100, 200});

  raylib::Rectangle recGradient{posX * 2 - length / 2, posY * 2 - length / 2, length, length};
  recGradient.DrawGradientV(raylib::Color{200, 200, 200}, raylib::Color{0, 100, 200});
}

void DrawRotateRectangle(float posX, float posY, raylib::Color color)
{
  static float recWidth = 60.0f;
  static float recHeight = 50.0f;
  static float rotationAngle = 0.0f;
  static float sizeChange = 3.0f;

  recWidth += sizeChange;
  recHeight += sizeChange;

  if (recWidth > 120 || recWidth < 60)
  {
    sizeChange *= -1.0f;
  }

  rotationAngle += 1.5f;

  raylib::Rectangle rec{posX, posY, recWidth, recHeight};
  rec.Draw(raylib::Vector2{recHeight / 10, recWidth / 3}, rotationAngle, color);
}

int main()
{
  raylib::Window window(WIDTH, HEIGHT, "Raylib Tutorial 2");

  SetTargetFPS(60);

  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    ::BeginDrawing();

    ::ClearBackground(::RAYWHITE);

    // use static so that updates at each loop is "remembered"
    static int xPosition = 200;
    static int step = -5;

    if (xPosition < 10 || xPosition > 600) // change direction at border
    {
      step *= -1;
    }

    xPosition += step;
    ::DrawText("Rip And Tear", xPosition, 20, 30, ::MAROON);

    DrawPulsingSquare(100, 200, ::BLUE);

    DrawRotateRectangle(500, 200, ::MAROON);

    ::EndDrawing();
  }

  return 0;
}
