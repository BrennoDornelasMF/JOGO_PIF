#include "player.h"
#include "road.h"
#include "audio.h"
#include <math.h>

static const struct {
    KeyboardKey up, down, left, right;
} controls[MAX_PLAYERS] = {
    { KEY_UP,  KEY_DOWN, KEY_LEFT,  KEY_RIGHT },
    { KEY_W,   KEY_S,    KEY_A,     KEY_D     },
};

// WHITE para os dois — sem tint colorido
static const Color playerTints[MAX_PLAYERS] = { WHITE, WHITE };

Player players[MAX_PLAYERS];
float  playersX[MAX_PLAYERS];
int    activePlayerCount = 1;

static const char* spriteFiles[MAX_PLAYERS][CAR_SPRITE_COUNT] = {
    {
        "assets/sprites/player.png",
        "assets/sprites/player_left.png",
        "assets/sprites/player_right.png",
    },
    {
        "assets/sprites/player.png",
        "assets/sprites/player_left.png",
        "assets/sprites/player_right.png",
    },
};

void InitPlayers(void) {
    for (int id = 0; id < activePlayerCount; id++) {
        playersX[id]              = 0;
        players[id].x             = 0;
        players[id].speed         = 0;
        players[id].hitboxWidth   = 80.0f;
        players[id].hitboxHeight  = 1.5f;
        players[id].onGrass       = false;
        players[id].hitTimer      = 0.0f;
        players[id].currentSprite = CAR_STRAIGT;
        players[id].roadPos       = 0.0f;

        for (int s = 0; s < CAR_SPRITE_COUNT; s++)
            players[id].sprites[s] = LoadTexture(spriteFiles[id][s]);
    }
}

void UnloadPlayers(void) {
    for (int id = 0; id < activePlayerCount; id++)
        for (int s = 0; s < CAR_SPRITE_COUNT; s++)
            UnloadTexture(players[id].sprites[s]);
}

Rectangle GetPlayerScreenBox(int id, int viewW) {
    float scale   = 2.0f;
    float width   = players[id].sprites[CAR_STRAIGT].width  * scale;
    float height  = players[id].sprites[CAR_STRAIGT].height * scale;
    float screenX = viewW / 2 + playersX[id] * 0.5f;
    float screenY = SCREEN_HEIGHT - height / 2 + 20;
    return (Rectangle){
        screenX - width / 2 + 20,
        screenY - height    + 20,
        width  - 40,
        height - 40
    };
}

bool CheckPlayerOnGrass(int id) {
    return (playersX[id] < -500.0f || playersX[id] > 500.0f);
}

static void UpdateOnePlayer(int id) {
    float dt = GetFrameTime();
    Player *p = &players[id];
    float  *x = &playersX[id];

    if (p->hitTimer > 0) p->hitTimer -= dt;

    if (IsKeyDown(controls[id].up))
        p->speed += 0.03f;

    bool isBraking = IsKeyDown(controls[id].down);
    if (isBraking)
        p->speed -= 0.015f;

    p->speed *= 0.992f;

    if (p->speed < 0)    p->speed = 0;
    if (p->speed > 2.7f) p->speed = 2.7f;

    float steerStrength = 3.5f + p->speed;
    bool  pressingLeft  = IsKeyDown(controls[id].left);
    bool  pressingRight = IsKeyDown(controls[id].right);

    if (pressingLeft)  *x -= steerStrength;
    if (pressingRight) *x += steerStrength;

    if (pressingRight)     p->currentSprite = CAR_RIGHT;
    else if (pressingLeft) p->currentSprite = CAR_LEFT;
    else                   p->currentSprite = CAR_STRAIGT;

    // posição independente na pista por jogador
    p->roadPos += p->speed * 0.01f;
    p->roadPos  = fmod(p->roadPos, trackDataLen);

    float curve = readTrack(p->roadPos);
    *x -= curve * p->speed * 0.01f;

    p->onGrass = CheckPlayerOnGrass(id);
    if (p->onGrass) {
        p->speed *= 0.97f;
        *x       *= 0.97f;
    }

    if (*x < -600) *x = -600;
    if (*x >  600) *x =  600;
    p->x = *x;

    if (id == 0)
        UpdateCarSounds(p->speed, 2.7f, isBraking);
}

void UpdatePlayers(void) {
    for (int id = 0; id < activePlayerCount; id++)
        UpdateOnePlayer(id);

    roadPosition = players[0].roadPos;
}

void DrawPlayer(int id, int viewW, int offsetX) {
    Player    *p   = &players[id];
    Texture2D  tex = p->sprites[p->currentSprite];

    float scale   = 2.0f;
    float width   = tex.width  * scale;
    float height  = tex.height * scale;

    extern float turnValue;
    float rotation = (id == 0 && p->currentSprite == CAR_STRAIGT)
                     ? turnValue * 0.005f : 0.0f;

    // no coop, o carro DONO da viewport fica sempre centralizado
    // o carro do OUTRO jogador aparece deslocado conforme sua posição lateral
    float localX  = (float)viewW / 2.0f + playersX[id] * 0.5f;
    float screenX = offsetX + localX;
    float screenY = SCREEN_HEIGHT - height / 2 + 20;

    DrawTexturePro(
        tex,
        (Rectangle){ 0, 0, tex.width, tex.height },
        (Rectangle){ screenX, screenY, width, height },
        (Vector2){ width / 2, height },
        rotation,
        playerTints[id]
    );

#ifdef DEBUG_HITBOX
    DrawRectangleLinesEx(
        (Rectangle){
            screenX - width / 2 + 20,
            screenY - height    + 20,
            width  - 40,
            height - 40
        }, 2, RED);
    DrawText(TextFormat("P%d spd: %.2f", id+1, p->speed), offsetX+10, 65+id*25, 18, WHITE);
    DrawText(TextFormat("P%d X: %.1f",   id+1, playersX[id]), offsetX+10, 90+id*25, 18, WHITE);
#endif
}