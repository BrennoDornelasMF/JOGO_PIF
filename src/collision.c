#include "npc.h"
#include "game.h"
#include "player.h"
#include "road.h"
#include "raylib.h"
#include <math.h>

void CheckCollisions(float playerRoadPos) {
    (void)playerRoadPos;

    if (player.hitTimer > 0) return;

    Rectangle playerBox = GetPlayerScreenBox();

    for (int i = 0; i < MAX_NPCS; i++) {
        if (!npcs[i].active) continue;

        // NPC só tem hitbox válida se foi desenhado nesse frame
        if (npcs[i].screenBox.width < 4.0f) continue;


#ifdef DEBUG_HITBOX
        // Mostra em amarelo quando as boxes estão se tocando
        if (CheckCollisionRecs(playerBox, npcs[i].screenBox))
            DrawRectangleLinesEx(npcs[i].screenBox, 3, YELLOW);
#endif

        if (CheckCollisionRecs(playerBox, npcs[i].screenBox)) {
            gameState.lives--;
            player.hitTimer  = 2.0f;
            player.speed    *= 0.3f;
            if (gameState.lives < 0)
                gameState.gameOver = true;
            break;
        }
    }
}