#include "raylib.h"
#include <string>
#include <math.h>
// Animation variables
float timeElapsed = 0.0f;
const float amplitude = 15.0f;
const float frequency = 2.0f;

enum GameState {
    MENU,
    GAME,
    GAMEOVER,
    EXIT
};

Color white = (Color){ 255, 255, 255, 255 };

int main() {
    InitWindow(800, 600, "Bible Quest");
    SetTargetFPS(60);

    // Load logo texture (do this once, outside the loop)
    Texture2D logo = LoadTexture("logo.png");
	Texture2D background = LoadTexture("lou.png"); // make sure path is correct

    GameState state = MENU;

    while (!WindowShouldClose()) {
        // --- Update logic ---
        timeElapsed += GetFrameTime();
        float bounceY = 80 + amplitude * sin(frequency * timeElapsed);

        // --- Draw everything ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        

        if (state == MENU) {
			// Draw logo centered
        int logoX = GetScreenWidth()/2 - logo.width/2;
        int logoY = 50;
        DrawTextureEx(logo, (Vector2){logoX - 50, bounceY}, 0.0f, 5.0f, white);
            DrawText("Press ENTER to Start", 290, 400, 20, GRAY);

            if (IsKeyPressed(KEY_ENTER)) state = GAME;
        }
        else if (state == GAME) {
            DrawText("Before we start, I'd \n like to test your Bible knowledge.", 400, 40, 20, BLACK);
            DrawText("Take the test!", 400, 300, 20, BLACK);
            


            if (IsKeyPressed(KEY_ESCAPE)) state = GAMEOVER;
        }
        else if (state == GAMEOVER) {
            DrawText("Game Over!", 320, 200, 40, RED);
            DrawText("Press ENTER to Return to Menu", 200, 300, 20, GRAY);

            if (IsKeyPressed(KEY_ENTER)) state = MENU;
        }

        EndDrawing();
    }

    // --- Cleanup ---
    UnloadTexture(logo);
    CloseWindow();

    return 0;
}
