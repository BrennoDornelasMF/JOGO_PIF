#ifndef ROAD_H
#define ROAD_H

#include "raylib.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define TRACK_LENGTH  440
#define BIOME_RURAL 0
#define BIOME_TUNEL 1
#define BIOME_CITY  2

extern float roadPosition;       // posição global = P1 (para compatibilidade com NPCs/bioma)
extern float trackValue[TRACK_LENGTH];
extern float track[100];
extern float turnValue;
extern int   trackDataLen;
extern int   currentBiome;
extern float biomeTransition;

void  InitRoad(void);

// roadPos  = posição do jogador naquela câmera
// cameraX  = deslocamento lateral do jogador (playerX)
// viewW    = largura da viewport (SCREEN_WIDTH no single, SCREEN_WIDTH/2 no coop)
// offsetX  = onde a viewport começa na tela (0 no single/P1, viewW no P2)
void  DrawRoad(float roadPos, float cameraX, int viewW, int offsetX);

float readTrack(float x);
float lineToDistance(float line);
float distanceToLine(float distance);
void  UpdateBiome(void);

#endif