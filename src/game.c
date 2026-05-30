#include "game.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include "player.h"

#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  720
#define MAX_SPEED_KMH  270.f
#define NEEDLE_START  -135.f
#define NEEDLE_SWEEP   260.f

static float     needleAngle = -220.f;
static Texture2D speedoBg;
static Texture2D speedoNeedle;

GameState gameState;
GameMode  gameMode = MODE_SINGLE; // padrão; o menu vai alterar

// ======================================================
// Highscores
// ======================================================

static void LoadHighscores(void) {
    gameState.scoreCount = 0;
    FILE* f = fopen(SCORE_FILE, "r");
    if (!f) return;
    while (gameState.scoreCount < MAX_SCORES) {
        ScoreEntry* e = &gameState.highscores[gameState.scoreCount];
        if (fscanf(f, "%31s %d", e->name, &e->score) != 2) break;
        gameState.scoreCount++;
    }
    fclose(f);
}

static void SaveHighscores(void) {
    FILE* f = fopen(SCORE_FILE, "w");
    if (!f) return;
    for (int i = 0; i < gameState.scoreCount; i++)
        fprintf(f, "%s %d\n", gameState.highscores[i].name,
                              gameState.highscores[i].score);
    fclose(f);
}

static void TryInsertScore(const char* name, int score) {
    int count = gameState.scoreCount;
    if (count >= MAX_SCORES && score <= gameState.highscores[MAX_SCORES-1].score)
        return;

    int pos = (count < MAX_SCORES) ? count : MAX_SCORES - 1;
    for (int i = 0; i < count && i < MAX_SCORES; i++) {
        if (score > gameState.highscores[i].score) { pos = i; break; }
    }

    int limit = (count < MAX_SCORES) ? count : MAX_SCORES - 1;
    for (int i = limit; i > pos; i--)
        gameState.highscores[i] = gameState.highscores[i-1];

    strncpy(gameState.highscores[pos].name, name, 31);
    gameState.highscores[pos].name[31] = '\0';
    gameState.highscores[pos].score    = score;

    if (gameState.scoreCount < MAX_SCORES) gameState.scoreCount++;
    SaveHighscores();
}

static void DrawHighscores(void) {
    DrawRectangle(SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 260,
                  440, 520, Fade(BLACK, 0.88f));
    DrawText("TOP 10", SCREEN_WIDTH/2 - 55, SCREEN_HEIGHT/2 - 240, 32, YELLOW);

    if (gameState.scoreCount == 0) {
        DrawText("Nenhuma pontuacao ainda",
                 SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 190, 20, GRAY);
        return;
    }

    for (int i = 0; i < gameState.scoreCount; i++) {
        Color c = (i == 0) ? GOLD :
                  (i == 1) ? LIGHTGRAY :
                  (i == 2) ? ORANGE : WHITE;
        DrawText(TextFormat("%2d. %-14s %d",
                            i+1,
                            gameState.highscores[i].name,
                            gameState.highscores[i].score),
                 SCREEN_WIDTH/2 - 190,
                 SCREEN_HEIGHT/2 - 190 + i * 40,
                 22, c);
    }
    DrawText("ENTER para reiniciar  ESC para sair",
             SCREEN_WIDTH/2 - 190, SCREEN_HEIGHT/2 + 230, 18, GRAY);
}

// ======================================================
// Init / Update
// ======================================================

void InitGame(void) {
    gameState.lives         = 3;
    gameState.score         = 0;
    gameState.scoreTimer    = 0.0f;
    gameState.gameOver      = false;
    gameState.enteringName  = false;
    gameState.scoreSaved    = false;
    gameState.nameLen       = 0;
    gameState.playerName[0] = '\0';
    speedoBg     = LoadTexture("assets/sprites/speedo_bg.png");
    speedoNeedle = LoadTexture("assets/sprites/speedo_needle.png");
    LoadHighscores();
}

void UpdateGame(float playerSpeed) {
    if (gameState.gameOver) {
        if (!gameState.scoreSaved && !gameState.enteringName)
            gameState.enteringName = true;

        if (gameState.enteringName) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125 && gameState.nameLen < 31) {
                    gameState.playerName[gameState.nameLen++] = (char)key;
                    gameState.playerName[gameState.nameLen]   = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && gameState.nameLen > 0)
                gameState.playerName[--gameState.nameLen] = '\0';

            if (IsKeyPressed(KEY_ENTER) && gameState.nameLen > 0) {
                TryInsertScore(gameState.playerName, gameState.score);
                gameState.scoreSaved   = true;
                gameState.enteringName = false;
            }
        }
        return;
    }

    gameState.scoreTimer += GetFrameTime();

    float fraction = (playerSpeed * 100.0f) / MAX_SPEED_KMH;
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;

    float targetAngle = NEEDLE_START + fraction * NEEDLE_SWEEP;
    needleAngle += (targetAngle - needleAngle) * 0.12f;

    if (gameState.scoreTimer >= 1.0f) {
        gameState.scoreTimer = 0.0f;
        gameState.score += (int)(playerSpeed * 10);
    }
}

// ======================================================
// HUD — parametrizado por viewport
// id      = índice do jogador (0 ou 1)
// viewW   = largura da viewport
// offsetX = início horizontal da viewport na tela
// ======================================================

static void DrawSpeedo(int id, int viewW, int offsetX) {
    float cx = offsetX + viewW - 90.0f;
    float cy = SCREEN_HEIGHT - 90.0f;

    DrawTexturePro(
        speedoBg,
        (Rectangle){0, 0, (float)speedoBg.width, (float)speedoBg.height},
        (Rectangle){cx - 70, cy - 70, 140, 140},
        (Vector2){0, 0}, 0.0f, WHITE
    );
    DrawTexturePro(
        speedoNeedle,
        (Rectangle){0, 0, (float)speedoNeedle.width, (float)speedoNeedle.height},
        (Rectangle){cx, cy, 140, 140},
        (Vector2){70, 70}, needleAngle, WHITE
    );

    int kmh = (int)(players[id].speed * 100.0f);
    DrawText(TextFormat("%d", kmh), cx - 15, cy + 30, 18, WHITE);
}

void DrawHUD(int id, int viewW, int offsetX) {
    DrawSpeedo(id, viewW, offsetX);

    DrawText(TextFormat("SCORE: %d", gameState.score),
             offsetX + 20, 10, 24, WHITE);

    for (int i = 0; i < gameState.lives; i++)
        DrawText("[v]", offsetX + 20 + i * 50, 40, 24, RED);

    // Label do jogador no coop
    if (gameMode == MODE_COOP) {
        const char* label = (id == 0) ? "P1" : "P2";
        DrawText(label, offsetX + viewW / 2 - 12, 8, 22,
                 id == 0 ? WHITE : SKYBLUE);
    }

    if (!gameState.gameOver) return;

    // Game Over só no P1 (tela central)
    if (id != 0) return;

    if (gameState.enteringName) {
        DrawRectangle(SCREEN_WIDTH/2 - 240, SCREEN_HEIGHT/2 - 100,
                      480, 200, Fade(BLACK, 0.90f));
        DrawText("GAME OVER",
                 SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 - 85, 36, RED);
        DrawText("Digite seu nome:",
                 SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 30, 22, WHITE);
        DrawText(TextFormat("%s_", gameState.playerName),
                 SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 10, 28, YELLOW);
        DrawText("ENTER para confirmar",
                 SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 60, 18, GRAY);
    } else if (gameState.scoreSaved) {
        DrawHighscores();
    }
}