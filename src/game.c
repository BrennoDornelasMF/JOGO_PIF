#include "game.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include "player.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define MAX_SPEED_KMH 270.f // velocidade maxima em kmh
#define NEEDLE_START -135.f // angulo de comeco do ponteiro
#define NEEDLE_SWEEP 260.f // angulo de movimento do ponteiro do inicio ao fim 

static float needleAngle = -220.f; // guarda o angulo atual do ponteiro começando em zero e atualizando depois
static Texture2D speedoBg; //declaracao da textura
static Texture2D speedoNeedle; //declaracao da textura



GameState gameState;

static void LoadHighscores() {
    gameState.scoreCount = 0;
    FILE* f = fopen(SCORE_FILE, "r");
    if (!f) return;

    while (gameState.scoreCount < MAX_SCORES) {
        ScoreEntry* e = &gameState.highscores[gameState.scoreCount];
        if (fscanf(f, "%31s %d", e->name, &e->score) != 2) break;
        gameState.scoreCount++;
    }
    fclose(f);
}

static void SaveHighscores(){
    FILE* f = fopen(SCORE_FILE, "w");
    if(!f) return;
    for(int i =0; i < gameState.scoreCount; i++)
        fprintf(f, "%s %d\n",
                gameState.highscores[i].name,
                gameState.highscores[i].score);
    fclose(f);
}

static void TryInsertScore(const char* name, int score){
    int count = gameState.scoreCount;

    if (count >= MAX_SCORES && score <= gameState.highscores[MAX_SCORES-1].score)
        return;

    int pos = (count < MAX_SCORES) ? count : MAX_SCORES - 1;
    for (int i = 0; i < count && i < MAX_SCORES; i++) {
        if (score > gameState.highscores[i].score) { pos = i; break; }
    }

    int limit = (count < MAX_SCORES) ? count : MAX_SCORES - 1;
    for (int i = limit; i > pos; i--)
        gameState.highscores[i] = gameState.highscores[i-1];

    strncpy(gameState.highscores[pos].name, name, 31);
    gameState.highscores[pos].name[31] = '\0';
    gameState.highscores[pos].score    = score;

    if (gameState.scoreCount < MAX_SCORES) gameState.scoreCount++;

    SaveHighscores();
}

static void DrawHighscores(void) {
    DrawRectangle(SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 260, 440, 520,
                  Fade(BLACK, 0.88f));
    DrawText("TOP 10", SCREEN_WIDTH/2 - 55, SCREEN_HEIGHT/2 - 240, 32, YELLOW);

    if (gameState.scoreCount == 0) {
        DrawText("Nenhuma pontuacao ainda",
                 SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 190, 20, GRAY);
        return;
    }

    for (int i = 0; i < gameState.scoreCount; i++) {
        Color c = (i == 0) ? GOLD :
                  (i == 1) ? LIGHTGRAY :
                  (i == 2) ? ORANGE : WHITE;
        DrawText(TextFormat("%2d. %-14s %d",
                            i+1,
                            gameState.highscores[i].name,
                            gameState.highscores[i].score),
                 SCREEN_WIDTH/2 - 190,
                 SCREEN_HEIGHT/2 - 190 + i * 40,
                 22, c);
    }

    DrawText("ENTER para reiniciar  ESC para sair",
             SCREEN_WIDTH/2 - 190, SCREEN_HEIGHT/2 + 230, 18, GRAY);
}


void InitGame(void) {
    gameState.lives         = 3;
    gameState.score         = 0;
    gameState.scoreTimer    = 0.0f;
    gameState.gameOver      = false;
    gameState.enteringName  = false;
    gameState.scoreSaved    = false;
    gameState.nameLen       = 0;
    gameState.playerName[0] = '\0';
    speedoBg = LoadTexture("assets/sprites/speedo_bg.png"); //inicia o velocimetro
    speedoNeedle = LoadTexture("assets/sprites/speedo_needle.png"); // inicia o ponteiro

    LoadHighscores();
}

void UpdateGame(float playerSpeed) {
    if (gameState.gameOver) {
        if (!gameState.scoreSaved && !gameState.enteringName)
            gameState.enteringName = true;

        if (gameState.enteringName) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125 && gameState.nameLen < 31) {
                    gameState.playerName[gameState.nameLen++] = (char)key;
                    gameState.playerName[gameState.nameLen]   = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && gameState.nameLen > 0)
                gameState.playerName[--gameState.nameLen] = '\0';

            if (IsKeyPressed(KEY_ENTER) && gameState.nameLen > 0) {
                TryInsertScore(gameState.playerName, gameState.score);
                gameState.scoreSaved   = true;
                gameState.enteringName = false;
            }
        }
        return;  // mantém o return — reinício é tratado no main.c
    }

    gameState.scoreTimer += GetFrameTime();
    float fraction = (playerSpeed * 100.0f) / MAX_SPEED_KMH;
    if (fraction < 0.0f) fraction=0.0f;
    if (fraction > 1.0f) fraction = 1.0f;
    float targetAngle = NEEDLE_START + fraction * NEEDLE_SWEEP;
    needleAngle += (targetAngle - needleAngle) *0.12f; // lerp para o ponteiro se mover suavemente ao inves de teleportar 
    if (gameState.scoreTimer >= 1.0f) {
        gameState.scoreTimer = 0.0f;
        gameState.score += (int)(playerSpeed * 10);
    }
}

static void DrawSpeedo(void){
    //posicao central do velocimetro canto inferior direito da tela, ou seja, defino onde eu quero o velocimetro na tela 
    float cx = SCREEN_WIDTH - 90.0f; //mover para esquerda diminui o cx ou para direita aumenta o cx
    float cy = SCREEN_HEIGHT - 90.0f; // mover para cima diminui o cy ou para baixo aumenta o cy

    //desenha o velocimetro centralizado em cx e cy
    //tamanho 140x140
    // 
    DrawTexturePro(
        speedoBg,
        (Rectangle){0, 0, (float)speedoBg.width, (float)speedoBg.height}, //sprite inteiro
        (Rectangle){cx - 70, cy - 70 ,  140 , 140}, // é definido 70 pois é o centro da posicao que eu defini no cx/cy. como o drawtexture pro nao aceita "centro" é preciso metade do tamanho do sprite pra centralizar-lo
        (Vector2){0,0}, //sem pivo extra, ja centralizei manualmente o sprite
        0.0f, //sem rotacao no fundo
        WHITE
    );


    //desenho do ponteiro
    DrawTexturePro(
        speedoNeedle,
        (Rectangle){0, 0, (float)speedoNeedle.width, (float)speedoNeedle.height}, 
        (Rectangle){cx, cy,  140 , 140}, // não é definido manualmente pois o programa entenderia que deslocaria sempre que andasse então não giraria no centro do velocimetro
        (Vector2){70,70}, //necessario pivo pois o ponteiro se movimenta
        needleAngle, //angulo atual atualizado em UpdateGame
        WHITE
    );

    int kmh = (int)(player.speed * 100.0f); // velocidade maxima do velocimetro, formula (270(velomaxima) / player(speed = 2.70)) = 100, assim consigo sincronizar a velocidade do player com o velocimetro
    DrawText(TextFormat("%d", kmh), cx -15, cy +30, 18, WHITE);


}





void DrawHUD(void) {
    DrawSpeedo();
    DrawText(TextFormat("SCORE: %d", gameState.score), 20, 10, 24, WHITE);

    for (int i = 0; i < gameState.lives; i++)
        DrawText("[v]", 20 + i * 50, 40, 24, RED);

    if (!gameState.gameOver) return;

    if (gameState.enteringName) {
        // Tela de entrada de nome
        DrawRectangle(SCREEN_WIDTH/2 - 240, SCREEN_HEIGHT/2 - 100,
                      480, 200, Fade(BLACK, 0.90f));
        DrawText("GAME OVER",
                 SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 - 85, 36, RED);
        DrawText("Digite seu nome:",
                 SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 30, 22, WHITE);
        DrawText(TextFormat("%s_", gameState.playerName),
                 SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 10, 28, YELLOW);
        DrawText("ENTER para confirmar",
                 SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 60, 18, GRAY);
    } else if (gameState.scoreSaved) {
        DrawHighscores();
    }
}
