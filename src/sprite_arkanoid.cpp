/*******************************************************************************************
*
*   raylib Arkanoid - Enhanced Graphics
*
*   Original sample game Marc Palau and Ramon Santamaria
*
*   This game has been created using raylib v3.0.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Original Copyright (c) 2015 Ramon Santamaria (@raysan5)
*   Modified and enhanced 2020 Ian Y.E. Pan (@ianpan870102)
*
********************************************************************************************/

#include "raylib.h"
#include "../include/raylib-cpp.hpp"

#include <cmath>
#include <vector>

// Compile command: g++ sprite_arkanoid.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c++17

const int PLAYER_MAX_LIFE = 5;
const int BRICK_ROWS = 5;
const int BRICK_COLUMNS = 11;
const int SCREEN_WIDTH = 970;
const int SCREEN_HEIGHT = 675;
const int PLAYER_SPEED = 8;
const int BALL_SIZE = 12;
const int BALL_SPEED = 5;
const raylib::Color BG_COLOR{3, 1, 146};

raylib::Texture2D backgroundTexture;
raylib::Texture2D bricks;
raylib::Texture2D player_bar;
const int brickWidth = 88, brickHeight = 46;
const std::vector<raylib::Rectangle> BRICK_VECTOR{
    raylib::Rectangle{0, 0, brickWidth, brickHeight},
    raylib::Rectangle{88, 96, brickWidth, brickHeight},
    raylib::Rectangle{264, 51, brickWidth, brickHeight},
    raylib::Rectangle{176, 145, brickWidth, brickHeight},
};

struct Player
{
  raylib::Vector2 position; // player bar center
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
  raylib::Vector2 position; // ball center
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
  raylib::Vector2 position; // brick center
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

  ::SetTargetFPS(120);

  backgroundTexture = ::LoadTexture("../images/hexagon_pattern.png");
  bricks = ::LoadTexture("../images/bricks.png");
  player_bar = ::LoadTexture("../images/player_bar.png");

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}

void InitGame()
{
  brickSize = raylib::Vector2{brickWidth, brickHeight};

  player.Init(raylib::Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8}, raylib::Vector2{228, 25}, PLAYER_MAX_LIFE);

  ball.Init(raylib::Vector2{SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8 - 30}, (raylib::Vector2){0, 0}, BALL_SIZE, false);

  // Initialize bricks
  int marginTop = brickSize.GetY() * 2;

  for (int i = 0; i < BRICK_ROWS; ++i)
  {
    for (int j = 0; j < BRICK_COLUMNS; ++j)
    {
      brick[i][j].Init(raylib::Vector2{(j + 0.5f) * brickSize.GetX(), (i + 0.5f) * brickSize.GetY() + marginTop}, true);
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

      // if (::IsKeyDown(::KEY_LEFT) && (player.position.GetX() -
      // player.size.GetX() / 2) > 0)
      // {
      //   player.position.SetX(player.position.GetX() - PLAYER_SPEED);
      // }
      // else if (::IsKeyDown(::KEY_RIGHT) && (player.position.GetX() +
      // player.size.GetX() / 2) < SCREEN_WIDTH)
      // {
      //   player.position.SetX(player.position.GetX() + PLAYER_SPEED);
      // }

      player.position.SetX(::GetMouseX());

      // Ball launching logic
      if (!ball.shouldRender && ::IsKeyPressed(::KEY_SPACE))
      {
        ball.shouldRender = true;
        ball.speed = raylib::Vector2{0, -BALL_SPEED}; // straight up
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
          Brick currBrick = brick[i][j];
          if (currBrick.shouldRender)
          {
            int ballTop = ball.position.GetY() - ball.radius;
            int ballBottom = ball.position.GetY() + ball.radius;
            int ballLeft = ball.position.GetX() - ball.radius;
            int ballRight = ball.position.GetX() + ball.radius;
            int brickTop = currBrick.position.GetY() - brickSize.GetY() / 2;
            int brickBottom = currBrick.position.GetY() + brickSize.GetY() / 2;
            int brickLeft = currBrick.position.GetX() - brickSize.GetX() / 2;
            int brickRight = currBrick.position.GetX() + brickSize.GetX() / 2;

            // Hit below
            if ((ballTop <= brickBottom) && (ballTop > brickBottom + ball.speed.GetY()) &&
                (std::fabs(ball.position.GetX() - brick[i][j].position.GetX()) <
                 (brickSize.GetX() / 2 + ball.radius * 2 / 3)) &&
                (ball.speed.GetY() < 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetY(ball.speed.GetY() * -1);
            }
            // Hit above
            else if ((ballBottom >= brickTop) && (ballBottom < brickTop + ball.speed.GetY()) &&
                     ((std::fabs(ball.position.GetX() - brick[i][j].position.GetX())) <
                      (brickSize.GetX() / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.GetY() > 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetY(ball.speed.GetY() * -1);
            }
            // Hit left
            else if ((ballRight >= brickLeft) && (ballRight < brickLeft + ball.speed.GetX()) &&
                     ((std::fabs(ball.position.GetY() - brick[i][j].position.GetY())) <
                      (brickSize.GetY() / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.GetX() > 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetX(ball.speed.GetX() * -1);
            }
            // Hit right
            else if ((ballLeft <= brickRight) && (ballLeft > brickRight + ball.speed.GetX()) &&
                     ((std::fabs(ball.position.GetY() - brick[i][j].position.GetY())) <
                      (brickSize.GetY() / 2 + ball.radius * 2 / 3)) &&
                     (ball.speed.GetX() < 0))
            {
              brick[i][j].shouldRender = false;
              ball.speed.SetX(ball.speed.GetX() * -1);
            }
          }
        }
      }

      // Game over logic
      if (player.life <= 0)
      {
        gameOver = true;
      }
      else
      {
        gameOver = true;
        for (int i = 0; i < BRICK_ROWS; ++i)
        {
          for (int j = 0; j < BRICK_COLUMNS; ++j)
          {
            if (brick[i][j].shouldRender)
            {
              gameOver = false;
              break;
            }
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

  backgroundTexture.Draw(raylib::Vector2{0, 0}, 0.0f, 5.0f, raylib::Color::White);

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
    player_bar.Draw(raylib::Rectangle{0, 0, 228, 25},
                    raylib::Vector2{player.position.GetX() - player.size.GetX() / 2,
                                    player.position.GetY() - player.size.GetY() / 2},
                    raylib::Color::RayWhite);
    // ::DrawRectangleGradientV(player.position.GetX() - player.size.GetX() / 2,
    //                          player.position.GetY() - player.size.GetY() / 2, player.size.GetX(), player.size.GetY(),
    //                          raylib::Color::LightGray, raylib::Color::DarkGray);

    // Draw ball
    ::DrawCircleGradient(ball.position.GetX(), ball.position.GetY(), ball.radius, raylib::Color::White,
                         raylib::Color::RayWhite);

    // Draw bricks
    for (int i = 0; i < BRICK_ROWS; ++i)
    {
      for (int j = 0; j < BRICK_COLUMNS; ++j)
      {
        if (brick[i][j].shouldRender)
        {
          bricks.Draw(BRICK_VECTOR[(i + j) % (int)BRICK_VECTOR.size()],
                      raylib::Vector2{brick[i][j].position.GetX() - brickSize.GetX() / 2,
                                      brick[i][j].position.GetY() - brickSize.GetY() / 2},
                      raylib::Color::RayWhite);

          // ::DrawRectangleGradientH(brick[i][j].position.GetX() - brickSize.GetX() / 2,
          //                          brick[i][j].position.GetY() - brickSize.GetY() / 2, brickSize.GetX(),
          //                          brickSize.GetY(), COLOR_VECTOR[(i + j) % (int)COLOR_VECTOR.size()],
          //                          TINT_VECTOR[(i + j) % (int)TINT_VECTOR.size()]);
          // ::DrawRectangleLinesEx(raylib::Rectangle{brick[i][j].position.GetX() - brickSize.GetX() / 2,
          //                                          brick[i][j].position.GetY() - brickSize.GetY() / 2,
          //                                          brickSize.GetX(), brickSize.GetY()},
          //                        3, raylib::Color::Black);
        }
      }
    }

    if (pause)
    {
      ::DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
                 GRAY);
    }
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
