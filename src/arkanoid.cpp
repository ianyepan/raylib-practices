#include "../../../raylib-cpp/include/raylib-cpp.hpp"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cmath>
#include <vector>

const int PLAYER_MAX_LIFE = 5;
const int BRICK_ROWS = 5;
const int BRICK_COLUMNS = 20;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 675;
const int PLAYER_SPEED = 15 / 2;
const int BALL_SPEED = 8 / 2;
const raylib::Color BG_COLOR{11,11,11};
const raylib::Color PLAYER_COLOR{raylib::Color::LightGray};
const std::vector<raylib::Color> COLOR_VECTOR{raylib::Color::Green,
                                              raylib::Color::Blue,
                                              raylib::Color::Red,
                                              raylib::Color::Pink,
                                              raylib::Color::Gold};
const int NUM_OF_COLORS = (int)COLOR_VECTOR.size();

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

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}

void InitGame()
{
  brickSize = raylib::Vector2{(float)::GetScreenWidth() / BRICK_COLUMNS, 40};

  player.Init(
      raylib::Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8}, raylib::Vector2{SCREEN_WIDTH / 7, 20}, PLAYER_MAX_LIFE);

  ball.Init(raylib::Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8 - 30}, (raylib::Vector2){0, 0}, 10, false);

  // Initialize bricks
  int offset = 50;

  for (int i = 0; i < BRICK_ROWS; ++i)
  {
    for (int j = 0; j < BRICK_COLUMNS; ++j)
    {
      brick[i][j].Init(raylib::Vector2{j * brickSize.GetX() + brickSize.GetX() / 2, i * brickSize.y + offset}, true);
    }
  }
}

// Update game (one frame)
void UpdateGame()
{
  if (!gameOver)
  {
    if (IsKeyPressed('P'))
    {
      pause = !pause;
    }

    if (!pause)
    {
      // Player movement logic
      if (IsKeyDown(KEY_LEFT))
      {
        player.position.x -= PLAYER_SPEED;
      }
      if ((player.position.x - player.size.x / 2) <= 0)
      {
        player.position.x = player.size.x / 2;
      }
      if (IsKeyDown(KEY_RIGHT))
      {
        player.position.x += PLAYER_SPEED;
      }
      if ((player.position.x + player.size.x / 2) >= SCREEN_WIDTH)
      {
        player.position.x = SCREEN_WIDTH - player.size.x / 2;
      }

      // Ball launching logic
      if (!ball.shouldRender)
      {
        if (IsKeyPressed(KEY_SPACE))
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
        ball.speed.x *= -1;
      if ((ball.position.GetY() - ball.radius) <= 0)
      {
        ball.speed.y *= -1;
      }
      if ((ball.position.GetY() + ball.radius) >= SCREEN_HEIGHT) // drop below window
      {
        ball.speed = (raylib::Vector2){0, 0};
        ball.shouldRender = false;

        --player.life;
      }

      // Collision logic: ball vs player
      raylib::Rectangle playerRectangle{player.position.GetX() - player.size.GetX() / 2,
                                        player.position.GetY() - player.size.GetY() / 2,
                                        player.size.GetX(),
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
            }
            // Hit above
            else if (((ball.position.y + ball.radius) >= (brick[i][j].position.y - brickSize.y / 2)) &&
                     ((ball.position.y + ball.radius) < (brick[i][j].position.y - brickSize.y / 2 + ball.speed.y)) &&
                     ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.y > 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetY(ball.speed.GetY() * -1);
            }
            // Hit left
            else if (((ball.position.x + ball.radius) >= (brick[i][j].position.x - brickSize.x / 2)) &&
                     ((ball.position.x + ball.radius) < (brick[i][j].position.x - brickSize.x / 2 + ball.speed.x)) &&
                     ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.x > 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetX(ball.speed.GetX() * -1);
            }
            // Hit right
            else if (((ball.position.x - ball.radius) <= (brick[i][j].position.x + brickSize.x / 2)) &&
                     ((ball.position.x - ball.radius) > (brick[i][j].position.x + brickSize.x / 2 + ball.speed.x)) &&
                     ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.x < 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetX(ball.speed.GetX() * -1);
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
    if (IsKeyPressed(KEY_ENTER))
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

  ClearBackground(BG_COLOR);

  if (!gameOver)
  {
    // Draw player bar
    ::DrawRectangleGradientV(player.position.GetX() - player.size.GetX() / 2,
                             player.position.GetY() - player.size.GetY() / 2,
                             player.size.GetX(),
                             player.size.GetY(),
                             PLAYER_COLOR,
                             PLAYER_COLOR.Fade(0.5f));

    // Draw lives (health)
    for (int i = 0; i < player.life; ++i)
    {
      DrawRectangleGradientV(20 + 40 * i, SCREEN_HEIGHT - 30, 35, 10, raylib::Color::Green, raylib::Color::Lime);
    }

    // Draw ball
    // ball.position.DrawCircle(ball.radius, raylib::Color::Maroon);
    ::DrawCircleGradient(
        ball.position.GetX(), ball.position.GetY(), ball.radius, raylib::Color::White, raylib::Color::Red);

    // Draw bricks
    for (int i = 0; i < BRICK_ROWS; ++i)
    {
      for (int j = 0; j < BRICK_COLUMNS; ++j)
      {
        if (brick[i][j].shouldRender)
        {
          ::DrawRectangleGradientV(brick[i][j].position.GetX() - brickSize.GetX() / 2,
                                   brick[i][j].position.GetY() - brickSize.GetY() / 2,
                                   brickSize.GetX(),
                                   brickSize.GetY(),
                                   COLOR_VECTOR[(i + j) % (int)COLOR_VECTOR.size()],
                                   ::Fade(COLOR_VECTOR[(i + j) % (int)COLOR_VECTOR.size()], 0.5f));
          ::DrawRectangleLines(brick[i][j].position.GetX() - brickSize.GetX() / 2,
                               brick[i][j].position.GetY() - brickSize.GetY() / 2,
                               brickSize.GetX(),
                               brickSize.GetY(),
                               raylib::Color::LightGray);
        }
      }
    }

    if (pause)
      ::DrawText(
          "GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, GRAY);
  }
  else // Game over
  {
    ::DrawText("PRESS [ENTER] TO PLAY AGAIN",
               ::GetScreenWidth() / 2 - ::MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
               ::GetScreenHeight() / 2 - 50,
               20,
               raylib::Color::Gray);
  }

  ::EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame()
{
  UpdateGame();
  DrawGame();
}
