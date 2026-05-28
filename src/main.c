#include "raylib.h"
#include "road.h"
#include "player.h"
#include "camera.h"
#include "sprite.h"
#include "npc.h"
#include "game.h"
#include "collision.h"
#include "cloud.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "teste pista");
    SetTargetFPS(60);

    InitRoad();
    InitPlayer();
    InitSprites();
    InitClouds();
    InitGame();
    InitNPCs(roadPosition);

    while (!WindowShouldClose()) {
        // Update
        UpdatePlayer();
        UpdateGameCamera();
        UpdateGame(player.speed);
        UpdateNPCs(roadPosition, player.speed);
        
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        DrawClouds();
        DrawRoad();
        DrawSprites();
        DrawNPCs(roadPosition, playerX);  // NPCs ANTES do player
        DrawPlayer();                      // player por cima
        DrawHUD();
        EndDrawing();
        
        CheckCollisions(roadPosition);
    }

    UnloadSprites();
    UnloadClouds();
    UnloadPlayer();
    UnloadNPCs();
    CloseWindow();
    return 0;
}