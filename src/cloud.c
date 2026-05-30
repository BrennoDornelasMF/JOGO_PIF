#include "cloud.h"
#include "road.h"
#include "player.h"
#include "raylib.h"
#include <math.h>

#define MAX_CLOUDS  20
#define CLOUD_TYPES  3

typedef struct { Texture2D tex; } CloudTex;

typedef struct {
    int   typeIdx;
    float x, y;
    float speed;
    float scale;
    float alpha;
} Cloud;

static CloudTex cloudTextures[CLOUD_TYPES];
static Cloud    clouds[MAX_CLOUDS];
static int      cloudTexCount = 0;
static Texture2D bgTexture;
static float     bgOffset = 0.0f;

static int AddCloudTex(const char *path) {
    if (cloudTexCount >= CLOUD_TYPES) return 0;
    cloudTextures[cloudTexCount].tex = LoadTexture(path);
    return cloudTexCount++;
}

void InitClouds(void) {
    int big = AddCloudTex("assets/sprites/cloud_big.png");
    int med = AddCloudTex("assets/sprites/cloud_med.png");
    int sml = AddCloudTex("assets/sprites/cloud_sml.png");

    bgTexture = LoadTexture("assets/sprites/background.png");

    int horizon = SCREEN_HEIGHT / 2 + 35;
    int types[] = { big, med, sml, big, med, sml, big, med,
                    sml, big, med, sml, big, med, sml, big,
                    med, sml, big, med };

    for (int i = 0; i < MAX_CLOUDS; i++) {
        clouds[i].typeIdx = types[i];
        clouds[i].x     = (float)(i * (SCREEN_WIDTH / MAX_CLOUDS))
                          + (float)GetRandomValue(-30, 30);
        clouds[i].y     = (float)GetRandomValue(30, horizon - 50);
        clouds[i].speed = 15.0f + (float)GetRandomValue(0, 40);
        clouds[i].scale = 0.6f  + (float)GetRandomValue(0, 8) * 0.05f;
        clouds[i].alpha = 0.75f + (float)GetRandomValue(0, 5) * 0.05f;
    }
}

void UpdateClouds(void) {
    float dt = GetFrameTime();
    int   sw = SCREEN_WIDTH;

    /* offset do background — parallax mais lento que as nuvens */
    bgOffset -= turnValue * 0.03f * player.speed / 50.0f;
    if (bgOffset >  64.0f) bgOffset -= 64.0f;
    if (bgOffset < -64.0f) bgOffset += 64.0f;

    for (int i = 0; i < MAX_CLOUDS; i++) {
        Texture2D tex = cloudTextures[clouds[i].typeIdx].tex;
        float w = (float)tex.width * clouds[i].scale;

        clouds[i].x -= turnValue * 0.05f * player.speed / 50.0f;
        clouds[i].x += clouds[i].speed * dt;

        if (clouds[i].x > sw + w / 2.0f) clouds[i].x -= sw + w;
        if (clouds[i].x < -w / 2.0f)     clouds[i].x += sw + w;
    }
}

void DrawClouds(void) {
    int horizon = SCREEN_HEIGHT / 2 + 35;

    /* 1 — gradiente do céu */
    
    //cores do ceu rural
    Color skyTop = (Color){155,220,255,255};
    Color skyBottom = (Color){80,170,230,255};

    //tunel escurece gradualmente
    if(currentBiome == BIOME_TUNEL){
        //interpola de azul para o preto conforme o biome|Transition
        skyTop.r = (unsigned char)(155*(1.0f - biomeTransition));
        skyTop.g = (unsigned char)(220*(1.0f - biomeTransition));
        skyTop.b = (unsigned char)(255*(1.0f - biomeTransition));
        skyBottom.r = (unsigned char)(80*(1.0f - biomeTransition));
        skyBottom.g = (unsigned char)(170*(1.0f - biomeTransition));
        skyBottom.b = (unsigned char)(230*(1.0f - biomeTransition));
    }

    //na cidade ceu noturno roxo escuro
    if(currentBiome == BIOME_CITY){
        skyTop = (Color){10,10,30,255};
        skyBottom = (Color){30,10,60,255};
    }

    DrawRectangleGradientV(0,0,SCREEN_WIDTH, horizon, skyTop, skyBottom);

    if (currentBiome != BIOME_TUNEL){
        
    
        /* 2 — faixa de background tileada no horizonte
            ocupa os últimos 48px antes do horizonte */
        int bgH    = 64;
        int bgY    = horizon - bgH;
        int tileW  = bgTexture.width;   /* 64px */
        int tilesN = (SCREEN_WIDTH / tileW) + 2;

        for (int t = -1; t < tilesN; t++) {
            float tx = (float)(t * tileW) + bgOffset;
            DrawTexturePro(
                bgTexture,
                (Rectangle){ 0, 0, (float)tileW, (float)bgTexture.height },
                (Rectangle){ tx, (float)bgY, (float)tileW, (float)bgH },
                (Vector2){ 0, 0 },
                0.0f, WHITE
            );
        }

        /* 3 — nuvens por cima do background */
        for (int i = 0; i < MAX_CLOUDS; i++) {
            Texture2D tex = cloudTextures[clouds[i].typeIdx].tex;
            float w  = (float)tex.width  * clouds[i].scale;
            float h2 = (float)tex.height * clouds[i].scale;

            Color tint = WHITE;
            tint.a = (unsigned char)(clouds[i].alpha * 255);

            DrawTexturePro(
                tex,
                (Rectangle){ 0, 0, (float)tex.width, (float)tex.height },
                (Rectangle){ clouds[i].x - w / 2.0f, clouds[i].y - h2 / 2.0f, w, h2 },
                (Vector2){ 0, 0 },
                0.0f, tint
            );
        }
    }
}

void UnloadClouds(void) {
    for (int i = 0; i < cloudTexCount; i++)
        UnloadTexture(cloudTextures[i].tex);
    UnloadTexture(bgTexture);
}