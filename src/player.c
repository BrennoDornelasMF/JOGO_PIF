#include "player.h"
#include "road.h"
#include <math.h>
extern float turnValue;

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
    // VELOCIDADE MÁXIMA
    // ========================

    if(player.speed > 10)
        player.speed = 10;

    // =========================
    // FREIO
    // =========================

    if (IsKeyDown(KEY_DOWN)) {

        player.speed -= 0.05f;
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

    if (player.speed > 3)
        player.speed = 3;

    // =========================
    // DIREÇÃO
    // =========================

    float steerStrength =
        3.5f + player.speed ;

    if (IsKeyDown(KEY_LEFT))
        playerX -= steerStrength;

    if (IsKeyDown(KEY_RIGHT))
        playerX += steerStrength;
    
    // =========================
    // MOVIMENTO DA PISTA
    // =========================
    
    
    roadPosition += player.speed * 0.01f;

    // if(roadPosition > 10){
    //     roadPosition = 0;
    // }

    roadPosition = fmod(roadPosition, trackDataLen);
    float curve = readTrack(roadPosition);
    playerX += curve * player.speed * 0.005f;
    
    
    

    // =========================
    // LIMITE DA PISTA
    // =========================

    if (playerX < -600)
        playerX = -600;

    if (playerX > 600)
        playerX = 600;

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

        turnValue * 0.01f,
        WHITE
    );
}

void UnloadPlayer(void) {

    UnloadTexture(player.texture);
}