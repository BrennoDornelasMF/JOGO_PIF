#include "road.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

float roadPosition = 0;
float trackValue[TRACK_LENGTH];
float track[100];
float turnValue = 0;
int   currentBiome    = BIOME_RURAL;
float biomeTransition = 0.0f;

static const char* trackData =
    "=============lll===rrr==="
    "rrRRRRRrr=========="
    "llLLll===rrRRrr====="
    "lllllLLLLLLLlllll=========="
    "ll==rr==ll==rr==LL==RR==LL==RR====="
    "rrRRRRRRRrr===llLLLLLLLLLll==="
    "=========================";

int trackDataLen = 0;

float lineToDistance(float line) {
    return 10.0f / ((100.0f - line) + 1.0f);
}

float distanceToLine(float distance) {
    return 101.0f - 10.0f / distance;
}

float readTrack(float x) {
    int   i1 = (int)floorf(x);
    float a  = x - i1;
    int   i2 = i1 + 1;
    float v1 = trackValue[i1 % trackDataLen];
    float v2 = trackValue[i2 % trackDataLen];
    a = a * a * (3 - 2 * a);
    return v1 * (1 - a) + v2 * a;
}

void InitRoad(void) {
    trackDataLen = strlen(trackData);
    printf("trackdatalen: %d\n", trackDataLen);
    for (int i = 0; i < trackDataLen; i++) {
        switch (trackData[i]) {
            case 'l': trackValue[i] = -150; break;
            case 'L': trackValue[i] = -300; break;
            case 'r': trackValue[i] =  150; break;
            case 'R': trackValue[i] =  300; break;
            default:  trackValue[i] =    0; break;
        }
    }
}

void UpdateBiome(void) {
    float dt = GetFrameTime();

    int targetBiome;
    if      (roadPosition <  60.0f) targetBiome = BIOME_RURAL;
    else if (roadPosition < 120.0f) targetBiome = BIOME_TUNEL;
    else                            targetBiome = BIOME_CITY;

    if (targetBiome != currentBiome) {
        currentBiome    = targetBiome;
        biomeTransition = 0.0f;
    }

    if (biomeTransition < 1.0f) biomeTransition += dt * 0.5f;
    if (biomeTransition > 1.0f) biomeTransition  = 1.0f;
}

void DrawRoad(float roadPos, float cameraX, int viewW, int offsetX) {
    float off  = 0;
    float off2 = -cameraX;
    float tt   = 0;

    int horizon = SCREEN_HEIGHT / 2 + 35;

    // ceu / fundo superior
    DrawRectangle(offsetX, 0, viewW, horizon, BLACK);

    // chao
    DrawRectangle(offsetX, horizon, viewW, SCREEN_HEIGHT - horizon,
        currentBiome == BIOME_TUNEL ? (Color){30,30,35,255} : DARKGREEN);

    for (int i = 5; i < 100; i++) {
        float screenY = SCREEN_HEIGHT - (float)i * (SCREEN_HEIGHT - horizon) / 100.0f;
        if (screenY < horizon) continue;

        float dist = lineToDistance(i);
        float d    = roadPos + dist;

        // SEMPRE usa SCREEN_WIDTH para manter a largura original da pista
        float width = (SCREEN_WIDTH * 0.06f) / dist;

        float t = readTrack(d);
        tt   += t * dist * dist * 0.005f;
        off  += tt;

        float localX  = (float)viewW / 2.0f + off + off2 * (100 - i) / 100.0f;
        float screenX = offsetX + localX;

        int   seg = (int)(d * 10);
        Color grassColor, roadColor;

        if (currentBiome == BIOME_TUNEL) {
            grassColor = (Color){ 50, 50, 55, 255 };
            roadColor  = (Color){ 40, 40, 45, 255 };
        } else {
            grassColor = (seg % 10 < 5) ? DARKGREEN : GREEN;
            roadColor  = (seg % 10 < 5) ? DARKGRAY  : GRAY;
        }

        float lineH = (SCREEN_HEIGHT - horizon) / 100.0f;

        DrawRectangle(offsetX,          screenY, viewW,      lineH + 1, grassColor);
        DrawRectangle(screenX - width,  screenY, width * 2,  lineH + 1, roadColor);
        DrawRectangle(screenX - width,          screenY, width * 0.08f, lineH + 1, WHITE);
        DrawRectangle(screenX + width * 0.92f,  screenY, width * 0.08f, lineH + 1, WHITE);

        if (seg % 20 < 10)
            DrawRectangle(screenX - width * 0.03f, screenY, width * 0.06f, lineH + 1, YELLOW);

        if (offsetX == 0) {
            track[i] = screenX;
            if (i == 0) turnValue = t;
        }
    }

    // teto do tunel
    if (currentBiome == BIOME_TUNEL) {
        DrawRectangle(offsetX, 0, viewW, horizon, (Color){30, 30, 35, 255});

        for (int i = 5; i < 100; i += 8) {
            float line  = (float)i;
            float vigaH = 4.0f + (float)i * 0.3f;
            float width = (SCREEN_WIDTH * 0.06f) / lineToDistance(line);
            float px    = (offsetX == 0) ? track[i]
                                         : offsetX + (float)viewW / 2.0f;
            float vigaW = width * 2.5f;
            DrawRectangle(px - vigaW / 2, horizon - (int)vigaH,
                          vigaW, vigaH, (Color){70, 70, 80, 255});
        }
    }

    DrawRectangle(offsetX, horizon, viewW, 3,
        currentBiome == BIOME_TUNEL ? (Color){50,50,55,255} : GREEN);
}