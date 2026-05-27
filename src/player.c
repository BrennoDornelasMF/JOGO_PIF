#include "player.h"
#include "road.h"
#include <math.h>

extern float turnValue;



Player player;
float playerX = 0;

void InitPlayer(void) {

    playerX = 0;
    player.x = 0;
    player.speed = 0;
    player.hitboxWidth = 80.0f;
    player.hitboxHeight = 1.5f;
    player.onGrass = false;
    player.hitTimer = 0.0f;
    player.currentSprite = CAR_STRAIGT;

    player.sprites[CAR_STRAIGT] = LoadTexture("assets/sprites/player.png");
    player.sprites[CAR_LEFT]    = LoadTexture("assets/sprites/player_left.png");
    player.sprites[CAR_RIGHT] = LoadTexture("assets/sprites/player_right.png");

}

Rectangle GetPlayerHitbox(){
    return (Rectangle){
        playerX - player.hitboxWidth / 2,
        0,
        player.hitboxWidth,
        player.hitboxHeight
    };
}

bool CheckPlayerOnGrass(){
    return (playerX < -500.0f || playerX > 500.0f);
}

void UpdatePlayer(void) {

    float dt = GetFrameTime();

    if(player.hitTimer > 0)
        player.hitTimer -= dt;

    // =========================
    // ACELERAÇÃO
    // =========================

    if (IsKeyDown(KEY_UP)) {

        player.speed += 0.05f;
    }

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

    // ========================
    // VELOCIDADE MÁXIMA
    // ========================
    // if(player.speed > 10)
    //     player.speed = 10;

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

    bool pressingLeft  = IsKeyDown(KEY_LEFT);
    bool pressingRight = IsKeyDown(KEY_RIGHT);

    if (pressingLeft)
        playerX -= steerStrength;

    if (pressingRight)
        playerX += steerStrength;

    if(pressingRight)
        player.currentSprite = CAR_RIGHT;
    else if(pressingLeft)
        player.currentSprite = CAR_LEFT;
    else 
        player.currentSprite = CAR_STRAIGT;
    // =========================
    // MOVIMENTO DA PISTA
    // =========================
    
    
    roadPosition += player.speed * 0.01f;
    roadPosition  = fmod(roadPosition, trackDataLen);

    float curve = readTrack(roadPosition);

    playerX -= curve * player.speed * 0.03f;

    // GRAMA
    player.onGrass = CheckPlayerOnGrass();

    if(player.onGrass){
        player.speed *= 0.97f;
        playerX      *= 0.97f;
    }

    // Empurra o Carro para fora nas curvas
    // float curvePush = curve * player.speed * 0.015f;
    // playerX -= curvePush;
    
 
    // =========================
    // LIMITE DA PISTA
    // =========================

    if (playerX < -600)
        playerX = -600;

    if (playerX > 600)
        playerX = 600;

    player.x = playerX;

}

void DrawPlayer(void) {

    Texture2D tex = player.sprites[player.currentSprite];

    float scale = 2.0f;
    float width = tex.width * scale;
    float height = tex.height * scale;
   
    float screenX = SCREEN_WIDTH /2 + playerX * 0.5f;
    float screenY = SCREEN_HEIGHT - height / 2 + 20;
    float rotation = (player.currentSprite == CAR_STRAIGT) ? turnValue * 0.005f : 0.0f;
    DrawTexturePro(

        tex,

        (Rectangle){
            0,
            0,
            tex.width,
            tex.height
        },

        (Rectangle){
            screenX,
            screenY,
            width,
            height
        },

        (Vector2){
            width / 2,
            height
        },

        rotation,
        WHITE
  
    );
    #ifdef DEBUG_HITBOX
        DrawRectangleLinesEx(
            (Rectangle){
                screenX - width / 2,
                screenY - height,
                width,
                height
            },
            2,
            RED
        );

        DrawText(TextFormat("sprite: %d", player.currentSprite), 10, 145, 20, YELLOW);
        DrawText(TextFormat("playerX: %.1f", playerX), 10, 65, 20, WHITE);
        DrawText(TextFormat("speed:   %.2f", player.speed), 10, 90, 20, WHITE);
        DrawText(TextFormat("curve: %.3f", readTrack(roadPosition)), 10, 115, 20, YELLOW);
    #endif  
}


void UnloadPlayer(void) {
    for (int i = 0; i < CAR_SPRITE_COUNT; i++)
        UnloadTexture(player.sprites[i]);
}