#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define MAX_TUBES 100
#define FLAPPY_RADIUS 24
#define TUBES_WIDTH 40

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Flappy
{
  Vector2 position;
  int radius;
  Color color;
} Flappy;

typedef struct Tubes
{
  Rectangle rec;
  Color color;
  bool active;
} Tubes;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static bool gameOver = false;
static bool pause = false;
static int score = 0;
static int hiScore = 0;

static Flappy flappy = {0};
static Tubes tubes[MAX_TUBES * 2] = {0};
static Vector2 tubesPos[MAX_TUBES] = {0};
static int tubesSpeedX = 0;
static bool superfx = false;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);        // Initialize game
static void UpdateGame(void);      // Update game (one frame)
static void DrawGame(void);        // Draw game (one frame)
static void UnloadGame(void);      // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization
  //---------------------------------------------------------
  InitWindow(screenWidth, screenHeight, "sample game: flappy");

  InitGame();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
  SetTargetFPS(60);
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update and Draw
    //----------------------------------------------------------------------------------
    UpdateDrawFrame();
    //----------------------------------------------------------------------------------
  }
#endif
  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadGame(); // Unload loaded data (textures, sounds, models...)

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{
  flappy.radius = FLAPPY_RADIUS;
  flappy.position = (Vector2){80, screenHeight / 2 - flappy.radius};
  tubesSpeedX = 2;

  for (int i = 0; i < MAX_TUBES; i++)
  {
    tubesPos[i].x = 400 + 280 * i;
    tubesPos[i].y = -GetRandomValue(0, 120);
  }

  for (int i = 0; i < MAX_TUBES * 2; i += 2)
  {
    tubes[i].rec.x = tubesPos[i / 2].x;
    tubes[i].rec.y = tubesPos[i / 2].y;
    tubes[i].rec.width = TUBES_WIDTH;
    tubes[i].rec.height = 255;

    tubes[i + 1].rec.x = tubesPos[i / 2].x;
    tubes[i + 1].rec.y = 600 + tubesPos[i / 2].y - 255;
    tubes[i + 1].rec.width = TUBES_WIDTH;
    tubes[i + 1].rec.height = 255;

    tubes[i / 2].active = true;
  }

  score = 0;

  gameOver = false;
  superfx = false;
  pause = false;
}

// Update game (one frame)
void UpdateGame(void)
{
  if (!gameOver)
  {
    if (IsKeyPressed('P'))
      pause = !pause;

    if (!pause)
    {
      for (int i = 0; i < MAX_TUBES; i++)
        tubesPos[i].x -= tubesSpeedX;

      for (int i = 0; i < MAX_TUBES * 2; i += 2)
      {
        tubes[i].rec.x = tubesPos[i / 2].x;
        tubes[i + 1].rec.x = tubesPos[i / 2].x;
      }

      if (IsKeyDown(KEY_SPACE) && !gameOver)
        flappy.position.y -= 3;
      else
        flappy.position.y += 1;

      // Check Collisions
      for (int i = 0; i < MAX_TUBES * 2; i++)
      {
        if (CheckCollisionCircleRec(flappy.position, flappy.radius, tubes[i].rec))
        {
          gameOver = true;
          pause = false;
        }
        else if ((tubesPos[i / 2].x < flappy.position.x) && tubes[i / 2].active && !gameOver)
        {
          score += 100;
          tubes[i / 2].active = false;

          superfx = true;

          if (score > hiScore)
            hiScore = score;
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
  BeginDrawing();

  ClearBackground(RAYWHITE);

  if (!gameOver)
  {
    DrawCircle(flappy.position.x, flappy.position.y, flappy.radius, DARKGRAY);

    // Draw tubes
    for (int i = 0; i < MAX_TUBES; i++)
    {
      DrawRectangle(tubes[i * 2].rec.x, tubes[i * 2].rec.y, tubes[i * 2].rec.width, tubes[i * 2].rec.height, GRAY);
      DrawRectangle(tubes[i * 2 + 1].rec.x, tubes[i * 2 + 1].rec.y, tubes[i * 2 + 1].rec.width,
                    tubes[i * 2 + 1].rec.height, GRAY);
    }

    // Draw flashing fx (one frame only)
    if (superfx)
    {
      DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
      superfx = false;
    }

    DrawText(TextFormat("%04i", score), 20, 20, 40, GRAY);
    DrawText(TextFormat("HI-SCORE: %04i", hiScore), 20, 70, 20, LIGHTGRAY);

    if (pause)
      DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
  }
  else
    DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
             GetScreenHeight() / 2 - 50, 20, GRAY);

  EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
  // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
  UpdateGame();
  DrawGame();
}
