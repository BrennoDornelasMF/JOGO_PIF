#include "game.h"
#include "raylib.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

GameState gameState;

void InitGame(void) {
    gameState.lives      = 3;
    gameState.score      = 0;
    gameState.scoreTimer = 0.0f;
    gameState.gameOver   = false;
}

void UpdateGame(float playerSpeed) {
    if (gameState.gameOver) return;

    gameState.scoreTimer += GetFrameTime();
    if (gameState.scoreTimer >= 1.0f) {
        gameState.scoreTimer = 0.0f;
        gameState.score += (int)(playerSpeed * 10);
    }
}

void DrawHUD(void) {
    DrawText(TextFormat("SCORE: %d", gameState.score), 10, 10, 24, WHITE);

    // Vidas como [v] já que fonte padrão não suporta ♥
    for (int i = 0; i <= gameState.lives; i++)
        DrawText("[v]", 20 + i * 50, 40, 24, RED);

    if (gameState.gameOver)
        DrawText("GAME OVER", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2, 48, RED);
        
}