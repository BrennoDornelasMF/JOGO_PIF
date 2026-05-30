#ifndef ROAD_H
#define ROAD_H
#include "raylib.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define TRACK_LENGTH  440  // trackData.length * 10
#define BIOME_RURAL 0 //incio do jogo
#define BIOME_TUNEL 1 // tunel, ambiente intermediario
#define BIOME_CITY 2 //cidade à noite - ambiente final

extern float roadPosition;
extern float trackValue[TRACK_LENGTH];
extern float track[100];
extern float turnValue;
extern int trackDataLen;
extern int currentBiome; //ambiente atual (0,1,2) muda conforme o roadpositon 
//progresso de transicao de bioma(0.0 = inicio, 1.0=completo) 
//usado para fazer o fade suave entre os biomas
extern float biomeTransition; 

void InitRoad(void);
void DrawRoad(void);
float readTrack(float x);
float lineToDistance(float line);
float distanceToLine(float distance);
void UpdateBiome(void);


#endif