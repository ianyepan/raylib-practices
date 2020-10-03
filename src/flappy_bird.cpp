#include "../include/raylib-cpp.hpp"
#include "raylib.h"

#include <algorithm>
#include <vector>

const int MAX_TUBES = 100;
const int FLAPPY_RADIUS = 22;
const int TUBES_WIDTH = 50;
const int GRAVITY = 1;
const float JUMP_HEIGHT = 1.5f;

struct Flappy
{
  raylib::Vector2 position;
  int radius;
  raylib::Color color;

  Flappy(raylib::Vector2 p_position, int p_radius, raylib::Color p_color)
      : position{p_position}, radius{p_radius}, color{p_color}
  {
  }
};

struct Tubes
{
  raylib::Rectangle rec;
  raylib::Color color;
  bool active;
};

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 450;
static const int TUBE_SPEED = 1;

static bool isGameOver = false;
static bool isPaused = false;
static int score = 0;
static int hiScore = 0;

static Flappy flappy{raylib::Vector2{80, (float)(SCREEN_HEIGHT / 2 - FLAPPY_RADIUS)}, FLAPPY_RADIUS,
                     raylib::Color::DarkGray};
static std::vector<Tubes> tubes(MAX_TUBES * 2);
static std::vector<raylib::Vector2> tubesPos(MAX_TUBES);

raylib::Texture2D backgroundTexture;
raylib::Texture2D flappyTexture;
raylib::Texture2D tubeTexture;

static void InitGame();
static void UpdateGame();
static void DrawGame();
static void UpdateDrawFrame();

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

void InitGame()
{
  backgroundTexture = ::LoadTexture("../assets/flappy_background.png");
  flappyTexture = ::LoadTexture("../assets/flappy_bird.png");
  tubeTexture = ::LoadTexture("../assets/flappy_tube.png");

  score = 0;
  isGameOver = false;
  isPaused = false;

  // TODO: how?
  for (int i = 0; i < MAX_TUBES; ++i)
  {
    tubesPos[i].SetX(400 + 280 * i);
    // tubesPos[i].SetY(-GetRandomValue(0, 120));
    tubesPos[i].SetY(0);
  }

  // TODO: how?
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

// Update game (one frame)
void UpdateGame()
{
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

      if (::IsKeyDown(::KEY_SPACE) && !isGameOver)
      {
        flappy.position.y -= JUMP_HEIGHT;
      }
      else
      {
        flappy.position.y += GRAVITY;
      }

      // Check Collisions
      for (int i = 0; i < MAX_TUBES * 2; i++)
      {
        if (CheckCollisionCircleRec(flappy.position, flappy.radius, tubes[i].rec))
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

// Draw game (one frame)
void DrawGame()
{
  ::BeginDrawing();
  ::ClearBackground(raylib::Color::RayWhite);

  if (!isGameOver)
  {
    backgroundTexture.Draw(raylib::Vector2{0, 0}, 0.0f, 0.8f, raylib::Color::White);

    // flappy.position.DrawCircle(flappy.radius, flappy.color);
    raylib::Vector2 adjustedFlappyPosition{flappy.position.GetX() - flappy.radius,
                                           flappy.position.GetY() - flappy.radius - 2};

    flappyTexture.Draw(adjustedFlappyPosition, 0.0f, 0.4f, raylib::Color::White);

    // Draw tubes
    for (int i = 0; i < MAX_TUBES; ++i)
    {
      // ::DrawRectangle(tubes[i * 2].rec.x, tubes[i * 2].rec.y, tubes[i * 2].rec.width, tubes[i * 2].rec.height,
      //                          raylib::Color::Maroon);
      // ::DrawRectangle(tubes[i * 2 + 1].rec.x, tubes[i * 2 + 1].rec.y, tubes[i * 2 + 1].rec.width,
      //                          tubes[i * 2 + 1].rec.height, raylib::Color::Maroon);
      // ::DrawRectangleLinesEx(
      //     raylib::Rectangle{tubes[i * 2].rec.x, tubes[i * 2].rec.y, tubes[i * 2].rec.width, tubes[i * 2].rec.height}, 2,
      //     raylib::Color::Black);
      // ::DrawRectangleLinesEx(raylib::Rectangle{tubes[i * 2 + 1].rec.x, tubes[i * 2 + 1].rec.y,
      //                                          tubes[i * 2 + 1].rec.width, tubes[i * 2 + 1].rec.height},
      //                        2, raylib::Color::Black);

      tubeTexture.Draw(raylib::Vector2{tubes[i * 2].rec.x + TUBES_WIDTH, tubes[i * 2].rec.y + tubes[i * 2].rec.height},
                       180.0f, 0.7f, raylib::Color::White);
      tubeTexture.Draw(raylib::Vector2{tubes[i * 2 + 1].rec.x, tubes[i * 2 + 1].rec.y}, 0.0f, 0.7f,
                       raylib::Color::White);
    }

    ::DrawText(TextFormat("%04i", score), 20, 20, 40, raylib::Color::White);
    ::DrawText(TextFormat("HI-SCORE: %04i", hiScore), 20, 70, 20, raylib::Color::White);

    if (isPaused)
    {
      ::DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
                 raylib::Color::White);
    }
  }
  else
  {
    backgroundTexture.Draw(raylib::Vector2{0, 0}, 0.0f, 0.8f, raylib::Color::Gray);

    ::DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 25) / 2,
               GetScreenHeight() / 2 - 50, 25, raylib::Color::White);
  }

  ::EndDrawing();
}

void UpdateDrawFrame()
{
  UpdateGame();
  DrawGame();
}