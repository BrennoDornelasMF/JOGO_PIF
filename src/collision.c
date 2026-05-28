#include "npc.h"
#include "game.h"
#include "player.h"
#include "raylib.h"
#include <math.h>

#define HIT_COOLDOWN 2.0f   // segundos de invencibilidade

void CheckCollisions(float playerRoadPos) {
    if (player.hitTimer > 0) return;   // invencível

    for (int i = 0; i < MAX_NPCS; i++) {
        if (!npcs[i].active) continue;

        float dz = fabsf(npcs[i].roadPos - playerRoadPos);
        float dx = fabsf(npcs[i].laneX - playerX);

        // Ajuste esses valores conforme o tamanho dos sprites
        if (dz < 0.5f && dx < 200.0f) {
            // BATEU!
            gameState.lives--;
            player.hitTimer = HIT_COOLDOWN;
            player.speed   *= 0.3f;   // trava o carro

            if (gameState.lives <= 0)
                gameState.gameOver = true;

            break;
        }
    }
}