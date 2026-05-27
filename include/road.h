#ifndef ROAD_H
#define ROAD_H
#include "raylib.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define TRACK_LENGTH  440  // trackData.length * 10

extern float roadPosition;
extern float trackValue[TRACK_LENGTH];
extern float track[100];
extern float turnValue;
extern int trackDataLen;

void InitRoad(void);
void DrawRoad(void);
float readTrack(float x);
float lineToDistance(float line);
float distanceToLine(float distance);


#endif