#include "../../../raylib-cpp/include/raylib-cpp.hpp"

#include <vector>

// Compile command:  g++ main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c++17

#define MAX_COLUMNS 20
const int WIDTH = 1600;
const int HEIGHT = 900;

int main()
{
  raylib::Window window(WIDTH, HEIGHT, "Raylib-CPP First Person");
  raylib::Color background(RAYWHITE);

  raylib::Camera3D camera(raylib::Vector3(4.0f, 2.0f, 4.0f), raylib::Vector3(0.0f, 1.8f, 0.0f),
                          raylib::Vector3(0.0f, 1.0f, 0.0f), 60.0f, CAMERA_PERSPECTIVE);

  // Generates some random columns
  std::vector<float> heights(MAX_COLUMNS); // float heights[MAX_COLUMNS] = {0.0f};
  std::vector<raylib::Vector3> positions(MAX_COLUMNS);
  std::vector<raylib::Color> colors(MAX_COLUMNS);

  for (int i = 0; i < MAX_COLUMNS; ++i)
  {
    heights[i] = (float)GetRandomValue(1, 3); // int
    positions[i] = raylib::Vector3(GetRandomValue(-15, 15), heights[i] / 2, GetRandomValue(-15, 15));
    colors[i] = raylib::Color(GetRandomValue(30, 50), GetRandomValue(15, 25), GetRandomValue(5, 25));
  }

  camera.SetMode(CAMERA_FIRST_PERSON);

  SetTargetFPS(120);

  while (!window.ShouldClose())
  {
    camera.Update();

    BeginDrawing();

    background.ClearBackground();

    camera.BeginMode3D();

    // Draw ground (plane) and walls (cube)
    DrawPlane(raylib::Vector3{0.0f, 0.0f, 0.0f}, raylib::Vector2{32.0f, 32.0f}, raylib::Color{93, 63, 39});
    DrawCube(raylib::Vector3{-16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, raylib::Color{17, 36, 14});
    DrawCube(raylib::Vector3{16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f, raylib::Color{80, 80, 80});
    DrawCube(raylib::Vector3{0.0f, 2.5f, 16.0f}, 32.0f, 5.0f, 1.0f, raylib::Color{10, 20, 30});
    DrawCube(raylib::Vector3{0.0f, 2.5f, -16.0f}, 32.0f, 5.0f, 1.0f, raylib::Color{30, 20, 10});

    // Draw some cubes around
    for (int i = 0; i < MAX_COLUMNS; ++i)
    {
      positions[i].DrawCube(2.0f, heights[i], 2.0f, colors[i]);
      positions[i].DrawCubeWires(2.0f, heights[i], 2.0f, MAROON);
    }

    EndMode3D();

    DrawRectangle(10, 10, 220, 70, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(10, 10, 220, 70, BLUE);

    DrawText("First person camera default controls:", 20, 20, 10, BLACK);
    DrawText("- Move with keys: W, A, S, D", 40, 40, 10, DARKGRAY);
    DrawText("- Mouse move to look around", 40, 60, 10, DARKGRAY);

    EndDrawing();
  }

  return 0;
}
