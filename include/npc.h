#ifndef NPC_H
#define NPC_H

#include "raylib.h"
#include <stdbool.h>

#define LANE_COUNT 5
#define SEGMENTS 50
#define NPC_SPRITE_COUNT 2

//#define MAX_NPCS 10

typedef struct {
    float roadPos;   // posição na pista (igual ao roadPosition do player)
    float laneX;     // posição lateral lógica (-400 a 400)
    float speed;
    int   spriteIdx; // 0 = verde, 1 = preto
    int   laneIdx;
    int   segIdx;
    bool  active;
  //  bool finished;
    Rectangle screenBox;
} NPC;

typedef struct NPCNode {
    NPC data;
    struct NPCNode* next;
    
}NPCNode;


extern NPCNode* npcList;
extern Texture2D npcSprites[NPC_SPRITE_COUNT];

void InitNPCs(float playerRoadPos);
void UpdateNPCs(float playerRoadPos, float playerSpeed);
void DrawNPCs(float playerRoadPos, float cameraX);
void UnloadNPCs(void);

#endif