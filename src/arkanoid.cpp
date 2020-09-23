#include "../../../raylib-cpp/include/raylib-cpp.hpp"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int PLAYER_MAX_LIFE = 5;
const int BRICK_ROWS = 5;
const int BRICK_COLUMNS = 20;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 675;
const int PLAYER_SPEED = 15;
const int BALL_SPEED = 8;

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
  bool active;

  void Init(raylib::Vector2 position, raylib::Vector2 speed, int radius, bool active)
  {
    this->position = position;
    this->speed = speed;
    this->radius = radius;
    this->active = active;
  }
};

struct Brick
{
  raylib::Vector2 position;
  bool active;

  void Init(raylib::Vector2 position, bool active)
  {
    this->position = position;
    this->active = active;
  }
};

// Global Variables Declaration
static bool gameOver = false;
static bool pause = false;

static Player player;
static Ball ball;
static Brick brick[BRICK_ROWS][BRICK_COLUMNS];
static raylib::Vector2 brickSize;


// Module Functions Declaration (local)
static void InitGame(void);        // Initialize game
static void UpdateGame(void);      // Update game (one frame)
static void DrawGame(void);        // Draw game (one frame)
static void UpdateDrawFrame(void); // Update and Draw (one frame)

int main(void)
{
  raylib::Window window{SCREEN_WIDTH, SCREEN_HEIGHT, "Sample game: Arkanoid"};

  InitGame();

  SetTargetFPS(60);

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}

void InitGame(void)
{
  brickSize = raylib::Vector2{(float)GetScreenWidth() / BRICK_COLUMNS, 40};

  player.Init(raylib::Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8}, raylib::Vector2{SCREEN_WIDTH / 10, 20}, PLAYER_MAX_LIFE);

  ball.Init(raylib::Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8 - 30}, (raylib::Vector2){0, 0}, 10, false);

  // Initialize bricks
  int offset = 50;

  for (int i = 0; i < BRICK_ROWS; i++)
  {
    for (int j = 0; j < BRICK_COLUMNS; j++)
    {
      brick[i][j].Init(raylib::Vector2{j * brickSize.GetX() + brickSize.GetX() / 2, i * brickSize.y + offset}, true);
    }
  }
}

// Update game (one frame)
void UpdateGame(void)
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
      if (!ball.active)
      {
        if (IsKeyPressed(KEY_SPACE))
        {
          ball.active = true;
          ball.speed = raylib::Vector2{0, -BALL_SPEED}; // straight up
        }
      }

      // Ball movement logic
      if (ball.active)
      {
        // ball.position.x += ball.speed.x;
        // ball.position.y += ball.speed.y;
        ball.position.SetX(ball.position.GetX() + ball.speed.GetX());
        ball.position.SetY(ball.position.GetY() + ball.speed.GetY());
      }
      else
      {
        ball.position = (raylib::Vector2){player.position.GetX(), SCREEN_HEIGHT * 7 / 8 - 30}; // return to player position
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
        ball.active = false;

        player.life--;
      }

      // Collision logic: ball vs player
      if (CheckCollisionCircleRec(ball.position, ball.radius,
                                  (Rectangle){player.position.x - player.size.x / 2,
                                              player.position.y - player.size.y / 2, player.size.x, player.size.y}))
      {
        if (ball.speed.y > 0)
        {
          ball.speed.y *= -1;
          ball.speed.x = (ball.position.x - player.position.x) / (player.size.x / 2) * 5;
        }
      }

      // Collision logic: ball vs bricks
      for (int i = 0; i < BRICK_ROWS; i++)
      {
        for (int j = 0; j < BRICK_COLUMNS; j++)
        {
          if (brick[i][j].active)
          {
            // Hit below
            if (((ball.position.y - ball.radius) <= (brick[i][j].position.y + brickSize.y / 2)) &&
                ((ball.position.y - ball.radius) > (brick[i][j].position.y + brickSize.y / 2 + ball.speed.y)) &&
                ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) &&
                (ball.speed.y < 0))
            {
              brick[i][j].active = false;
              ball.speed.y *= -1;
            }
            // Hit above
            else if (((ball.position.y + ball.radius) >= (brick[i][j].position.y - brickSize.y / 2)) &&
                     ((ball.position.y + ball.radius) < (brick[i][j].position.y - brickSize.y / 2 + ball.speed.y)) &&
                     ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.y > 0))
            {
              brick[i][j].active = false;
              ball.speed.y *= -1;
            }
            // Hit left
            else if (((ball.position.x + ball.radius) >= (brick[i][j].position.x - brickSize.x / 2)) &&
                     ((ball.position.x + ball.radius) < (brick[i][j].position.x - brickSize.x / 2 + ball.speed.x)) &&
                     ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.x > 0))
            {
              brick[i][j].active = false;
              ball.speed.x *= -1;
            }
            // Hit right
            else if (((ball.position.x - ball.radius) <= (brick[i][j].position.x + brickSize.x / 2)) &&
                     ((ball.position.x - ball.radius) > (brick[i][j].position.x + brickSize.x / 2 + ball.speed.x)) &&
                     ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.x < 0))
            {
              brick[i][j].active = false;
              ball.speed.x *= -1;
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

        for (int i = 0; i < BRICK_ROWS; i++)
        {
          for (int j = 0; j < BRICK_COLUMNS; j++)
          {
            if (brick[i][j].active)
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
void DrawGame(void)
{
  ::BeginDrawing();

  ClearBackground(RAYWHITE);

  if (!gameOver)
  {
    // Draw player bar
    DrawRectangle(player.position.x - player.size.x / 2, player.position.y - player.size.y / 2, player.size.x,
                  player.size.y, BLACK);

    // Draw player lives
    for (int i = 0; i < player.life; i++)
      DrawRectangle(20 + 40 * i, SCREEN_HEIGHT - 30, 35, 10, LIGHTGRAY);

    // Draw ball
    DrawCircleV(ball.position, ball.radius, MAROON);

    // Draw bricks
    for (int i = 0; i < BRICK_ROWS; i++)
    {
      for (int j = 0; j < BRICK_COLUMNS; j++)
      {
        if (brick[i][j].active)
        {
          if ((i + j) % 2 == 0)
            DrawRectangle(brick[i][j].position.x - brickSize.x / 2, brick[i][j].position.y - brickSize.y / 2,
                          brickSize.x, brickSize.y, GRAY);
          else
            DrawRectangle(brick[i][j].position.x - brickSize.x / 2, brick[i][j].position.y - brickSize.y / 2,
                          brickSize.x, brickSize.y, DARKGRAY);
        }
      }
    }

    if (pause)
      ::DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, GRAY);
  }
  else
    ::DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
             GetScreenHeight() / 2 - 50, 20, GRAY);

  ::EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
  UpdateGame();
  DrawGame();
}
