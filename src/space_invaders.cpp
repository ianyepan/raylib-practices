#include "../include/raylib-cpp.hpp"
#include "raylib.h"

#include <algorithm>
#include <array>

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;

const int NUM_BULLETS = 50;
const int NUM_MAX_ENEMIES = 70;
const int FIRST_WAVE_ENEMIES = 25;
const int SECOND_WAVE_ENEMIES = 40;
const int THIRD_WAVE_ENEMIES = NUM_MAX_ENEMIES;
const int PLAYER_HEIGHT = 50;
const int PLAYER_WIDTH = 20;
const int ENEMY_HEIGHT = 35;
const int ENEMY_WIDTH = 45;
const std::array<raylib::Color, 3> ENEMY_COLOR_POOL{
  raylib::Color{238,237,49}, raylib::Color{243,49,242}, raylib::Color{38,233,235}
};

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

  Player() = default;
  Player(raylib::Rectangle _rec, raylib::Vector2 _speed, raylib::Color _color) : rec{_rec}, speed{_speed}, color{_color}
  {
  }
};

struct Enemy
{
  raylib::Rectangle rec;
  int speed;
  bool active;
  raylib::Color color;

  Enemy() = default;
  Enemy(raylib::Rectangle _rec, int _speed, bool _active, raylib::Color _color)
      : rec{_rec}, speed{_speed}, active{_active}, color{_color}
  {
  }
};

struct Bullet
{
  raylib::Rectangle rec;
  int speed;
  bool active;
  raylib::Color color;
  Bullet() = default;
  Bullet(raylib::Rectangle _rec, int _speed, bool _active, raylib::Color _color)
      : rec{_rec}, speed{_speed}, active{_active}, color{_color}
  {
  }
};

static bool isGameOver = false;
static bool isPaused = false;
static int score = 0;
static bool victory = false;

static Player player{raylib::Rectangle{20, 50, PLAYER_WIDTH, PLAYER_HEIGHT}, raylib::Vector2{3, 3}, raylib::Color::White};
static std::array<Enemy, NUM_MAX_ENEMIES> enemies;
static std::array<Bullet, NUM_BULLETS> bullets;
static EnemyWave wave = FIRST_WAVE;

static int bulletRate = 0;
static float alpha = 0.0f;

static int activeEnemies = FIRST_WAVE_ENEMIES;
static int enemyKills = 0;
static bool isOpaque = false;

raylib::Texture2D playerTexture;
raylib::Texture2D enemyTexture;
raylib::Texture2D enemyTexture2;

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
  SetTargetFPS(120);

  while (!window.ShouldClose())
  {
    UpdateDrawFrame();
  }

  return 0;
}

void InitGame()
{
  playerTexture = ::LoadTexture("../assets/space_player.png");
  enemyTexture = ::LoadTexture("../assets/space_enemy.png");
  enemyTexture2 = ::LoadTexture("../assets/space_enemy2.png");

  // Initialize enemies
  for (auto &enemy : enemies)
  {
    raylib::Rectangle enemyRec{(float)GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000),
                               (float)GetRandomValue(0, SCREEN_HEIGHT - ENEMY_HEIGHT), ENEMY_WIDTH, ENEMY_HEIGHT};
    int enemySpeed = 2;
    auto enemyColor = ENEMY_COLOR_POOL[::GetRandomValue(0, (int)ENEMY_COLOR_POOL.size())];
    enemy = Enemy{enemyRec, enemySpeed, true, enemyColor};
  }

  // Initialize bullets
  for (auto &bullet : bullets)
  {
    raylib::Rectangle bulletRec{player.rec.x, player.rec.y + player.rec.height / 4, 10, 5};
    int bulletSpeed = 4;
    bullet = Bullet{bulletRec, bulletSpeed, false, raylib::Color::Yellow};
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
        if (player.rec.CheckCollision(enemies[i].rec))
        {
          isGameOver = true;
        }
      }

      // Enemy behaviour
      for (int i = 0; i < activeEnemies; ++i)
      {
        if (enemies[i].active)
        {
          enemies[i].rec.x -= enemies[i].speed;

          if (enemies[i].rec.x < 0)
          {
            enemies[i].rec.x = GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000);
            enemies[i].rec.y = GetRandomValue(0, SCREEN_HEIGHT - enemies[i].rec.height);
          }
        }
      }

      // Wall limits
      player.rec.x = std::max(player.rec.x, 0.0f);
      player.rec.x = std::min(player.rec.x, SCREEN_WIDTH - player.rec.width);
      player.rec.y = std::max(player.rec.y, 0.0f);
      player.rec.y = std::min(player.rec.y, SCREEN_HEIGHT - player.rec.height);

      // Bullet spawning
      if (::IsKeyDown(::KEY_SPACE))
      {
        bulletRate += 3;

        for (int i = 0; i < NUM_BULLETS; ++i)
        {
          if (!bullets[i].active && bulletRate % 20 == 0)
          {
            bullets[i].rec.x = player.rec.x;
            bullets[i].rec.y = player.rec.y + player.rec.height / 2;
            bullets[i].active = true;
            break;
          }
        }
      }

      // Bullet logic
      for (auto &bullet : bullets)
      {
        if (bullet.active)
        {
          // Movement
          bullet.rec.x += bullet.speed;

          // Collision with enemies
          for (int j = 0; j < activeEnemies; ++j)
          {
            if (enemies[j].active)
            {
              if (bullet.rec.CheckCollision(enemies[j].rec))
              {
                bullet.active = false;
                enemies[j].rec.x = GetRandomValue(SCREEN_WIDTH, SCREEN_WIDTH + 1000);
                enemies[j].rec.y = GetRandomValue(0, SCREEN_HEIGHT - enemies[j].rec.height);
                bulletRate = 0;
                ++enemyKills;
                score += 100;
              }

              if (bullet.rec.x + bullet.rec.width >= SCREEN_WIDTH)
              {
                bullet.active = false;
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
    // Draw Player
    // player.rec.Draw(player.color);
    playerTexture.Draw(raylib::Vector2{player.rec.x+PLAYER_WIDTH*2, player.rec.y-5}, 90.0f, 0.35f, raylib::Color::White);

    // Draw enemies
    for (int i = 0; i < activeEnemies; ++i)
    {
      if (enemies[i].active)
      {
        // enemies[i].rec.Draw(enemies[i].color);
        enemyTexture.Draw(raylib::Vector2{enemies[i].rec.x, enemies[i].rec.y}, 0.0f, 0.16f, enemies[i].color);
      }
    }

    // Draw bullets
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

  announceWave(wave);

  ::EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame()
{
  UpdateGame();
  DrawGame();
}
