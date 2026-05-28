#include "npc.h"
#include "raylib.h"
#include "road.h"
#include <stdlib.h>
#include <math.h>

NPC       npcs[MAX_NPCS];
Texture2D npcSprites[NPC_SPRITE_COUNT];

/* -----------------------------------------------------------------
   IMPORTANTE: track[] no seu road.c já armazena screenX em pixels
   Raylib (0..1280), NÃO em espaço microStudio.

   Por isso DrawNPCs NÃO faz conversão de espaço — usa track[]
   diretamente como X base e calcula Y via a mesma fórmula do DrawRoad.
----------------------------------------------------------------- */

void InitNPCs(float playerRoadPos) {
    npcSprites[0] = LoadTexture("assets/sprites/npc_green.png");
    npcSprites[1] = LoadTexture("assets/sprites/npc_black.png");

    /* laneX: desvio lateral em pixels na linha de base (i=5, perto da tela)
       Valores positivos = direita, negativos = esquerda.
       A escala diminui com a distância, então o efeito visual fica correto. */
    float startX[] = { -120.0f, 120.0f, -60.0f, 60.0f, 0.0f };

    for (int i = 0; i < MAX_NPCS; i++) {
        npcs[i].active    = true;
        npcs[i].finished  = false;
        /* espaça os NPCs à frente do jogador (igual ao exemplo: i * 0.1) */
        npcs[i].roadPos   = playerRoadPos + 0.5f + (float)i * 0.4f;
        npcs[i].laneX     = startX[i];
        npcs[i].speed     = 30.0f + (float)(i * 5);
        npcs[i].spriteIdx = i % NPC_SPRITE_COUNT;
    }
}

void UpdateNPCs(float playerRoadPos, float playerSpeed) {
    (void)playerRoadPos;
    (void)playerSpeed;
    float dt = GetFrameTime();
    for (int i = 0; i < MAX_NPCS; i++) {
        if (!npcs[i].active || npcs[i].finished) continue;
        /* idêntico ao exemplo: c.position += c.speed * dt / 50 */
        npcs[i].roadPos += npcs[i].speed * dt / 50.0f;
        if (npcs[i].roadPos >= (float)trackDataLen)
            npcs[i].roadPos -= (float)trackDataLen;
    }
}

void DrawNPCs(float playerRoadPos, float camX) {
    (void)camX;

    int   sh     = SCREEN_HEIGHT;
    int   sw     = SCREEN_WIDTH;
    int   horizon = sh / 2 + 35;   /* igual ao DrawRoad */

    for (int i = 0; i < MAX_NPCS; i++) {
        if (!npcs[i].active) continue;

        /* distância na pista */
        float d = npcs[i].roadPos - playerRoadPos;
        if (d < -((float)trackDataLen / 2.0f)) d += (float)trackDataLen;
        if (d <= 0.0f) continue;

        /* linha de tela [0..100] equivalente a essa distância */
        float line = distanceToLine(d);

        /* só desenha na faixa visível — começa em 5 igual ao DrawRoad */
        if (line < 5.0f || line > 99.0f) continue;

        /* scale no espaço microStudio (igual ao exemplo: 1000 / d*120) */
        float scale_ms = 1000.0f / (d * 120.0f);

        /* interpolação em track[] — já em pixels Raylib */
        int   h = (int)line;
        float a = line - (float)h;
        if (h < 5)  h = 5;   /* track[0..4] não são preenchidos */
        if (h > 98) h = 98;
        float px = track[h] * (1.0f - a) + track[h + 1] * a;
        /* px == screenX da linha central da pista nessa profundidade */

        /* desvio lateral: scale_ms diminui com a distância → perspectiva correta
           o fator 0.012 vem do exemplo; aqui multiplicamos por sw/400 para
           converter do espaço lógico microStudio (±200) para pixels Raylib */
        float laneOffset = npcs[i].laneX * scale_ms * 0.012f * (sw / 400.0f);
        float screenX    = px + laneOffset;

        /* Y: mesma fórmula do DrawRoad
             screenY = sh - line * (sh - horizon) / 100 */
        float screenY = (float)sh - line * (float)(sh - horizon) / 100.0f;

        /* tamanho do sprite — scale_ms está em espaço microStudio (tela 200px),
           converte para pixels Raylib com sh/200 */
        float scalePx = scale_ms * ((float)sh / 200.0f);
        Texture2D tex = npcSprites[npcs[i].spriteIdx];

        #define NPC_SCALE 0.7f //quanto maior o numero maior os npcs 
        float w  = (float)tex.width  * (scalePx / 80.0f) * NPC_SCALE; 
        float h2 = (float)tex.height * (scalePx / 80.0f) * NPC_SCALE;

        if (w < 4.0f) continue;

        /* ancora o sprite na base (rodas no chão) */
        DrawTexturePro(
            tex,
            (Rectangle){ 0, 0, (float)tex.width, (float)tex.height },
            (Rectangle){ screenX - w / 2.0f, screenY - h2, w, h2 },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
    }
}

void UnloadNPCs(void) {
    for (int i = 0; i < NPC_SPRITE_COUNT; i++)
        UnloadTexture(npcSprites[i]);
}