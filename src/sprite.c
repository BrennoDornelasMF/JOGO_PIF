#include "sprite.h"
#include "road.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

/* -----------------------------------------------------------------
   Sistema de objetos laterais — mesma lógica do drawSideObjects()
   do exemplo microStudio, convertendo para o espaço Raylib do
   road.c (track[] já em pixels, screenY pela fórmula do DrawRoad).
----------------------------------------------------------------- */

#define MAX_SPRITE_TYPES  8
#define OBJECTS_PER_SIDE  (TRACK_LENGTH * 10)  /* densidade ao longo da pista */

/* --- tipos de sprite disponíveis -------------------------------- */
typedef struct {
    Texture2D tex;
    float     sizeScale;  /* multiplicador de tamanho (1.0 = normal) */
    float     offset;     /* quanto afasta da borda da pista (0..2)  */
} SpriteType;

static SpriteType spriteTypes[MAX_SPRITE_TYPES];
static int spriteTypeCount = 0;
static int idxTunnelWall = -1;        

/* --- objetos fixos em cada lado da pista ----------------------- */
typedef struct {
    int   typeIdx;   /* índice em spriteTypes; -1 = vazio */
} SideObject;

static SideObject leftObjects[OBJECTS_PER_SIDE];
static SideObject rightObjects[OBJECTS_PER_SIDE];

/* -----------------------------------------------------------------
   Utilitário: registra um tipo de sprite
----------------------------------------------------------------- */
static int AddSpriteType(const char *path, float sizeScale, float offset) {
    if (spriteTypeCount >= MAX_SPRITE_TYPES) return -1;
    int idx = spriteTypeCount++;
    spriteTypes[idx].tex       = LoadTexture(path);
    spriteTypes[idx].sizeScale = sizeScale;
    spriteTypes[idx].offset    = offset;
    return idx;
}

/* -----------------------------------------------------------------
   InitSprites — carrega texturas e distribui objetos pela pista
----------------------------------------------------------------- */
void InitSprites(void) {
    /* registra os tipos disponíveis */
    int idxTree = AddSpriteType("assets/sprites/tree.png", 2.0f, 0.3f);
    /* quando tiver mais sprites, adicione aqui:
    int idxPalm  = AddSpriteType("assets/sprites/palm_tree.png", 1.8f, 0.4f); */

    int idxBush  = AddSpriteType("assets/sprites/bush.png",  0.8f, 0.1f);
    int idxRock  = AddSpriteType("assets/sprites/rock_sml.png",  0.6f, 0.0f);
    int idxTunnelWall = AddSpriteType("assets/sprites/tunnel_wall.png", 1.5f,0.0f);
    idxTunnelWall = AddSpriteType("assets/sprites/tunnel_wall.png", 1.5f, 0.0f);
    printf("idxTunnelWall: %d w:%d h:%d\n", 
       idxTunnelWall, 
       spriteTypes[idxTunnelWall].tex.width,
       spriteTypes[idxTunnelWall].tex.height); 
       
    /* inicializa tudo vazio */
    for (int i = 0; i < OBJECTS_PER_SIDE; i++) {
        leftObjects[i].typeIdx  = -1;
        rightObjects[i].typeIdx = -1;
    }

    /* distribui aleatoriamente — ~10% de chance em cada slot */
    for (int i = 0; i < OBJECTS_PER_SIDE; i++) {
        if (GetRandomValue(0, 99) < 12)
            leftObjects[i].typeIdx  = idxTree;
        if (GetRandomValue(0, 99) < 12)
            rightObjects[i].typeIdx = idxTree;
    }

    for(int i =0; i<OBJECTS_PER_SIDE;i++){
        if(GetRandomValue(0,99) < 2)
            leftObjects[i].typeIdx=idxRock;
        if(GetRandomValue(0,99) < 2)
            rightObjects[i].typeIdx=idxRock;
    }

    for(int i=0; i<OBJECTS_PER_SIDE;i++){
        if(GetRandomValue(0,99) < 10)
            leftObjects[i].typeIdx=idxBush;
        if(GetRandomValue(0,99) < 10)
            rightObjects[i].typeIdx=idxBush;
    }

    printf("idxtunelwall: %d\n",idxTunnelWall);
    printf("spritetypecount: %d\n", spriteTypeCount);
}

/* -----------------------------------------------------------------
   DrawSprites — renderiza objetos de trás para frente (i alto → baixo)
   para que objetos próximos fiquem por cima dos distantes.

   Usa exatamente o mesmo espaço de coordenadas do DrawNPCs:
     - track[h]  → screenX central da pista nessa profundidade
     - screenY   → sh - line * (sh - horizon) / 100
     - scale_ms  → 1000 / (d * 120)   (espaço microStudio)
     - scalePx   → scale_ms * (sh / 200)
----------------------------------------------------------------- */
void DrawSprites(void) {
    int   sh     = SCREEN_HEIGHT;
    int   sw     = SCREEN_WIDTH;
    int   horizon = sh / 2 + 35;
    
    //no tunel desenha as paredes
    int isTunnel = (currentBiome == BIOME_TUNEL);

    /* de trás para frente: line pequeno = longe, line grande = perto */
    for (int i = 98; i >= 5; i--) {
        float line = (float)i;
        float d    = lineToDistance(line);   /* distância na pista */

        /* posição absoluta na pista */
        int segmento = (int)floorf(roadPosition * 10.0f) + i;

        //calculo do warap-around
        int objIdx = ((segmento % OBJECTS_PER_SIDE) + OBJECTS_PER_SIDE) % OBJECTS_PER_SIDE;
       
        /* interpolação em track[] para o X central da pista */
        int   h = i < 5 ? 5 : (i > 98 ? 98 : i);
        float px = track[h];   /* screenX em pixels Raylib */

        /* scale no espaço microStudio */
        float scale_ms = 1000.0f / (d * 120.0f);
        /* largura lógica da pista nessa profundidade (igual ao DrawRoad) */
        float roadHalfWidth = (sw * 0.06f) / d;

        /* screenY igual ao DrawRoad e DrawNPCs */
        float screenY = (float)sh - line * (float)(sh - horizon) / 100.0f;

        /* fator de conversão microStudio → Raylib */
        float scalePx = scale_ms * ((float)sh / 200.0f);

        /* --- lado esquerdo --- */
        int leftIdx = isTunnel ? idxTunnelWall:leftObjects[objIdx].typeIdx;
        if (leftObjects[objIdx].typeIdx >= 0) {
            SpriteType *sp = &spriteTypes[leftObjects[objIdx].typeIdx];
            float w  = (float)sp->tex.width  * (scalePx / 80.0f) * sp->sizeScale;
            float h2 = (float)sp->tex.height * (scalePx / 80.0f) * sp->sizeScale;
            if (w >= 4.0f) {
                /* posiciona à esquerda da pista + offset de afastamento */
                float screenX = px - roadHalfWidth - w * sp->offset - w * 0.5f;
                DrawTexturePro(
                    sp->tex,
                    (Rectangle){ 0, 0, (float)sp->tex.width, (float)sp->tex.height },
                    (Rectangle){ screenX - w / 2.0f, screenY - h2, w, h2 },
                    (Vector2){ 0, 0 },
                    0.0f, WHITE
                );
            }
        }

        /* --- lado direito --- */
        int rightIdx = isTunnel ? idxTunnelWall : rightObjects[objIdx].typeIdx;
        if (rightObjects[objIdx].typeIdx >= 0) {
            SpriteType *sp = &spriteTypes[rightObjects[objIdx].typeIdx];
            float w  = (float)sp->tex.width  * (scalePx / 80.0f) * sp->sizeScale;
            float h2 = (float)sp->tex.height * (scalePx / 80.0f) * sp->sizeScale;
            if (w >= 4.0f) {
                float screenX = px + roadHalfWidth + w * sp->offset + w * 0.5f;
                DrawTexturePro(
                    sp->tex,
                    (Rectangle){ 0, 0, (float)sp->tex.width, (float)sp->tex.height },
                    (Rectangle){ screenX - w / 2.0f, screenY - h2, w, h2 },
                    (Vector2){ 0, 0 },
                    0.0f, WHITE
                );
            }
        }
    }
    printf("istunel: %d currentbiome: %d\n", isTunnel, currentBiome);
}

/* -----------------------------------------------------------------
   UnloadSprites
----------------------------------------------------------------- */
void UnloadSprites(void) {
    for (int i = 0; i < spriteTypeCount; i++)
        UnloadTexture(spriteTypes[i].tex);
}