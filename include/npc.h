#ifndef NPC_H
#define NPC_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_NPCS 5
#define NPC_SPRITE_COUNT 2

typedef struct {
    float roadPos;   // posição na pista (igual ao roadPosition do player)
    float laneX;     // posição lateral lógica (-400 a 400)
    float speed;
    int   spriteIdx; // 0 = verde, 1 = preto
    bool  active;
    bool finished;
} NPC;

extern NPC npcs[MAX_NPCS];
extern Texture2D npcSprites[NPC_SPRITE_COUNT];

void InitNPCs(float playerRoadPos);
void UpdateNPCs(float playerRoadPos, float playerSpeed);
void DrawNPCs(float playerRoadPos, float cameraX);
void UnloadNPCs(void);

#endif