#include "../../../raylib-cpp/include/raylib-cpp.hpp"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cmath>
#include <vector>

// Compile command: g++ arkanoid.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c++17

const int PLAYER_MAX_LIFE = 5;
const int BRICK_ROWS = 5;
const int BRICK_COLUMNS = 15;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 675;
const int PLAYER_SPEED = 8;
const int BALL_SIZE = 12;
const int BALL_SPEED = 4;
const raylib::Color BG_COLOR{3, 1, 146};
const std::vector<raylib::Color> COLOR_VECTOR{raylib::Color::Green, raylib::Color::Blue, raylib::Color::Red,
                                              raylib::Color::Pink, raylib::Color::Gold};
const std::vector<raylib::Color> TINT_VECTOR{raylib::Color{150, 255, 150}, raylib::Color{150, 150, 255},
                                             raylib::Color{255, 150, 150}, raylib::Color{255, 100, 255},
                                             raylib::Color{255, 255, 100}};
const int NUM_OF_COLORS = (int)COLOR_VECTOR.size();

auto checkedImage = raylib::Image::GenChecked(SCREEN_WIDTH, SCREEN_HEIGHT, 10, 10, BG_COLOR, BG_COLOR.Fade(0.9f));
raylib::Texture2D backgroundTexture;

struct Player
{
  raylib::Vector2 position;
  raylib::Vector2 size;
  int life;

  void Init(raylib::Vector2 position, raylib::Vector2 size, int life)
  {
    this->position = position;
    this->size = size;
    this->life = life;
  }
};

struct Ball
{
  raylib::Vector2 position;
  raylib::Vector2 speed;
  int radius;
  bool shouldRender;

  void Init(raylib::Vector2 position, raylib::Vector2 speed, int radius, bool shouldRender)
  {
    this->position = position;
    this->speed = speed;
    this->radius = radius;
    this->shouldRender = shouldRender;
  }
};

struct Brick
{
  raylib::Vector2 position;
  bool shouldRender;

  void Init(raylib::Vector2 position, bool shouldRender)
  {
    this->position = position;
    this->shouldRender = shouldRender;
  }
};

// Global Variables Declaration
static bool gameOver = false;
static bool pause = false;
static Player player;
static Ball ball;
static Brick brick[BRICK_ROWS][BRICK_COLUMNS];
static raylib::Vector2 brickSize;
static raylib::Color ballColorState = raylib::Color::LightGray;

// Function prototypes
static void InitGame();        // Initialize game
static void UpdateGame();      // Update game (one frame)
static void DrawGame();        // Draw game (one frame)
static void UpdateDrawFrame(); // Update and Draw (one frame)

int main()
{
  raylib::Window window{SCREEN_WIDTH, SCREEN_HEIGHT, "Sample game: Arkanoid"};

  InitGame();

  SetTargetFPS(120);

  backgroundTexture.LoadFromImage(checkedImage);

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}

void InitGame()
{
  brickSize = raylib::Vector2{(float)::GetScreenWidth() / BRICK_COLUMNS, 40};

  player.Init(raylib::Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8}, raylib::Vector2{SCREEN_WIDTH / 7, 20},
              PLAYER_MAX_LIFE);

  ball.Init(raylib::Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8 - 30}, (raylib::Vector2){0, 0}, BALL_SIZE, false);

  // Initialize bricks
  int marginTop = brickSize.GetY() * 2;

  for (int i = 0; i < BRICK_ROWS; ++i)
  {
    for (int j = 0; j < BRICK_COLUMNS; ++j)
    {
      brick[i][j].Init(raylib::Vector2{j * brickSize.GetX() + brickSize.GetX() / 2, i * brickSize.GetY() + marginTop},
                       true);
    }
  }
}

// Update game variables (one frame)
void UpdateGame()
{
  if (!gameOver)
  {
    if (::IsKeyPressed('P'))
    {
      pause = !pause;
    }

    if (!pause)
    {
      // Player movement logic
      if (::IsKeyDown(::KEY_LEFT))
      {
        player.position.SetX(player.position.GetX() - PLAYER_SPEED);
      }
      if ((player.position.x - player.size.x / 2) <= 0)
      {
        player.position.SetX(player.size.GetX() / 2);
      }
      if (::IsKeyDown(::KEY_RIGHT))
      {
        player.position.SetX(player.position.GetX() + PLAYER_SPEED);
      }
      if ((player.position.x + player.size.x / 2) >= SCREEN_WIDTH)
      {
        player.position.SetX(SCREEN_WIDTH - player.size.GetX() / 2);
      }

      // Ball launching logic
      if (!ball.shouldRender)
      {
        if (::IsKeyPressed(::KEY_SPACE))
        {
          ball.shouldRender = true;
          ball.speed = raylib::Vector2{0, -BALL_SPEED}; // straight up
        }
      }

      // Ball movement logic
      if (ball.shouldRender)
      {
        ball.position.SetX(ball.position.GetX() + ball.speed.GetX());
        ball.position.SetY(ball.position.GetY() + ball.speed.GetY());
      }
      else
      {
        ball.position =
            (raylib::Vector2){player.position.GetX(), SCREEN_HEIGHT * 7 / 8 - 30}; // return to player position
      }

      // Collision logic: ball vs window walls
      if (((ball.position.GetX() + ball.radius) >= SCREEN_WIDTH) || ((ball.position.GetX() - ball.radius) <= 0))
      {
        ball.speed.SetX(ball.speed.GetX() * -1);
      }
      if ((ball.position.GetY() - ball.radius) <= 0)
      {
        ball.speed.SetY(ball.speed.GetY() * -1);
      }
      if ((ball.position.GetY() + ball.radius) >= SCREEN_HEIGHT) // drop below window
      {
        ball.speed = (raylib::Vector2){0, 0};
        ball.shouldRender = false;

        --player.life;
      }

      // Collision logic: ball vs player
      raylib::Rectangle playerRectangle{player.position.GetX() - player.size.GetX() / 2,
                                        player.position.GetY() - player.size.GetY() / 2, player.size.GetX(),
                                        player.size.GetY()};
      if (::CheckCollisionCircleRec(ball.position, ball.radius, playerRectangle))
      {
        if (ball.speed.GetY() > 0)
        {
          ball.speed.SetY(ball.speed.GetY() * -1);

          // reflect with an angle
          ball.speed.SetX((ball.position.GetX() - player.position.GetX()) / (player.size.GetX() / 2) * 5);
        }
      }

      // Collision logic: ball vs bricks
      for (int i = 0; i < BRICK_ROWS; ++i)
      {
        for (int j = 0; j < BRICK_COLUMNS; ++j)
        {
          if (brick[i][j].shouldRender)
          {
            // Hit below
            if (((ball.position.y - ball.radius) <= (brick[i][j].position.y + brickSize.y / 2)) &&
                ((ball.position.y - ball.radius) > (brick[i][j].position.y + brickSize.y / 2 + ball.speed.y)) &&
                ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) &&
                (ball.speed.y < 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetY(ball.speed.GetY() * -1);
              ballColorState = COLOR_VECTOR[(i + j) % (int)COLOR_VECTOR.size()];
            }
            // Hit above
            else if (((ball.position.y + ball.radius) >= (brick[i][j].position.y - brickSize.y / 2)) &&
                     ((ball.position.y + ball.radius) < (brick[i][j].position.y - brickSize.y / 2 + ball.speed.y)) &&
                     ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.y > 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetY(ball.speed.GetY() * -1);
              ballColorState = COLOR_VECTOR[(i + j) % (int)COLOR_VECTOR.size()];
            }
            // Hit left
            else if (((ball.position.x + ball.radius) >= (brick[i][j].position.x - brickSize.x / 2)) &&
                     ((ball.position.x + ball.radius) < (brick[i][j].position.x - brickSize.x / 2 + ball.speed.x)) &&
                     ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.x > 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetX(ball.speed.GetX() * -1);
              ballColorState = COLOR_VECTOR[(i + j) % (int)COLOR_VECTOR.size()];
            }
            // Hit right
            else if (((ball.position.x - ball.radius) <= (brick[i][j].position.x + brickSize.x / 2)) &&
                     ((ball.position.x - ball.radius) > (brick[i][j].position.x + brickSize.x / 2 + ball.speed.x)) &&
                     ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.x < 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetX(ball.speed.GetX() * -1);
              ballColorState = COLOR_VECTOR[(i + j) % (int)COLOR_VECTOR.size()];
            }
          }
        }
      }

      // Game over logic
      if (player.life <= 0)
        gameOver = true;
      else
      {
        gameOver = true;

        for (int i = 0; i < BRICK_ROWS; ++i)
        {
          for (int j = 0; j < BRICK_COLUMNS; ++j)
          {
            if (brick[i][j].shouldRender)
              gameOver = false;
          }
        }
      }
    }
  }
  else
  {
    if (::IsKeyPressed(::KEY_ENTER))
    {
      InitGame();
      gameOver = false;
    }
  }
}

// Draw game (one frame)
void DrawGame()
{
  ::BeginDrawing();

  ::ClearBackground(raylib::Color::RayWhite);

  ::DrawTexture(backgroundTexture, 0, 0, raylib::Color::White);

  if (!gameOver)
  {
    // Draw lives (health)
    raylib::Vector2 hudPosition{20, SCREEN_HEIGHT - 70};
    raylib::Vector2 hudSize{150, 40};
    hudPosition.DrawRectangle(hudSize, raylib::Color::Green.Fade(0.5f));
    ::DrawRectangleLines(hudPosition.GetX(), hudPosition.GetY(), hudSize.GetX(), hudSize.GetY(),
                         raylib::Color::RayWhite);
    ::DrawText("Lives Left:", hudPosition.GetX() + 10, hudPosition.GetY() + 10, 20, raylib::Color::Green);

    for (int i = 0; i < player.life; ++i)
    {
      DrawRectangleGradientV(20 + 40 * i, SCREEN_HEIGHT - 20, 35, 10, raylib::Color::Green, raylib::Color::Lime);
    }

    // Draw player bar
    ::DrawRectangleGradientV(player.position.GetX() - player.size.GetX() / 2,
                             player.position.GetY() - player.size.GetY() / 2, player.size.GetX(), player.size.GetY(),
                             raylib::Color::LightGray, raylib::Color::DarkGray);

    // Draw ball
    ::DrawCircleGradient(ball.position.GetX(), ball.position.GetY(), ball.radius, raylib::Color::White, ballColorState);

    // Draw bricks
    for (int i = 0; i < BRICK_ROWS; ++i)
    {
      for (int j = 0; j < BRICK_COLUMNS; ++j)
      {
        if (brick[i][j].shouldRender)
        {
          ::DrawRectangleGradientH(brick[i][j].position.GetX() - brickSize.GetX() / 2,
                                   brick[i][j].position.GetY() - brickSize.GetY() / 2, brickSize.GetX(),
                                   brickSize.GetY(), COLOR_VECTOR[(i + j) % (int)COLOR_VECTOR.size()],
                                   TINT_VECTOR[(i + j) % (int)TINT_VECTOR.size()]);
          ::DrawRectangleLinesEx(raylib::Rectangle{brick[i][j].position.GetX() - brickSize.GetX() / 2,
                                                   brick[i][j].position.GetY() - brickSize.GetY() / 2, brickSize.GetX(),
                                                   brickSize.GetY()},
                                 3, raylib::Color::Black);
        }
      }
    }

    if (pause)
      ::DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
                 GRAY);
  }
  else // Game over
  {
    ::DrawText("PRESS [ENTER] TO PLAY AGAIN",
               ::GetScreenWidth() / 2 - ::MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
               ::GetScreenHeight() / 2 - 50, 20, raylib::Color::Gray);
  }

  ::EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame()
{
  UpdateGame();
  DrawGame();
}
