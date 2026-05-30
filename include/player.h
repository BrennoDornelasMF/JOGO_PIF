#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_PLAYERS 2

typedef enum {
    CAR_STRAIGT = 0,
    CAR_LEFT,
    CAR_RIGHT,
    CAR_SPRITE_COUNT
} CarSpriteState;

typedef struct {
    float x;
    float speed;

    Texture2D sprites[CAR_SPRITE_COUNT];
    CarSpriteState currentSprite;

    float hitboxWidth;
    float hitboxHeight;

    bool  onGrass;
    float hitTimer;

    // posição própria na pista (independente por jogador no coop)
    float roadPos;
} Player;

// Modo single player: usa players[0]
// Modo coop:          usa players[0] e players[1]
extern Player players[MAX_PLAYERS];

// Atalho de compatibilidade — aponta para players[0]
#define player players[0]

// posição lateral na tela de cada jogador
extern float playersX[MAX_PLAYERS];

// Atalho de compatibilidade — aponta para playersX[0]
#define playerX playersX[0]

// quantos jogadores ativos (1 ou 2) — definido pelo menu
extern int activePlayerCount;

void InitPlayers(void);
void UpdatePlayers(void);

// renderiza o jogador id dentro de uma viewport de largura viewW
// offsetX é a posição X onde essa metade da tela começa (0 para P1, viewW para P2)
void DrawPlayer(int id, int viewW, int offsetX);

void UnloadPlayers(void);

Rectangle GetPlayerScreenBox(int id, int viewW);
bool CheckPlayerOnGrass(int id);

#endif