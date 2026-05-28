#include "raylib.h"
#include "road.h"
#include "player.h"
#include "camera.h"
#include "sprite.h"
#include "npc.h"
#include "game.h"
#include "collision.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "teste pista");
    SetTargetFPS(60);

    InitRoad();
    InitPlayer();
    InitSprites();
    InitGame();
    InitNPCs(roadPosition);

    while (!WindowShouldClose()) {
        // Update
        UpdatePlayer();
        UpdateGameCamera();
        UpdateGame(player.speed);
        UpdateNPCs(roadPosition, player.speed);
        CheckCollisions(roadPosition);

        // Draw
        BeginDrawing();
            ClearBackground(SKYBLUE);
            DrawRoad();
            DrawSprites();
            DrawNPCs(roadPosition, playerX);  // NPCs ANTES do player
            DrawPlayer();                      // player por cima
            DrawHUD();
        EndDrawing();
    }

    UnloadSprites();
    UnloadPlayer();
    UnloadNPCs();
    CloseWindow();
    return 0;
}