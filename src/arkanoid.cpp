#include "../include/raylib-cpp.hpp"
#include "raylib.h"

#include <array>
#include <cmath>

namespace
{
const int PLAYER_MAX_LIFE = 3;
const int BRICK_ROWS = 5;
const int BRICK_COLUMNS = 11;
const int SCREEN_WIDTH = 970;
const int SCREEN_HEIGHT = 675;
const int PLAYER_SPEED = 8;
const int BALL_SIZE = 12;
const int BALL_SPEED = 5;
const int BRICK_WIDTH = 88, BRICK_HEIGHT = 46;
const std::array<raylib::Rectangle, 6> BRICK_TEXTURE_POOL{
    raylib::Rectangle{0, 0, BRICK_WIDTH, BRICK_HEIGHT},     raylib::Rectangle{88, 96, BRICK_WIDTH, BRICK_HEIGHT},
    raylib::Rectangle{264, 51, BRICK_WIDTH, BRICK_HEIGHT},  raylib::Rectangle{264, 95, BRICK_WIDTH, BRICK_HEIGHT},
    raylib::Rectangle{440, 144, BRICK_WIDTH, BRICK_HEIGHT}, raylib::Rectangle{176, 145, BRICK_WIDTH, BRICK_HEIGHT},
};

raylib::Texture2D backgroundTexture;
raylib::Texture2D bricksTexture;
raylib::Texture2D playerTexture;
std::array<raylib::Rectangle, BRICK_ROWS * BRICK_COLUMNS> brickTextures;

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
bool gameOver = false;
bool pause = false;
Player player;
Ball ball;
Brick brick[BRICK_ROWS][BRICK_COLUMNS];
raylib::Vector2 brickSize;

// Function prototypes
void InitGame();        // Initialize game
void UpdateGame();      // Update game (one frame)
void DrawGame();        // Draw game (one frame)
void UpdateDrawFrame(); // Update and Draw (one frame)

void InitGame()
{
  brickSize = raylib::Vector2{BRICK_WIDTH, BRICK_HEIGHT};

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

  for (int i = 0; i < BRICK_ROWS * BRICK_COLUMNS; ++i)
  {
    brickTextures[i] = BRICK_TEXTURE_POOL[::GetRandomValue(0, (int)BRICK_TEXTURE_POOL.size() - 1)];
  }

  // Load textures
  backgroundTexture = ::LoadTexture("../assets/hexagon_pattern.png");
  bricksTexture = ::LoadTexture("../assets/bricks.png");
  playerTexture = ::LoadTexture("../assets/player_bar.png");
}

// Update game variables (one frame)
void UpdateGame()
{
  if (!gameOver)
  {
    if (::IsKeyPressed(::KEY_P))
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
      if (!ball.shouldRender && (::IsKeyPressed(::KEY_SPACE) || ::IsMouseButtonPressed(::MOUSE_LEFT_BUTTON)))
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

  ::ClearBackground(::RAYWHITE);

  backgroundTexture.Draw(raylib::Vector2{0, 0}, 0.0f, 5.0f, WHITE);

  if (!gameOver)
  {
    // Draw lives (health)
    raylib::Vector2 hudPosition{20, SCREEN_HEIGHT - 70};
    raylib::Vector2 hudSize{150, 40};
    hudPosition.DrawRectangle(hudSize, raylib::Color(::GREEN).Fade(0.5f));
    ::DrawRectangleLines(hudPosition.GetX(), hudPosition.GetY(), hudSize.GetX(), hudSize.GetY(),
                         ::RAYWHITE);
    ::DrawText("Lives Left:", hudPosition.GetX() + 10, hudPosition.GetY() + 10, 20, ::GREEN);

    for (int i = 0; i < player.life; ++i)
    {
      DrawRectangleGradientV(20 + 40 * i, SCREEN_HEIGHT - 20, 35, 10, ::GREEN, ::LIME);
    }

    // Draw player bar
    playerTexture.Draw(raylib::Rectangle{0, 0, 228, 25},
                       raylib::Vector2{player.position.GetX() - player.size.GetX() / 2,
                                       player.position.GetY() - player.size.GetY() / 2},
                       ::RAYWHITE);

    // Draw ball
    ::DrawCircleGradient(ball.position.GetX(), ball.position.GetY(), ball.radius, ::WHITE,
                         ::RAYWHITE);

    // Draw bricks
    int k = 0;
    for (int i = 0; i < BRICK_ROWS; ++i)
    {
      for (int j = 0; j < BRICK_COLUMNS; ++j)
      {
        if (brick[i][j].shouldRender)
        {
          bricksTexture.Draw(brickTextures[k],
                             raylib::Vector2{brick[i][j].position.GetX() - brickSize.GetX() / 2,
                                             brick[i][j].position.GetY() - brickSize.GetY() / 2},
                             ::RAYWHITE);
        }
        ++k;
      }
    }

    if (pause)
    {
      ::DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
                 ::LIGHTGRAY);
    }
  }
  else // Game over
  {
    ::DrawText("PRESS [ENTER] TO PLAY AGAIN",
               ::GetScreenWidth() / 2 - ::MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
               ::GetScreenHeight() / 2 - 50, 20, ::LIGHTGRAY);
  }

  ::EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame()
{
  UpdateGame();
  DrawGame();
}

} // namespace

int main()
{
  raylib::Window window{SCREEN_WIDTH, SCREEN_HEIGHT, "Sample game: Arkanoid"};

  InitGame();

  ::SetTargetFPS(120);

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}
