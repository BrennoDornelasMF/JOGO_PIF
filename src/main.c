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
    InitCarSounds();

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
        //efeito de vinheta do tunel
        if(currentBiome == BIOME_TUNEL){
            //escurece as bordas da tela simulando o tunel
            unsigned char alpha =(unsigned char)(biomeTransition * 180);
            DrawRectangleGradientH(0,0,SCREEN_WIDTH/3,SCREEN_HEIGHT,
            (Color){0,0,0,alpha}, (Color){0,0,0,0});
            DrawRectangleGradientH(SCREEN_WIDTH*2/3,0,SCREEN_WIDTH/3,SCREEN_HEIGHT,
            (Color){0,0,0,0}, (Color){0,0,0, alpha});
        }
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