#include "../include/raylib-cpp.hpp"
#include "raylib.h"

#include <array>

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
const int NUM_BULLETS = 50;
const int NUM_MAX_ENEMIES = 50;
const int FIRST_WAVE = 10;
const int SECOND_WAVE = 20;
const int THIRD_WAVE = 50;

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
enum EnemyWave
{
  FIRST = 0,
  SECOND,
  THIRD
};

struct Player
{
  raylib::Rectangle rec;
  raylib::Vector2 speed;
  raylib::Color color;

  Player(raylib::Rectangle p_rec, raylib::Vector2 p_speed, raylib::Color p_color)
      : rec{p_rec}, speed{p_speed}, color{p_color}
  {
  }
};

struct Enemy
{
  raylib::Rectangle rec;
  raylib::Vector2 speed;
  bool active;
  raylib::Color color;
};

struct Bullet
{
  raylib::Rectangle rec;
  raylib::Vector2 speed;
  bool active;
  raylib::Color color;
};

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 450;

static bool isGameOver = false;
static bool isPaused = false;
static int score = 0;
static bool victory = false;

static Player player{raylib::Rectangle{20,50,20,20},raylib::Vector2{5,5}, raylib::Color::Black};
static std::array<Enemy, NUM_MAX_ENEMIES> enemy;
static std::array<Bullet, NUM_BULLETS> bullet;
static EnemyWave wave;

static int bulletRate = 0;
static float alpha = 0.0f;

static int activeEnemies = 0;
static int enemiesKill = 0;
static bool smooth = false;

static void InitGame();
static void UpdateGame();
static void DrawGame();
static void UpdateDrawFrame();

int main()
{
  raylib::Window window{SCREEN_WIDTH, SCREEN_HEIGHT, "Sample game: Space Invaders"};
  InitGame();
  SetTargetFPS(60);

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}

void InitGame()
{
  // Initialize enemies
  for (int i = 0; i < NUM_MAX_ENEMIES; i++)
  {
    enemy[i].rec.width = 10;
    enemy[i].rec.height = 10;
    enemy[i].rec.x = GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000);
    enemy[i].rec.y = GetRandomValue(0, SCREEN_HEIGHT - enemy[i].rec.height);
    enemy[i].speed.x = 5;
    enemy[i].speed.y = 5;
    enemy[i].active = true;
    enemy[i].color = GRAY;
  }

  // Initialize bullets
  for (int i = 0; i < NUM_BULLETS; i++)
  {
    bullet[i].rec.x = player.rec.x;
    bullet[i].rec.y = player.rec.y + player.rec.height / 4;
    bullet[i].rec.width = 10;
    bullet[i].rec.height = 5;
    bullet[i].speed.x = 7;
    bullet[i].speed.y = 0;
    bullet[i].active = false;
    bullet[i].color = MAROON;
  }
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
      switch (wave)
      {
      case FIRST: {
        if (!smooth)
        {
          alpha += 0.02f;

          if (alpha >= 1.0f)
            smooth = true;
        }

        if (smooth)
          alpha -= 0.02f;

        if (enemiesKill == activeEnemies)
        {
          enemiesKill = 0;

          for (int i = 0; i < activeEnemies; i++)
          {
            if (!enemy[i].active)
              enemy[i].active = true;
          }

          activeEnemies = SECOND_WAVE;
          wave = SECOND;
          smooth = false;
          alpha = 0.0f;
        }
      }
      break;
      case SECOND: {
        if (!smooth)
        {
          alpha += 0.02f;

          if (alpha >= 1.0f)
            smooth = true;
        }

        if (smooth)
          alpha -= 0.02f;

        if (enemiesKill == activeEnemies)
        {
          enemiesKill = 0;

          for (int i = 0; i < activeEnemies; i++)
          {
            if (!enemy[i].active)
              enemy[i].active = true;
          }

          activeEnemies = THIRD_WAVE;
          wave = THIRD;
          smooth = false;
          alpha = 0.0f;
        }
      }
      break;
      case THIRD: {
        if (!smooth)
        {
          alpha += 0.02f;

          if (alpha >= 1.0f)
            smooth = true;
        }

        if (smooth)
          alpha -= 0.02f;

        if (enemiesKill == activeEnemies)
          victory = true;
      }
      break;
      default:
        break;
      }

      // Player movement
      if (IsKeyDown(KEY_RIGHT))
        player.rec.x += player.speed.x;
      if (IsKeyDown(KEY_LEFT))
        player.rec.x -= player.speed.x;
      if (IsKeyDown(KEY_UP))
        player.rec.y -= player.speed.y;
      if (IsKeyDown(KEY_DOWN))
        player.rec.y += player.speed.y;

      // Player collision with enemy
      for (int i = 0; i < activeEnemies; i++)
      {
        if (CheckCollisionRecs(player.rec, enemy[i].rec))
          isGameOver = true;
      }

      // Enemy behaviour
      for (int i = 0; i < activeEnemies; i++)
      {
        if (enemy[i].active)
        {
          enemy[i].rec.x -= enemy[i].speed.x;

          if (enemy[i].rec.x < 0)
          {
            enemy[i].rec.x = GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000);
            enemy[i].rec.y = GetRandomValue(0, SCREEN_HEIGHT - enemy[i].rec.height);
          }
        }
      }

      // Wall behaviour
      if (player.rec.x <= 0)
        player.rec.x = 0;
      if (player.rec.x + player.rec.width >= SCREEN_WIDTH)
        player.rec.x = SCREEN_WIDTH - player.rec.width;
      if (player.rec.y <= 0)
        player.rec.y = 0;
      if (player.rec.y + player.rec.height >= SCREEN_HEIGHT)
        player.rec.y = SCREEN_HEIGHT - player.rec.height;

      // Bullet initialization
      if (IsKeyDown(KEY_SPACE))
      {
        bulletRate += 5;

        for (int i = 0; i < NUM_BULLETS; i++)
        {
          if (!bullet[i].active && bulletRate % 20 == 0)
          {
            bullet[i].rec.x = player.rec.x;
            bullet[i].rec.y = player.rec.y + player.rec.height / 4;
            bullet[i].active = true;
            break;
          }
        }
      }

      // Bullet logic
      for (int i = 0; i < NUM_BULLETS; i++)
      {
        if (bullet[i].active)
        {
          // Movement
          bullet[i].rec.x += bullet[i].speed.x;

          // Collision with enemy
          for (int j = 0; j < activeEnemies; j++)
          {
            if (enemy[j].active)
            {
              if (CheckCollisionRecs(bullet[i].rec, enemy[j].rec))
              {
                bullet[i].active = false;
                enemy[j].rec.x = GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000);
                enemy[j].rec.y = GetRandomValue(0, SCREEN_HEIGHT - enemy[j].rec.height);
                bulletRate = 0;
                enemiesKill++;
                score += 100;
              }

              if (bullet[i].rec.x + bullet[i].rec.width >= SCREEN_WIDTH)
              {
                bullet[i].active = false;
                bulletRate = 0;
              }
            }
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
      isGameOver = false;
    }
  }
}

// Draw each frame
void DrawGame()
{
  ::BeginDrawing();
  ::ClearBackground(raylib::Color::RayWhite);

  if (!isGameOver)
  {
    player.rec.Draw(player.color);

    if (wave == FIRST)
    {
      ::DrawText("FIRST WAVE", SCREEN_WIDTH / 2 - MeasureText("FIRST WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
               Fade(BLACK, alpha));
    }
    else if (wave == SECOND)
    {
      ::DrawText("SECOND WAVE", SCREEN_WIDTH / 2 - MeasureText("SECOND WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
               Fade(BLACK, alpha));
    }
    else if (wave == THIRD)
    {
      ::DrawText("THIRD WAVE", SCREEN_WIDTH / 2 - MeasureText("THIRD WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
               Fade(BLACK, alpha));
    }

    for (int i = 0; i < activeEnemies; i++)
    {
      if (enemy[i].active)
      {
        enemy[i].rec.Draw(enemy[i].color);
      }
    }

    for (int i = 0; i < NUM_BULLETS; i++)
    {
      if (bullet[i].active)
      {
        bullet[i].rec.Draw(bullet[i].color);
      }
    }

    DrawText(TextFormat("%04i", score), 20, 20, 40, GRAY);

    if (victory)
      DrawText("YOU WIN", SCREEN_WIDTH / 2 - MeasureText("YOU WIN", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, BLACK);

    if (isPaused)
      DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, GRAY);
  }
  else
    DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
             GetScreenHeight() / 2 - 50, 20, GRAY);

  EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame()
{
  UpdateGame();
  DrawGame();
}
