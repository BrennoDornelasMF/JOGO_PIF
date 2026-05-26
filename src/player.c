#include "player.h"
#include "road.h"
#include <math.h>

Player player;
float playerX = 0;

void InitPlayer(void) {

    playerX = 0;
    player.speed = 0;

    player.texture =
        LoadTexture("assets/sprites/player.png");
}

void UpdatePlayer(void) {

    // =========================
    // ACELERAÇÃO
    // =========================

    if (IsKeyDown(KEY_UP)) {

        player.speed += 0.05f;
    }

    // ========================
    //VELOCIDADE MÁXIMA
    // ========================

    if(player.speed >30)
        player.speed=30;

    // =========================
    // FREIO
    // =========================

    if (IsKeyDown(KEY_DOWN)) {

        player.speed -= 0.35f;
    }

    // =========================
    // ATRITO NATURAL
    // =========================

    player.speed *= 0.992f;

    // =========================
    // LIMITES
    // =========================

    if (player.speed < 0)
        player.speed = 0;

    if (player.speed > 80)
        player.speed = 80;

    // =========================
    // DIREÇÃO
    // =========================

    float steerStrength =
        2.0f + player.speed * 0.03f;

    if (IsKeyDown(KEY_LEFT))
        playerX -= steerStrength;

    if (IsKeyDown(KEY_RIGHT))
        playerX += steerStrength;
    
    // =========================
    // MOVIMENTO DA PISTA
    // =========================
    
    
    roadPosition += player.speed * 0.02f;

    roadPosition = fmod(roadPosition, trackDataLen);
    float curve = readTrack(roadPosition);
    playerX += curve * player.speed * 0.005f;
    
    
    

    // =========================
    // LIMITE DA PISTA
    // =========================

    if (playerX < -400)
        playerX = -400;

    if (playerX > 400)
        playerX = 400;

}

void DrawPlayer(void) {

    float scale = 2.0f;

    float width =
        player.texture.width * scale;

    float height =
        player.texture.height * scale;

    DrawTexturePro(

        player.texture,

        (Rectangle){
            0,
            0,
            player.texture.width,
            player.texture.height
        },

        (Rectangle){
            SCREEN_WIDTH / 2 + playerX * 0.5f,
            SCREEN_HEIGHT - height/2+20,
            width,
            height
        },

        (Vector2){
            width / 2,
            height
        },

        0,
        WHITE
    );
}

void UnloadPlayer(void) {

    UnloadTexture(player.texture);
}