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

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Corrida");
    SetTargetFPS(60);

    // ------------------------------------------------
    // TODO: aqui entrará o menu principal
    // Troque MODE_COOP por MODE_SINGLE para jogar sozinho
    // ------------------------------------------------
    gameMode          = MODE_COOP;
    activePlayerCount = (gameMode == MODE_COOP) ? 2 : 1;

    int viewW = (gameMode == MODE_COOP) ? SCREEN_WIDTH / 2 : SCREEN_WIDTH;

    RenderTexture2D rtP1 = { 0 };
    RenderTexture2D rtP2 = { 0 };
    if (gameMode == MODE_COOP) {
        rtP1 = LoadRenderTexture(viewW, SCREEN_HEIGHT);
        rtP2 = LoadRenderTexture(viewW, SCREEN_HEIGHT);
    }

    InitRoad();
    InitPlayers();
    InitSprites();
    InitClouds();
    InitGame();
    InitNPCs(roadPosition);
    InitAudio_Game();
    InitCarSounds();

    while (!WindowShouldClose()) {

        // ============ UPDATE ============
        UpdatePlayers();
        UpdateGameCamera();
        UpdateBiome();
        UpdateGame(players[0].speed);
        UpdateNPCs(roadPosition, players[0].speed);
        UpdateAudio();
        CheckCollisions(roadPosition);

        if (gameState.scoreSaved && IsKeyPressed(KEY_ENTER)) {
            for (int id = 0; id < activePlayerCount; id++)
                players[id].roadPos = 0;
            roadPosition = 0;
            InitGame();
            InitPlayers();
            InitNPCs(roadPosition);
        }

        // ============ DRAW ============
        if (gameMode == MODE_SINGLE) {

            BeginDrawing();
                ClearBackground(BLACK);
                DrawClouds();
                DrawRoad(players[0].roadPos, playersX[0], SCREEN_WIDTH, 0);
                DrawSprites();
                DrawNPCs(roadPosition, playerX);
                DrawPlayer(0, SCREEN_WIDTH, 0);
                DrawHUD(0, SCREEN_WIDTH, 0);
                if (currentBiome == BIOME_TUNEL) {
                    unsigned char alpha = (unsigned char)(biomeTransition * 180);
                    DrawRectangleGradientH(0, 0, SCREEN_WIDTH/3, SCREEN_HEIGHT,
                        (Color){0,0,0,alpha}, (Color){0,0,0,0});
                    DrawRectangleGradientH(SCREEN_WIDTH*2/3, 0, SCREEN_WIDTH/3, SCREEN_HEIGHT,
                        (Color){0,0,0,0}, (Color){0,0,0,alpha});
                }
            EndDrawing();

        } else {

            // --- Câmera P1 ---
            BeginTextureMode(rtP1);
                ClearBackground(BLACK);
                DrawClouds();
                DrawRoad(players[0].roadPos, playersX[0], viewW, 0);
                DrawSprites();
                DrawNPCs(roadPosition, playersX[0]);
                DrawPlayer(0, viewW, 0);
                DrawPlayer(1, viewW, 0);
                DrawHUD(0, viewW, 0);
                if (currentBiome == BIOME_TUNEL) {
                    unsigned char alpha = (unsigned char)(biomeTransition * 180);
                    DrawRectangleGradientH(0, 0, viewW/3, SCREEN_HEIGHT,
                        (Color){0,0,0,alpha}, (Color){0,0,0,0});
                    DrawRectangleGradientH(viewW*2/3, 0, viewW/3, SCREEN_HEIGHT,
                        (Color){0,0,0,0}, (Color){0,0,0,alpha});
                }
            EndTextureMode();

            // --- Câmera P2 ---
            BeginTextureMode(rtP2);
                ClearBackground(BLACK);
                DrawClouds();
                DrawRoad(players[1].roadPos, playersX[1], viewW, 0);
                DrawSprites();
                DrawNPCs(roadPosition, playersX[1]);
                DrawPlayer(1, viewW, 0);
                DrawPlayer(0, viewW, 0);
                DrawHUD(1, viewW, 0);
                if (currentBiome == BIOME_TUNEL) {
                    unsigned char alpha = (unsigned char)(biomeTransition * 180);
                    DrawRectangleGradientH(0, 0, viewW/3, SCREEN_HEIGHT,
                        (Color){0,0,0,alpha}, (Color){0,0,0,0});
                    DrawRectangleGradientH(viewW*2/3, 0, viewW/3, SCREEN_HEIGHT,
                        (Color){0,0,0,0}, (Color){0,0,0,alpha});
                }
            EndTextureMode();

            // --- Composição final ---
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTextureRec(rtP1.texture,
                    (Rectangle){ 0, 0, (float)viewW, -(float)SCREEN_HEIGHT },
                    (Vector2){ 0, 0 }, WHITE);
                DrawTextureRec(rtP2.texture,
                    (Rectangle){ 0, 0, (float)viewW, -(float)SCREEN_HEIGHT },
                    (Vector2){ (float)viewW, 0 }, WHITE);
                DrawRectangle(viewW - 1, 0, 3, SCREEN_HEIGHT, BLACK);
            EndDrawing();
        }
    }

    if (gameMode == MODE_COOP) {
        UnloadRenderTexture(rtP1);
        UnloadRenderTexture(rtP2);
    }
    UnloadSprites();
    UnloadClouds();
    UnloadPlayers();
    UnloadNPCs();
    UnloadAudio_Game();
    CloseWindow();

    return 0;
}