#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
extern float playerX;

typedef enum{
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

    bool onGrass;
    float hitTimer;

} Player;

extern Player player;

void InitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void UnloadPlayer(void);

Rectangle GetPlayerScreenBox(void);
Rectangle GetPlayerHitbox();
bool CheckPlayerOnGrass();

#endif