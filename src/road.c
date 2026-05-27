#include "road.h"
#include <math.h>
#include <string.h>
extern float playerX;

float roadPosition = 0;
float trackValue[TRACK_LENGTH];
float track[100];
float turnValue = 0;


static const char* trackData = "===l===r===LR===RR==R=L=L=L======RR=RLR===";
int trackDataLen = 0;

float lineToDistance(float line) {
    return 10.0f / ((100.0f - line) + 1.0f);
}

float distanceToLine(float distance) {
    return 101.0f - 10.0f / distance;
}

float readTrack(float x) {
    int i1 = (int)floorf(x);
    float a = x - i1;
    int i2 = i1 + 1;
    float v1 = trackValue[i1 % trackDataLen];
    float v2 = trackValue[i2 % trackDataLen];
    a = a * a * (3 - 2 * a);  // smoothstep
    return v1 * (1 - a) + v2 * a;
}

void InitRoad(void) {
    trackDataLen = strlen(trackData);

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

    void DrawRoad(void) {
        float off  = 0;
        float off2 = -playerX;
        float tt   = 0;

        int horizon = SCREEN_HEIGHT / 2+35;  // linha do horizonte
        DrawRectangle(0, horizon, SCREEN_WIDTH, SCREEN_HEIGHT - horizon, DARKGREEN);

        for (int i = 5; i < 100; i++) {
            
            // mapeia i (0=horizonte, 100=base) para Y na tela
            float screenY = SCREEN_HEIGHT - (float)i * (SCREEN_HEIGHT - horizon) / 100.0f;
            
            if (screenY < horizon) continue;
            
            float dist  = lineToDistance(i);
            float d     = roadPosition + dist;
            float width = (SCREEN_WIDTH * 0.06f) / dist;
            
            float t = readTrack(d);
            tt   += t * dist * dist * 0.005f;
            off  += tt;
            
            float screenX = SCREEN_WIDTH / 2.0f + off + off2 * (100 - i) / 100.0f;

            // cores alternadas
            int seg = (int)(d * 10);
            Color grassColor = (seg % 10 < 5) ? DARKGREEN : GREEN;
            Color roadColor  = (seg % 10 < 5) ? DARKGRAY  : GRAY;

            float lineH = (SCREEN_HEIGHT - horizon) / 100.0f;

            // grama
            DrawRectangle(0, screenY, SCREEN_WIDTH, lineH + 1, grassColor);

            // pista
            DrawRectangle(screenX - width, screenY, width * 2, lineH + 1, roadColor);

            // bordas
            DrawRectangle(screenX - width,          screenY, width * 0.08f, lineH + 1, WHITE);
            DrawRectangle(screenX + width * 0.92f,  screenY, width * 0.08f, lineH + 1, WHITE);

            // faixa central
            if (seg % 20 < 10)
                DrawRectangle(screenX - width * 0.03f, screenY, width * 0.06f, lineH + 1, YELLOW);

            track[i] = screenX;
            if (i == 0) turnValue = t;
        }
        DrawRectangle(0, horizon, SCREEN_WIDTH, 3, GREEN);
    }   