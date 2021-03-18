#include "../include/raylib-cpp.hpp"
#include "raylib.h"

#include <algorithm>
#include <array>
#include <iostream>

namespace
{
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

double Y_speed = 0.0;
const double GRAVITATIONAL_ACCELRATION = 0.07;
const double BOUNCE_FORCE = -1.2;
const int MAX_TUBES = 100;
const int FLAPPY_RADIUS = 18;
const int TUBES_WIDTH = 50;
const float JUMP_HEIGHT = 1.5f;
const int TUBE_SPEED = 1;

struct Flappy
{
  raylib::Vector2 position;
  int radius;
  raylib::Color color;

  Flappy(raylib::Vector2 _position, int _radius, raylib::Color _color)
      : position{_position}, radius{_radius}, color{_color}
  {
  }
};

struct Tubes
{
  raylib::Rectangle rec;
  raylib::Color color;
  bool active;
};

bool isGameOver = false;
bool isPaused = false;
int score = 0;
int hiScore = 0;

float alpha = 0.0f;
bool isOpaque = false;

Flappy flappy{raylib::Vector2{80, (float)(SCREEN_HEIGHT / 2 - FLAPPY_RADIUS)}, FLAPPY_RADIUS, ::DARKGRAY};
std::array<Tubes, MAX_TUBES * 2> tubes;
std::array<raylib::Vector2, MAX_TUBES> tubesPos;

raylib::Texture2D backgroundTexture;
raylib::Texture2D flappyTexture;
raylib::Texture2D tubeTexture;

void initFlappyPosition();
bool touchBorder(raylib::Vector2);
void InitGame();
void UpdateGame();
void tuneAlpha();
void announceGame();
void DrawGame();
void UpdateDrawFrame();

bool touchBorder(raylib::Vector2 center)
{
  if (center.x < 0 || center.y < 0 || center.x > SCREEN_WIDTH || center.y > SCREEN_HEIGHT)
  {
    return true;
  }
  return false;
}

void initFlappyPosition()
{
  flappy.position.x = 80;
  flappy.position.y = (float)(SCREEN_HEIGHT / 2 - FLAPPY_RADIUS);
}

void InitGame()
{
  backgroundTexture = ::LoadTexture("../assets/flappy_bg.png");
  flappyTexture = ::LoadTexture("../assets/flappy_bird.png");
  tubeTexture = ::LoadTexture("../assets/flappy_tube.png");

  initFlappyPosition();
  Y_speed = 0.0;
  score = 0;

  for (int i = 0; i < MAX_TUBES; ++i)
  {
    tubesPos[i].SetX(400 + 280 * i);
    tubesPos[i].SetY(-GetRandomValue(0, 120));
  }

  for (int i = 0; i < MAX_TUBES * 2; i += 2)
  {
    tubes[i].rec.SetX(tubesPos[i / 2].GetX());
    tubes[i].rec.SetY(tubesPos[i / 2].GetY());
    tubes[i].rec.SetWidth(TUBES_WIDTH);
    tubes[i].rec.SetHeight(GetRandomValue(220, 245));

    tubes[i + 1].rec.SetX(tubesPos[i / 2].GetX());
    tubes[i + 1].rec.SetY(600 + tubesPos[i / 2].y - 255);
    tubes[i + 1].rec.SetWidth(TUBES_WIDTH);
    tubes[i + 1].rec.SetHeight(tubes[i].rec.GetHeight());

    tubes[i / 2].active = true;
  }
}

// Update game variables for a frame
void UpdateGame()
{
  tuneAlpha();
  if (!isGameOver)
  {
    if (::IsKeyPressed(KEY_P))
    {
      isPaused = !isPaused;
    }

    if (!isPaused)
    {
      for (int i = 0; i < MAX_TUBES; ++i)
      {
        tubesPos[i].x -= TUBE_SPEED;
      }

      for (int i = 0; i < MAX_TUBES * 2; i += 2)
      {
        tubes[i].rec.x = tubesPos[i / 2].x;
        tubes[i + 1].rec.x = tubesPos[i / 2].x;
      }

      // Gravitational acceleration
      // Reference: https://scratch.mit.edu/projects/502812449/editor/
      flappy.position.y += Y_speed;
      Y_speed += GRAVITATIONAL_ACCELRATION;
      if (::IsKeyDown(::KEY_SPACE) && !isGameOver)
      {
        Y_speed = BOUNCE_FORCE;
      }

      // Check Collisions
      for (int i = 0; i < MAX_TUBES * 2; i++)
      {
        if (CheckCollisionCircleRec(flappy.position, flappy.radius, tubes[i].rec) || touchBorder(flappy.position))
        {
          isGameOver = true;
          isPaused = false;
        }
        else if ((tubesPos[i / 2].x < flappy.position.x) && tubes[i / 2].active && !isGameOver)
        {
          score += 100;
          tubes[i / 2].active = false;
          hiScore = std::max(hiScore, score);
        }
      }
    }
  }
  else if (::IsKeyPressed(::KEY_ENTER))
  {
    InitGame();
    isGameOver = false;
  }
}

void tuneAlpha()
{
  if (!isOpaque)
  {
    alpha += 0.01f;
    if (alpha >= 1.0f)
    {
      isOpaque = true;
    }
  }
  else
  {
    alpha -= 0.005f;
  }
}

void announceGame()
{
  const char *welcomeMessage = "Welcome To Flappy Bird!";
  ::DrawText(welcomeMessage, SCREEN_WIDTH / 2 - MeasureText(welcomeMessage, 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
             Fade(::WHITE, alpha));
}

// Draw game for one frame
void DrawGame()
{
  ::BeginDrawing();
  ::ClearBackground(::RAYWHITE);

  if (!isGameOver)
  {
    backgroundTexture.Draw(raylib::Vector2{0, 0}, 0.0f, 0.5f, ::WHITE);

    // flappy.position.DrawCircle(flappy.radius, flappy.color);
    raylib::Vector2 adjustedFlappyPosition{flappy.position.GetX() - flappy.radius - 2,
                                           flappy.position.GetY() - flappy.radius - 6};

    flappyTexture.Draw(adjustedFlappyPosition, 0.0f, 0.4f, ::WHITE);

    // Draw tubes
    for (int i = 0; i < MAX_TUBES; ++i)
    {
      // ::DrawRectangle(tubes[i * 2].rec.x, tubes[i * 2].rec.y, tubes[i * 2].rec.width, tubes[i * 2].rec.height,
      //                          ::MAROON);
      // ::DrawRectangle(tubes[i * 2 + 1].rec.x, tubes[i * 2 + 1].rec.y, tubes[i * 2 + 1].rec.width,
      //                          tubes[i * 2 + 1].rec.height, ::MAROON);
      // ::DrawRectangleLinesEx(
      //     raylib::Rectangle{tubes[i * 2].rec.x, tubes[i * 2].rec.y, tubes[i * 2].rec.width, tubes[i * 2].rec.height},
      //     2, ::BLACK);
      // ::DrawRectangleLinesEx(raylib::Rectangle{tubes[i * 2 + 1].rec.x, tubes[i * 2 + 1].rec.y,
      //                                          tubes[i * 2 + 1].rec.width, tubes[i * 2 + 1].rec.height},
      //                        2, ::BLACK);

      tubeTexture.Draw(raylib::Vector2{tubes[i * 2].rec.x + TUBES_WIDTH, tubes[i * 2].rec.y + tubes[i * 2].rec.height},
                       180.0f, 0.7f, ::PINK);
      tubeTexture.Draw(raylib::Vector2{tubes[i * 2 + 1].rec.x, tubes[i * 2 + 1].rec.y}, 0.0f, 0.7f, ::PINK);
    }

    ::DrawText(TextFormat("%04i", score), 20, 20, 40, ::WHITE);
    ::DrawText(TextFormat("HI-SCORE: %04i", hiScore), 20, 70, 20, ::WHITE);

    if (isPaused)
    {
      ::DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
                 ::WHITE);
    }
  }
  else
  {
    backgroundTexture.Draw(raylib::Vector2{0, 0}, 0.0f, 0.5f, ::GRAY);

    ::DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 25) / 2,
               GetScreenHeight() / 2 - 50, 25, ::WHITE);
  }

  announceGame();

  ::EndDrawing();
}

void UpdateDrawFrame()
{
  UpdateGame();
  DrawGame();
}

} // namespace

int main()
{
  raylib::Window window{SCREEN_WIDTH, SCREEN_HEIGHT, "Sample game: Flappy Bird"};
  InitGame();
  ::SetTargetFPS(120);

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}
