#include "../include/raylib-cpp.hpp"
#include "raylib.h"

#include <array>

const int NUM_BULLETS = 50;
const int NUM_MAX_ENEMIES = 50;
const int FIRST_WAVE_ENEMIES = 10;
const int SECOND_WAVE_ENEMIES = 20;
const int THIRD_WAVE_ENEMIES = 50;

enum EnemyWave
{
  FIRST_WAVE,
  SECOND_WAVE,
  THIRD_WAVE
};

struct Player
{
  raylib::Rectangle rec;
  raylib::Vector2 speed;
  raylib::Color color;

  Player(raylib::Rectangle _rec, raylib::Vector2 _speed, raylib::Color _color) : rec{_rec}, speed{_speed}, color{_color}
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

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 450;

static bool isGameOver = false;
static bool isPaused = false;
static int score = 0;
static bool victory = false;

static Player player{raylib::Rectangle{20, 50, 20, 20}, raylib::Vector2{5, 5}, raylib::Color::White};
static std::array<Enemy, NUM_MAX_ENEMIES> enemies;
static std::array<Bullet, NUM_BULLETS> bullets;
static EnemyWave wave = FIRST_WAVE;

static int bulletRate = 0;
static float alpha = 0.0f;

static int activeEnemies = FIRST_WAVE_ENEMIES;
static int enemyKills = 0;
static bool isOpaque = false;

static void InitGame();
static void tuneAlpha();
static void initNextWave();
static void UpdateGame();
static void announceWave(EnemyWave wave);
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
  for (auto &enemy : enemies)
  {
    enemy.rec.x = GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000);
    enemy.rec.y = GetRandomValue(0, SCREEN_HEIGHT - enemy.rec.height);
    enemy.rec.width = 10;
    enemy.rec.height = 10;
    enemy.speed.x = 5;
    enemy.speed.y = 5;
    enemy.active = true;
    enemy.color = raylib::Color::LightGray;
  }

  // Initialize bullets
  for (auto &bullet : bullets)
  {
    bullet.rec.x = player.rec.x;
    bullet.rec.y = player.rec.y + player.rec.height / 4;
    bullet.rec.width = 10;
    bullet.rec.height = 5;
    bullet.speed.x = 7;
    bullet.speed.y = 0;
    bullet.active = false;
    bullet.color = raylib::Color::Yellow;
  }
}

void tuneAlpha()
{
  if (!isOpaque)
  {
    alpha += 0.02f;
    if (alpha >= 1.0f)
    {
      isOpaque = true;
    }
  }
  else
  {
    alpha -= 0.02f;
  }
}

void initNextWave()
{
  enemyKills = 0;

  for (int i = 0; i < activeEnemies; ++i)
  {
    enemies[i].active = true;
  }

  isOpaque = false; // so that text can fade in upon next wave
  alpha = 0.0f;
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
      if (wave == FIRST_WAVE)
      {
        tuneAlpha();

        // Ready to move on to next wave
        if (enemyKills == activeEnemies)
        {
          wave = SECOND_WAVE;
          activeEnemies = SECOND_WAVE_ENEMIES;
          initNextWave();
        }
      }
      else if (wave == SECOND_WAVE)
      {
        tuneAlpha();

        if (enemyKills == activeEnemies)
        {
          wave = THIRD_WAVE;
          activeEnemies = THIRD_WAVE_ENEMIES;
          initNextWave();
        }
      }
      else if (wave == THIRD_WAVE)
      {
        tuneAlpha();

        if (enemyKills == activeEnemies)
        {
          victory = true;
        }
      }

      // Player movement
      if (IsKeyDown(KEY_RIGHT))
      {
        player.rec.x += player.speed.x;
      }
      if (IsKeyDown(KEY_LEFT))
      {
        player.rec.x -= player.speed.x;
      }
      if (IsKeyDown(KEY_UP))
      {
        player.rec.y -= player.speed.y;
      }
      if (IsKeyDown(KEY_DOWN))
      {
        player.rec.y += player.speed.y;
      }

      // Player collision with enemies
      for (int i = 0; i < activeEnemies; ++i)
      {
        if (CheckCollisionRecs(player.rec, enemies[i].rec))
        {
          isGameOver = true;
        }
      }

      // Enemy behaviour
      for (int i = 0; i < activeEnemies; ++i)
      {
        if (enemies[i].active)
        {
          enemies[i].rec.x -= enemies[i].speed.x;

          if (enemies[i].rec.x < 0)
          {
            enemies[i].rec.x = GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000);
            enemies[i].rec.y = GetRandomValue(0, SCREEN_HEIGHT - enemies[i].rec.height);
          }
        }
      }

      // Wall behaviour
      if (player.rec.x <= 0)
      {
        player.rec.x = 0;
      }
      if (player.rec.x + player.rec.width >= SCREEN_WIDTH)
      {
        player.rec.x = SCREEN_WIDTH - player.rec.width;
      }
      if (player.rec.y <= 0)
      {
        player.rec.y = 0;
      }
      if (player.rec.y + player.rec.height >= SCREEN_HEIGHT)
      {
        player.rec.y = SCREEN_HEIGHT - player.rec.height;
      }

      // Bullet initialization
      if (IsKeyDown(KEY_SPACE))
      {
        bulletRate += 5;

        for (int i = 0; i < NUM_BULLETS; ++i)
        {
          if (!bullets[i].active && bulletRate % 20 == 0)
          {
            bullets[i].rec.x = player.rec.x;
            bullets[i].rec.y = player.rec.y + player.rec.height / 4;
            bullets[i].active = true;
            break;
          }
        }
      }

      // Bullet logic
      for (int i = 0; i < NUM_BULLETS; ++i)
      {
        if (bullets[i].active)
        {
          // Movement
          bullets[i].rec.x += bullets[i].speed.x;

          // Collision with enemies
          for (int j = 0; j < activeEnemies; ++j)
          {
            if (enemies[j].active)
            {
              if (CheckCollisionRecs(bullets[i].rec, enemies[j].rec))
              {
                bullets[i].active = false;
                enemies[j].rec.x = GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000);
                enemies[j].rec.y = GetRandomValue(0, SCREEN_HEIGHT - enemies[j].rec.height);
                bulletRate = 0;
                ++enemyKills;
                score += 100;
              }

              if (bullets[i].rec.x + bullets[i].rec.width >= SCREEN_WIDTH)
              {
                bullets[i].active = false;
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

void announceWave(EnemyWave wave)
{
  switch (wave)
  {
  case (FIRST_WAVE):
    ::DrawText("FIRST WAVE", SCREEN_WIDTH / 2 - MeasureText("FIRST WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
               Fade(raylib::Color::White, alpha));
    break;
  case (SECOND_WAVE):
    ::DrawText("SECOND WAVE", SCREEN_WIDTH / 2 - MeasureText("SECOND WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
               Fade(raylib::Color::White, alpha));
    break;
  case (THIRD_WAVE):
    ::DrawText("THIRD WAVE", SCREEN_WIDTH / 2 - MeasureText("THIRD WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
               Fade(raylib::Color::White, alpha));
    break;
  }
  // if (wave == FIRST_WAVE)
  // {
  //   ::DrawText("FIRST WAVE", SCREEN_WIDTH / 2 - MeasureText("FIRST WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
  //              Fade(raylib::Color::White, alpha));
  // }
  // else if (wave == SECOND_WAVE)
  // {
  //   ::DrawText("SECOND WAVE", SCREEN_WIDTH / 2 - MeasureText("SECOND WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
  //              Fade(raylib::Color::White, alpha));
  // }
  // else if (wave == THIRD_WAVE)
  // {
  //   ::DrawText("THIRD WAVE", SCREEN_WIDTH / 2 - MeasureText("THIRD WAVE", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
  //              Fade(raylib::Color::White, alpha));
  // }
}

// Draw each frame
void DrawGame()
{
  ::BeginDrawing();
  ::ClearBackground(raylib::Color::Black);

  if (!isGameOver)
  {
    player.rec.Draw(player.color);

    announceWave(wave);

    for (int i = 0; i < activeEnemies; ++i)
    {
      if (enemies[i].active)
      {
        enemies[i].rec.Draw(enemies[i].color);
      }
    }

    for (auto &bullet : bullets)
    {
      if (bullet.active)
      {
        bullet.rec.Draw(bullet.color);
      }
    }

    ::DrawText(::TextFormat("%04i", score), 20, 20, 40, GRAY);

    if (victory)
    {
      const char *winMessage = "YOU'VE WON!";
      ::DrawText(winMessage, SCREEN_WIDTH / 2 - MeasureText(winMessage, 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
                 raylib::Color::White);
    }

    if (isPaused)
    {
      const char *pauseMessage = "GAME PAUSED";
      ::DrawText(pauseMessage, SCREEN_WIDTH / 2 - MeasureText(pauseMessage, 40) / 2, SCREEN_HEIGHT / 2 - 40, 40,
                 raylib::Color::Gray);
    }
  }
  else
  {
    const char *promptRetry = "PRESS [ENTER] TO PLAY AGAIN";
    ::DrawText(promptRetry, SCREEN_WIDTH / 2 - MeasureText(promptRetry, 20) / 2, GetScreenHeight() / 2 - 50, 20,
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
