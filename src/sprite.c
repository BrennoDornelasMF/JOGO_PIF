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
static int idxTunnelLamp = -1;
static int idxTunnelEntrance = -1;  

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
    int idxBush  = AddSpriteType("assets/sprites/bush.png",  0.8f, 0.1f);
    int idxRock  = AddSpriteType("assets/sprites/rock_sml.png",  0.6f, 0.0f);
    int idxBirchTree = AddSpriteType("assets/sprites/Birch1.png" 2.0f,0.3f);
    int idxMaple = AddSpriteType("assets\sprites\Maple7.png", 2.0f, 0.3f);
    int idxBanco =AddSpriteType("assets\sprites\banquinho.png", 0.8f,0.0f);
    int idxArbusto = AddSpriteType("assets\sprites\arbusto.png", 0.8f, 0.0f);
    int idxCogumelo = AddSpriteType("assets\sprites\cogumelo.png", 0.5f, 0.0f);
    int idxTronco = AddSpriteType("assets\sprites\tronco.png", 0.5f,0.0f);
    idxTunnelWall = AddSpriteType("assets/sprites/tunnel_wall.png", 0.8f, 0.0f);
    idxTunnelLamp = AddSpriteType("assets/sprites/tunnel_lamp.png", 0.5f, 0.0f);
    idxTunnelEntrance = AddSpriteType("assets/sprites/tunnel_entrance_clean.png", 3.0f, 0.0f);
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

    for(int i=0; i<OBJECTS_PER_SIDE;i++){
        if(GetRandomValue(0,99) < 10)
            leftObjects[i].typeIdx=idxBirchTree;
        if(GetRandomValue(0,99) < 10)
            rightObjects[i].typeIdx=idxBirchTree;
    }

    for(int i=0; i<OBJECTS_PER_SIDE;i++){
        if(GetRandomValue(0,99) < 8)
            leftObjects[i].typeIdx=idxMaple;
        if(GetRandomValue(0,99) < 8)
            rightObjects[i].typeIdx=idxMaple;
    }


    for(int i=0; i<OBJECTS_PER_SIDE;i++){
        if(GetRandomValue(0,99) < 1)
            leftObjects[i].typeIdx=idxBanco;
        if(GetRandomValue(0,99) < 1)
            rightObjects[i].typeIdx=idxBanco;
    }


    for(int i=0; i<OBJECTS_PER_SIDE;i++){
        if(GetRandomValue(0,99) < 6)
            leftObjects[i].typeIdx=idxArbusto;
        if(GetRandomValue(0,99) < 6)
            rightObjects[i].typeIdx=idxArbusto;
    }


    for(int i=0; i<OBJECTS_PER_SIDE;i++){
        if(GetRandomValue(0,99) < 6)
            leftObjects[i].typeIdx=idxCogumelo;
        if(GetRandomValue(0,99) < 6)
            rightObjects[i].typeIdx=idxCogumelo;
    }
    


    for(int i=0; i<OBJECTS_PER_SIDE;i++){
        if(GetRandomValue(0,99) < 6)
            leftObjects[i].typeIdx=idxTronco;
        if(GetRandomValue(0,99) < 6)
            rightObjects[i].typeIdx=idxTronco;
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
        // lâmpadas no teto do túnel
        if (isTunnel && idxTunnelLamp >= 0 && i % 15 == 0) {
            SpriteType *lamp = &spriteTypes[idxTunnelLamp];
            float w  = (float)lamp->tex.width  * (scalePx / 80.0f) * lamp->sizeScale;
            float h2 = (float)lamp->tex.height * (scalePx / 80.0f) * lamp->sizeScale;
            if (w >= 4.0f) {
            // ancora no teto — sprite fica acima do horizonte
                DrawTexturePro(
                    lamp->tex,
                    (Rectangle){ 0, 0, (float)lamp->tex.width, (float)lamp->tex.height },
                    (Rectangle){ px - w / 2.0f, screenY - h2 * 7.0f, w, h2 },
                    (Vector2){ 0, 0 },
                    0.0f, WHITE
                );
            }
        }

        /* --- lado esquerdo --- */
        int leftIdx = isTunnel ? idxTunnelWall:leftObjects[objIdx].typeIdx;
        if (leftIdx >= 0) {
            SpriteType *sp = &spriteTypes[leftIdx];
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
        if (rightIdx >= 0) {
            SpriteType *sp = &spriteTypes[rightIdx];
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
    if (idxTunnelEntrance >= 0) {
        float tunnelStart = 60.0f;
        float d = tunnelStart - roadPosition;  /* distância até a entrada */

        /* só desenha se a entrada ainda estiver à frente do player */
        if (d > 0.0f && d < 10.0f) {
            float line = distanceToLine(d);
            if (line >= 5.0f && line <= 99.0f) {
                int h = (int)line;
                if (h < 5) h = 5;
                if (h > 98) h = 98;
                float px      = track[h];
                float scale_ms = 1000.0f / (d * 120.0f);
                float scalePx  = scale_ms * ((float)sh / 200.0f);
                SpriteType *sp = &spriteTypes[idxTunnelEntrance];
                float w  = (float)sp->tex.width  * (scalePx / 80.0f) * sp->sizeScale;
                float h2 = (float)sp->tex.height * (scalePx / 80.0f) * sp->sizeScale;
                float screenY = (float)sh - line * (float)(sh - horizon) / 100.0f;
                if (w >= 4.0f) {
                    DrawTexturePro(
                        sp->tex,
                        (Rectangle){ 0, 0, (float)sp->tex.width, (float)sp->tex.height },
                        (Rectangle){ px - w / 2.0f, screenY - h2, w, h2 },
                        (Vector2){ 0, 0 },
                        0.0f, WHITE
                    );
                }
            }
        }
    }
} 


/* -----------------------------------------------------------------
   UnloadSprites
----------------------------------------------------------------- */
void UnloadSprites(void) {
    for (int i = 0; i < spriteTypeCount; i++)
        UnloadTexture(spriteTypes[i].tex);
}