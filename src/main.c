#include "raylib.h"
#include "road.h"
#include "player.h"
#include "camera.h"
#include "sprite.h"
#include "npc.h"
#include "game.h"
#include "collision.h"
#include "cloud.h"
#include "audio.h"

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
    InitAudio_Game();

    while (!WindowShouldClose()) {
        // Update
        UpdatePlayer();
        UpdateGameCamera();
        UpdateBiome();
        UpdateGame(player.speed);
        UpdateNPCs(roadPosition, player.speed);
        UpdateAudio();
        
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

        // Reinicia o jogo após ver o placar
        if (gameState.scoreSaved && IsKeyPressed(KEY_ENTER)) {
            // Reseta tudo
            roadPosition = 0;
            InitGame();
            InitPlayer();
            InitNPCs(roadPosition);
        }
    }

    UnloadSprites();
    UnloadClouds();
    UnloadPlayer();
    UnloadNPCs();
    UnloadAudio_Game();
    CloseWindow();

    return 0;
}