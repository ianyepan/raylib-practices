#include "../include/raylib-cpp.hpp"
#include "raylib.h"

#include <array>

namespace
{
const int SNAKE_LENGTH = 256;
const int SQUARE_SIZE = 31;

struct Snake
{
  raylib::Vector2 position;
  raylib::Vector2 size;
  raylib::Vector2 speed;
  raylib::Color color;
};

struct Food
{
  raylib::Vector2 position;
  raylib::Vector2 size;
  bool active;
  raylib::Color color;
};

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

int framesCounter;
bool isGameOver;
bool isPaused;

Food fruit = {0};
// Snake snake[SNAKE_LENGTH] = {0};
std::array<Snake, SNAKE_LENGTH> snake;
// raylib::Vector2 snakePosition[SNAKE_LENGTH] = {0};
std::array<raylib::Vector2, SNAKE_LENGTH> snakePosition;
bool allowMove;
raylib::Vector2 offset;
int counterTail;

void InitGame();
void UpdateGame();
void DrawGame();
void UpdateDrawFrame();

void InitGame()
{
  framesCounter = 0;
  isGameOver = false;
  isPaused = false;

  counterTail = 1;
  allowMove = false;

  offset.x = SCREEN_WIDTH % SQUARE_SIZE;
  offset.y = SCREEN_HEIGHT % SQUARE_SIZE;

  for (int i = 0; i < SNAKE_LENGTH; ++i)
  {
    snake[i].position = raylib::Vector2{offset.x / 2, offset.y / 2};
    snake[i].size = raylib::Vector2{SQUARE_SIZE, SQUARE_SIZE};
    snake[i].speed = raylib::Vector2{SQUARE_SIZE, 0};

    snake[i].color = (i == 0) ? ::DARKBLUE : ::BLUE;
  }

  for (int i = 0; i < SNAKE_LENGTH; ++i)
  {
    snakePosition[i] = (raylib::Vector2){0.0f, 0.0f};
  }

  fruit.size = (raylib::Vector2){SQUARE_SIZE, SQUARE_SIZE};
  fruit.color = SKYBLUE;
  fruit.active = false;
}

// Update game (one frame)
void UpdateGame()
{
  if (!isGameOver)
  {
    if (IsKeyPressed('P'))
      isPaused = !isPaused;

    if (!isPaused)
    {
      // Player control
      if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
      {
        snake[0].speed = (raylib::Vector2){SQUARE_SIZE, 0};
        allowMove = false;
      }
      if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
      {
        snake[0].speed = (raylib::Vector2){-SQUARE_SIZE, 0};
        allowMove = false;
      }
      if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
      {
        snake[0].speed = (raylib::Vector2){0, -SQUARE_SIZE};
        allowMove = false;
      }
      if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
      {
        snake[0].speed = (raylib::Vector2){0, SQUARE_SIZE};
        allowMove = false;
      }

      // Snake movement
      for (int i = 0; i < counterTail; ++i)
        snakePosition[i] = snake[i].position;

      if ((framesCounter % 5) == 0)
      {
        for (int i = 0; i < counterTail; ++i)
        {
          if (i == 0)
          {
            snake[0].position.x += snake[0].speed.x;
            snake[0].position.y += snake[0].speed.y;
            allowMove = true;
          }
          else
            snake[i].position = snakePosition[i - 1];
        }
      }

      // Wall behaviour
      if (((snake[0].position.x) > (SCREEN_WIDTH - offset.x)) || ((snake[0].position.y) > (SCREEN_HEIGHT - offset.y)) ||
          (snake[0].position.x < 0) || (snake[0].position.y < 0))
      {
        isGameOver = true;
      }

      // Collision with yourself
      for (int i = 1; i < counterTail; ++i)
      {
        if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))
          isGameOver = true;
      }

      // Fruit position calculation
      if (!fruit.active)
      {
        fruit.active = true;
        fruit.position =
            (raylib::Vector2){GetRandomValue(0, (SCREEN_WIDTH / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2,
                              GetRandomValue(0, (SCREEN_HEIGHT / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2};

        for (int i = 0; i < counterTail; ++i)
        {
          while ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y))
          {
            fruit.position =
                (raylib::Vector2){GetRandomValue(0, (SCREEN_WIDTH / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2,
                                  GetRandomValue(0, (SCREEN_HEIGHT / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2};
            i = 0;
          }
        }
      }

      // Collision
      if ((snake[0].position.x < (fruit.position.x + fruit.size.x) &&
           (snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
          (snake[0].position.y < (fruit.position.y + fruit.size.y) &&
           (snake[0].position.y + snake[0].size.y) > fruit.position.y))
      {
        snake[counterTail].position = snakePosition[counterTail - 1];
        counterTail += 1;
        fruit.active = false;
      }

      framesCounter++;
    }
  }
  else
  {
    if (IsKeyPressed(KEY_ENTER))
    {
      InitGame();
      isGameOver = false;
    }
  }
}

// Draw game (one frame)
void DrawGame()
{
  BeginDrawing();

  ClearBackground(RAYWHITE);

  if (!isGameOver)
  {
    // Draw grid lines
    for (int i = 0; i < SCREEN_WIDTH / SQUARE_SIZE + 1; ++i)
    {
      DrawLineV((raylib::Vector2){SQUARE_SIZE * i + offset.x / 2, offset.y / 2},
                (raylib::Vector2){SQUARE_SIZE * i + offset.x / 2, SCREEN_HEIGHT - offset.y / 2}, LIGHTGRAY);
    }

    for (int i = 0; i < SCREEN_HEIGHT / SQUARE_SIZE + 1; ++i)
    {
      DrawLineV((raylib::Vector2){offset.x / 2, SQUARE_SIZE * i + offset.y / 2},
                (raylib::Vector2){SCREEN_WIDTH - offset.x / 2, SQUARE_SIZE * i + offset.y / 2}, LIGHTGRAY);
    }

    // Draw snake
    for (int i = 0; i < counterTail; ++i)
      DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

    // Draw fruit to pick
    DrawRectangleV(fruit.position, fruit.size, fruit.color);

    if (isPaused)
      DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, GRAY);
  }
  else
    DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
             GetScreenHeight() / 2 - 50, 20, ::DARKGRAY);

  EndDrawing();
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
  raylib::Window window{SCREEN_WIDTH, SCREEN_HEIGHT, "Sample Game: Snake"};
  InitGame();
  ::SetTargetFPS(40);

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}
