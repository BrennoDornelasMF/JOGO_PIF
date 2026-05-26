#include "raylib.h"

#include "road.h"
#include "player.h"
#include "camera.h"
#include "sprite.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(){


    InitWindow(SCREEN_WIDTH ,SCREEN_HEIGHT, "teste pista");

    SetTargetFPS(60);

    InitRoad();
    InitPlayer();
    InitSprites();

    while (!WindowShouldClose()){

        UpdatePlayer();
        UpdateGameCamera();

        //UpdateRoad();

        // DRAW
        BeginDrawing();

        ClearBackground(SKYBLUE);

        DrawRoad();
        DrawSprites();
        DrawPlayer();
        DrawText(TextFormat("roadPos: %.1f", roadPosition), 10, 10, 20, RED);
        


        EndDrawing();
    }

    UnloadSprites();
    UnloadPlayer();
    CloseWindow();

    return 0;
    

}