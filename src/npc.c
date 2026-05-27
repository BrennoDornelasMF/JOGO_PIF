    #include "npc.h"
    #include "raylib.h"
    #include "road.h"
    #include <stdlib.h>
    #include <math.h>

    // Deve bater com o main.c
    #define SCREEN_WIDTH  1280
    #define SCREEN_HEIGHT 720

    NPC npcs[MAX_NPCS];
    Texture2D npcSprites[NPC_SPRITE_COUNT];

    static float lanes[] = { -0.6f, -0.3f, 0.0f, 0.3f, 0.6f };
    #define LANE_COUNT 5

    void InitNPCs(void) {
        npcSprites[0] = LoadTexture("assets/sprites/npc_green.png");
        npcSprites[1] = LoadTexture("assets/sprites/npc_black.png");

        for (int i = 0; i < MAX_NPCS; i++) {
            npcs[i].active    = true;
            npcs[i].roadPos   = roadPosition + 1.0f + (float)(i * 0.8);
            npcs[i].laneX     = lanes[i % LANE_COUNT];
            npcs[i].speed     = 0.008f;
            npcs[i].spriteIdx = i % NPC_SPRITE_COUNT;
        }
    }

    void UpdateNPCs(float playerRoadPos, float playerSpeed) {
        for (int i = 0; i < MAX_NPCS; i++) {
            if (!npcs[i].active) continue;

            npcs[i].roadPos += playerSpeed * 0.006f;
            // npcs[i].roadPos += npcs[i].speed;

            float diff = npcs[i].roadPos - playerRoadPos;

#ifdef DEBUG_HITBOX
        // Imprime no terminal para ver o que acontece
        if (i == 0)
            TraceLog(LOG_INFO, "NPC0: roadPos=%.3f playerPos=%.3f diff=%.3f speed=%.4f",
                     npcs[i].roadPos, playerRoadPos, diff, npcs[i].speed);
#endif

            if (diff < 0.3f || diff > 8.0f) {
                npcs[i].roadPos   = playerRoadPos + 1.5f + (float)(rand() % 6);
                npcs[i].laneX     = lanes[rand() % LANE_COUNT];
                npcs[i].spriteIdx = rand() % NPC_SPRITE_COUNT;
            }
        }
    }

void DrawNPCs(float playerRoadPos, float camX) {
    int horizon = SCREEN_HEIGHT / 2 + 35;

    for (int i = 0; i < MAX_NPCS; i++) {
        if (!npcs[i].active) continue;

        float diff = npcs[i].roadPos - playerRoadPos;
        if (diff <= 0.0f) continue;  // atrás do player, não desenha

        // diff pequeno = perto = lineF grande (base da tela)
        // diff grande  = longe = lineF pequeno (horizonte)
        // diff esperado: 0.3 a 14.0
        float lineF = 100.0f - (diff / 8.0f) * 90.0f;

        if (lineF < 6.0f || lineF > 99.0f) continue;

        float screenY = SCREEN_HEIGHT - lineF * (SCREEN_HEIGHT - horizon) / 100.0f;
        if (screenY < horizon) continue;

        int   line       = (int)lineF;
        float roadCenter = track[line];
        float dist       = lineToDistance(lineF);
        float width      = (SCREEN_WIDTH * 0.06f) / dist;

        float screenX = roadCenter + npcs[i].laneX * width;

        // escala: perto = grande, longe = pequeno
        float scale = (lineF / 100.0f) * 2.0f;
        if (scale < 0.05f) continue;

        Texture2D tex = npcSprites[npcs[i].spriteIdx];
        float w = tex.width  * scale;
        float h = tex.height * scale;

        DrawTexturePro(
            tex,
            (Rectangle){ 0, 0, tex.width, tex.height },
            (Rectangle){ screenX, screenY, w, h },
            (Vector2){ w / 2, h },
            0.0f,
            WHITE
        );

#ifdef DEBUG_HITBOX
        DrawRectangleLinesEx(
            (Rectangle){ screenX - w/2, screenY - h, w, h },
            2, GREEN
        );
        DrawText(TextFormat("NPC%d diff:%.2f line:%.1f", i, diff, lineF),
                 900, 10 + i * 20, 16, YELLOW);
#endif
    }
}

    void UnloadNPCs(void) {
        for (int i = 0; i < NPC_SPRITE_COUNT; i++)
            UnloadTexture(npcSprites[i]);
    }