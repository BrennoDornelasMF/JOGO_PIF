#ifndef GAME_H
#define GAME_H

#include <stdbool.h>


typedef struct {
    int   lives;
    int   score;
    float scoreTimer;   // acumula tempo para dar pontos
    bool  gameOver;
    bool playerFinished;
} GameState;

extern GameState gameState;

void InitGame(void);
void UpdateGame(float playerSpeed);
void DrawHUD(void);

#endif