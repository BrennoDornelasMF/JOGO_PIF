#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define MAX_SCORES 10
#define SCORE_FILE "scores.txt"

// ======================================================
// Modos de jogo — o menu vai setar gameMode antes de
// chamar InitGame() e InitPlayers()
// ======================================================
typedef enum {
    MODE_SINGLE = 0,  // 1 jogador, tela inteira
    MODE_COOP   = 1,  // 2 jogadores, split screen
} GameMode;

extern GameMode gameMode;

typedef struct {
    char name[32];
    int  score;
} ScoreEntry;

typedef struct {
    int   lives;
    int   score;
    float scoreTimer;
    bool  gameOver;

    bool  enteringName;
    bool  scoreSaved;
    char  playerName[32];
    int   nameLen;

    ScoreEntry highscores[MAX_SCORES];
    int        scoreCount;
} GameState;

extern GameState gameState;

void InitGame(void);
void UpdateGame(float playerSpeed);

// viewW   = largura da viewport do jogador (SCREEN_WIDTH ou SCREEN_WIDTH/2)
// offsetX = onde começa horizontalmente na tela
void DrawHUD(int id, int viewW, int offsetX);

#endif