#include "npc.h"
#include "road.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define MAX_NPCS_ALIVE 10

Texture2D npcSprites[NPC_SPRITE_COUNT];
NPCNode*  npcList = NULL;
int       laneMap[SEGMENTS][LANE_COUNT];

static float lanePositions[LANE_COUNT] = { -120.0f, -60.0f, 0.0f, 60.0f, 120.0f };

// ================================
// MATRIZ — ocupação de faixas
// ================================

static void LaneMapClear(void) {
    for (int s = 0; s < SEGMENTS; s++)
        for (int l = 0; l < LANE_COUNT; l++)
            laneMap[s][l] = 0;
}

static int RoadPosToSeg(float roadPos) {
    int seg = (int)(roadPos * (SEGMENTS / (float)trackDataLen));
    if (seg < 0)         seg = 0;
    if (seg >= SEGMENTS) seg = SEGMENTS - 1;
    return seg;
}

static bool LaneIsFree(int seg, int lane) {
    return laneMap[seg][lane] == 0;
}

static void LaneOccupy(int seg, int lane) {
    laneMap[seg][lane] = 1;
}

static void LaneFree(int seg, int lane) {
    laneMap[seg][lane] = 0;
}

// ================================
// LISTA — spawn e remoção
// ================================

static int CountNPCs(void) {
    int count = 0;
    for (NPCNode* n = npcList; n != NULL; n = n->next)
        count++;
    return count;
}

static void SpawnNPC(float playerRoadPos) {
    for (int attempt = 0; attempt < 20; attempt++) {
        float roadPos = playerRoadPos + 1.5f + (float)(rand() % 10);
        int   lane    = rand() % LANE_COUNT;
        int   seg     = RoadPosToSeg(roadPos);

        if (!LaneIsFree(seg, lane)) continue;

        NPCNode* node = (NPCNode*)malloc(sizeof(NPCNode));
        if (!node) return;

        node->data.roadPos   = roadPos;
        node->data.laneX     = lanePositions[lane];
        node->data.laneIdx   = lane;
        node->data.segIdx    = seg;
        node->data.speed     = 25.0f + (float)(rand() % 30);
        node->data.spriteIdx = rand() % NPC_SPRITE_COUNT;
        node->data.active    = true;
        node->data.screenBox = (Rectangle){ 0, 0, 0, 0 };
        node->next           = npcList;
        npcList              = node;

        LaneOccupy(seg, lane);
        return;
    }
}

static void RemoveNPC(NPCNode** prev, NPCNode* node) {
    LaneFree(node->data.segIdx, node->data.laneIdx);
    *prev = node->next;
    free(node);
}

// ================================
// LIFECYCLE
// ================================

void InitNPCs(float playerRoadPos) {
    while (npcList != NULL) {
        NPCNode* tmp = npcList;
        npcList = npcList->next;
        free(tmp);
    }

    npcSprites[0] = LoadTexture("assets/sprites/npc_green.png");
    npcSprites[1] = LoadTexture("assets/sprites/npc_black.png");

    LaneMapClear();

    for (int i = 0; i < MAX_NPCS_ALIVE; i++)
        SpawnNPC(playerRoadPos + (float)i * 0.5f);
}

void UpdateNPCs(float playerRoadPos, float playerSpeed) {
    (void)playerSpeed;
    float dt = GetFrameTime();

    NPCNode** prev = &npcList;
    NPCNode*  node = npcList;

    while (node != NULL) {
        node->data.screenBox = (Rectangle){ 0, 0, 0, 0 };

        node->data.roadPos += node->data.speed * dt / 50.0f;

        if (node->data.roadPos >= (float)trackDataLen)
            node->data.roadPos -= (float)trackDataLen;

        // Atualiza segmento na matriz
        int newSeg = RoadPosToSeg(node->data.roadPos);
        if (newSeg != node->data.segIdx) {
            LaneFree(node->data.segIdx, node->data.laneIdx);

            if (LaneIsFree(newSeg, node->data.laneIdx)) {
                node->data.segIdx = newSeg;
                LaneOccupy(newSeg, node->data.laneIdx);
            } else {
                bool moved = false;
                for (int l = 0; l < LANE_COUNT; l++) {
                    if (LaneIsFree(newSeg, l)) {
                        node->data.laneIdx = l;
                        node->data.laneX   = lanePositions[l];
                        node->data.segIdx  = newSeg;
                        LaneOccupy(newSeg, l);
                        moved = true;
                        break;
                    }
                }
                if (!moved) {
                    node->data.roadPos += 1.0f;
                    node->data.segIdx   = RoadPosToSeg(node->data.roadPos);
                    LaneOccupy(node->data.segIdx, node->data.laneIdx);
                }
            }
        }

        float d = node->data.roadPos - playerRoadPos;
        if (d < -((float)trackDataLen / 2.0f)) d += (float)trackDataLen;

        if (d < -2.0f) {
            NPCNode* toRemove = node;
            node = node->next;
            RemoveNPC(prev, toRemove);
            continue;
        }

        prev = &node->next;
        node = node->next;
    }

    while (CountNPCs() < MAX_NPCS_ALIVE)
        SpawnNPC(playerRoadPos);
}

void DrawNPCs(float playerRoadPos, float camX) {
    (void)camX;

    int sh      = SCREEN_HEIGHT;
    int sw      = SCREEN_WIDTH;
    int horizon = sh / 2 + 35;

    for (NPCNode* node = npcList; node != NULL; node = node->next) {
        NPC* npc = &node->data;
        if (!npc->active) continue;

        float d = npc->roadPos - playerRoadPos;
        if (d < -((float)trackDataLen / 2.0f)) d += (float)trackDataLen;
        if (d <= 0.0f) continue;

        float line = distanceToLine(d);
        if (line < 5.0f || line > 99.0f) continue;

        float scale_ms = 1000.0f / (d * 120.0f);

        int   h = (int)line;
        float a = line - (float)h;
        if (h < 5)  h = 5;
        if (h > 98) h = 98;
        float px = track[h] * (1.0f - a) + track[h + 1] * a;

        float laneOffset = npc->laneX * scale_ms * 0.012f * (sw / 400.0f);
        float screenX    = px + laneOffset;
        float screenY    = (float)sh - line * (float)(sh - horizon) / 100.0f;

        if (screenY < horizon) continue;

        float scalePx = scale_ms * ((float)sh / 200.0f);
        Texture2D tex = npcSprites[npc->spriteIdx];

        float w  = (float)tex.width  * (scalePx / 80.0f) * 0.7f;
        float h2 = (float)tex.height * (scalePx / 80.0f) * 0.7f;

        if (w < 4.0f) continue;

        npc->screenBox = (Rectangle){ screenX - w/2.0f, screenY - h2, w, h2 };

        DrawTexturePro(
            tex,
            (Rectangle){ 0, 0, (float)tex.width, (float)tex.height },
            npc->screenBox,
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );

#ifdef DEBUG_HITBOX
        DrawRectangleLinesEx(npc->screenBox, 2, GREEN);
        DrawText(TextFormat("d:%.2f l:%.1f", d, line),
                 screenX - w/2.0f, screenY - h2 - 16, 14, GREEN);
#endif
    }
}

void UnloadNPCs(void) {
    while (npcList != NULL) {
        NPCNode* tmp = npcList;
        npcList = npcList->next;
        free(tmp);
    }
    for (int i = 0; i < NPC_SPRITE_COUNT; i++)
        UnloadTexture(npcSprites[i]);
}