#include "../../../raylib-cpp/include/raylib-cpp.hpp"

#include <vector>

// Compile command:  g++ main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c++17

const int MAX_COLUMNS = 20;
const int WIDTH = 1600;
const int HEIGHT = 900;

void DrawFloorAndWalls()
{
  raylib::Vector3 planePosition{0.0f, 0.0f, 0.0f};
  planePosition.DrawPlane(raylib::Vector2{32.0f, 32.0f}, raylib::Color{93, 63, 39});

  raylib::Vector3 wallPosition1{-16.0f, 2.5f, 0.0f};
  raylib::Vector3 wallPosition2{16.0f, 2.5f, 0.0f};
  raylib::Vector3 wallPosition3{0.0f, 2.5f, 16.0f};
  raylib::Vector3 wallPosition4{0.0f, 2.5f, -16.0f};

  wallPosition1.DrawCube(raylib::Vector3{1.0f, 5.0f, 32.0f}, raylib::Color{17, 36, 14});
  wallPosition2.DrawCube(raylib::Vector3{1.0f, 5.0f, 32.0f}, raylib::Color{80, 80, 80});
  wallPosition3.DrawCube(raylib::Vector3{32.0f, 5.0f, 1.0f}, raylib::Color{10, 20, 30});
  wallPosition4.DrawCube(raylib::Vector3{32.0f, 5.0f, 1.0f}, raylib::Color{30, 20, 10});
}

void drawHUD()
{
  raylib::Vector2 hudPosition{10, 10};
  raylib::Vector2 hudSize{440, 140};
  hudPosition.DrawRectangle(hudSize, raylib::Color::SkyBlue.Fade(0.5f));
  ::DrawRectangleLines(hudPosition.GetX(), hudPosition.GetY(), hudSize.GetX(), hudSize.GetY(), raylib::Color::Blue);

  int fontSize = 20;
  ::DrawText("First person camera default controls:", 20, hudSize.GetY() * 1 / 6, fontSize, raylib::Color::White);
  ::DrawText("- Move with keys: W, A, S, D", 40, hudSize.GetY() * 3 / 6, fontSize, raylib::Color::White);
  ::DrawText("- Mouse move to look around", 40, hudSize.GetY() * 5 / 6, fontSize, raylib::Color::White);
}

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
    heights[i] = (float)::GetRandomValue(1, 3); // int to float
    positions[i] = raylib::Vector3(GetRandomValue(-15, 15), heights[i] / 2, GetRandomValue(-15, 15));
    colors[i] = raylib::Color(GetRandomValue(30, 50), GetRandomValue(15, 25), GetRandomValue(5, 25));
  }

  camera.SetMode(CAMERA_FIRST_PERSON);

  ::SetTargetFPS(120);

  while (!window.ShouldClose())
  {
    camera.Update();

    ::BeginDrawing();

    background.ClearBackground();

    camera.BeginMode3D();

    DrawFloorAndWalls();

    // Draw some cubes around
    for (int i = 0; i < MAX_COLUMNS; ++i)
    {
      raylib::Vector3 sizeDimensions{2.0f, heights[i], 2.0f};
      positions[i].DrawCube(sizeDimensions, colors[i]);
      positions[i].DrawCubeWires(sizeDimensions, raylib::Color::LightGray);
    }

    camera.EndMode3D();

    drawHUD();

    ::EndDrawing();
  }

  return 0;
}
