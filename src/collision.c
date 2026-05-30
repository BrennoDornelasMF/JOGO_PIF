#include "collision.h"
#include "npc.h"
#include "game.h"
#include "player.h"
#include "raylib.h"
#include <stdlib.h>
#include "road.h"

void CheckCollisions(float playerRoadPos) {
    (void)playerRoadPos;
    if (player.hitTimer > 0) return;

    Rectangle playerBox = GetPlayerScreenBox(0, SCREEN_WIDTH);

    for (NPCNode* node = npcList; node != NULL; node = node->next) {
        if (!node->data.active) continue;
        if (node->data.screenBox.width < 4.0f) continue;

        if (CheckCollisionRecs(playerBox, node->data.screenBox)) {
            gameState.lives--;
            player.hitTimer  = 2.0f;
            player.speed    *= 0.3f;
            if (gameState.lives <= 0)
                gameState.gameOver = true;
            break;
        }
    }
   
}