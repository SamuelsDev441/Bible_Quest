#include <iostream>
#include "raylib.h"
#include <string.h>

int player1_score = 0;
int player2_score = 0;
std::string player_1_name = "";
std::string player_2_name = "";
int sl = 5; // score limit

// -------------------- BALL CLASS --------------------
class Ball {
public:
    float x, y;
    int speedX, speedY;
    int radius;

    void Draw() {
        DrawCircle(x, y, radius, WHITE);
    }

    void Reset() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;
        speedX *= -1;
    }

    void Update() {
        x += speedX;
        y += speedY;

        if (y + radius >= GetScreenHeight() || y - radius <= 0)
            speedY *= -1;

        if (x + radius >= GetScreenWidth()) {
            player2_score++;
            Reset();
        }

        if (x - radius <= 0) {
            player1_score++;
            Reset();
        }
    }
};

// -------------------- PLAYER PADDLE --------------------
class Paddle {
protected:
    void LimitMovement() {
        if (y <= 0) y = 0;
        if (y + height >= GetScreenHeight()) y = GetScreenHeight() - height;
    }

public:
    float x, y;
    int width, height;
    int speed;

    void Draw() {
        DrawRectangle(x, y, width, height, WHITE);
    }

    virtual void Update() {
        if (IsKeyDown(KEY_UP)) y -= speed;
        if (IsKeyDown(KEY_DOWN)) y += speed;
        LimitMovement();
    }
};

// -------------------- PLAYER 2 --------------------
class Paddle2 : public Paddle {
public:
    void Update() override {
        if (IsKeyDown(KEY_W)) y -= speed;
        if (IsKeyDown(KEY_S)) y += speed;
        LimitMovement();
    }
};

// -------------------- CPU PADDLE --------------------
class PaddleAI : public Paddle {
public:
    void Update(int ballY) {
        if (ballY < y + height / 2 && y > 0) y -= speed;
        if (ballY > y + height / 2 && y + height < GetScreenHeight()) y += speed;
        LimitMovement();
    }
};

// -------------------- GAME STATES --------------------
enum GameState {
    MENU,
    HOWTOPLAY,
    ENTER_NAME,
    GAME,
    GAMEOVER,
    SETTINGS,
    AI_MODE,
    EXIT
};

int main() {
    int width = 1200;
    int height = 800;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Pong");
    SetTargetFPS(60);

    Ball ball;
    Paddle p1;
    Paddle2 p2;
    PaddleAI p2_ai;

    // Initialize objects
    ball.x = width / 2;
    ball.y = height / 2;
    ball.radius = 10;
    ball.speedX = 6;
    ball.speedY = 6;

    p1.x = width - 40;
    p1.y = height / 2 - 60;
    p1.width = 25;
    p1.height = 120;
    p1.speed = 8;

    p2.x = 15;
    p2.y = height / 2 - 60;
    p2.width = 25;
    p2.height = 120;
    p2.speed = 7;

    p2_ai.x = 15;
    p2_ai.y = height / 2 - 60;
    p2_ai.width = 25;
    p2_ai.height = 120;
    p2_ai.speed = 6;

    // -------------------- INPUT BOXES --------------------
    Rectangle inputBox1 = { 500, 250, 400, 50 };
    Rectangle inputBox2 = { 500, 350, 400, 50 };
    Rectangle inputBox3 = { 500, 250, 400, 50 };
    char input3[11] = ""; // default score limit
    char input1[11] = "";
    char input2[11] = "";
    int activeBox = 0; // 0 = none, 1 = box1, 2 = box2

    GameState state = MENU;
    int selected = 0;
    const char* options[] = { "Play", "How to Play", "AI Mode", "Settings", "Exit" };
    const int optionsCount = 5;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // ================= MENU =================
        if (state == MENU) {
            DrawText("PONG", width / 2 - MeasureText("PONG", 100) / 2, 150, 100, WHITE);

            for (int i = 0; i < optionsCount; i++) {
                Color color = (i == selected) ? YELLOW : RAYWHITE;
                DrawText(options[i],
                         width / 2 - MeasureText(options[i], 40) / 2,
                         350 + i * 70,
                         40,
                         color);
            }

            if (IsKeyPressed(KEY_DOWN)) selected = (selected + 1) % optionsCount;
            if (IsKeyPressed(KEY_UP)) selected = (selected - 1 + optionsCount) % optionsCount;

            if (IsKeyPressed(KEY_ENTER)) {
                if (selected == 0) { // Play
                    state = ENTER_NAME;
                    input1[0] = '\0';
                    input2[0] = '\0';
                    activeBox = 1;
                } else if (selected == 1) state = HOWTOPLAY;
                else if (selected == 2) {
                    state = ENTER_NAME;
                    player1_score = 0;
                    player2_score = 0;
                    ball.Reset();
                } else if (selected == 4) state = EXIT;
                else if (selected == 3) state = SETTINGS;
            }
        }

        // ================= HOW TO PLAY =================
        else if (state == HOWTOPLAY) {
            DrawText("How to Play", width / 2 - MeasureText("How to Play", 60) / 2, 100, 60, WHITE);
            DrawText("Use W/S for Player 2 or UP/DOWN for Player 1.", 200, 250, 30, RAYWHITE);
            DrawText("Score points when your opponent misses the ball.", 200, 300, 30, RAYWHITE);
            DrawText("Press BACKSPACE to return to the Menu.", 200, 400, 30, GRAY);
            DrawText("First to reach the score limit wins!", 200, 350, 30, RAYWHITE);
            DrawText("In AI Mode, Player 2 is controlled by the computer.", 200, 450, 30, RAYWHITE);
            DrawText("Player 1 is on the LEFT.", 200, 500, 30, GRAY);
            DrawText("Player 2 and on the RIGHT.", 200, 550, 30, GRAY);
            DrawText("Player 1 uses WASD.", 200, 700, 30, GRAY);
            DrawText("Player 2 uses UP/DOWN.", 200, 750, 30, GRAY);
            DrawText("Enjoy the game!", 200, 600, 30, RAYWHITE);
            if (IsKeyPressed(KEY_BACKSPACE)) state = MENU;
        }

        // ================= ENTER NAME =================
        else if (state == ENTER_NAME && selected == 0) {
            DrawText("Enter Player Names", width / 2 - MeasureText("Enter Player Names", 40) / 2, 150, 40, WHITE);

            // Draw labels
            DrawText("Player 1:", inputBox1.x - 140, inputBox1.y + 10, 30, RAYWHITE);
            DrawText("Player 2:", inputBox2.x - 140, inputBox2.y + 10, 30, RAYWHITE);

            // Draw boxes
            DrawRectangleRec(inputBox1, (activeBox == 1) ? LIGHTGRAY : GRAY);
            DrawRectangleRec(inputBox2, (activeBox == 2) ? LIGHTGRAY : GRAY);
            DrawRectangleLines(inputBox1.x, inputBox1.y, inputBox1.width, inputBox1.height, DARKGRAY);
            DrawRectangleLines(inputBox2.x, inputBox2.y, inputBox2.width, inputBox2.height, DARKGRAY);

            // Draw current input text
            DrawText(input1, inputBox1.x + 5, inputBox1.y + 10, 30, BLACK);
            DrawText(input2, inputBox2.x + 5, inputBox2.y + 10, 30, BLACK);

            // Check for mouse clicks to select box
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, inputBox1)) activeBox = 1;
                else if (CheckCollisionPointRec(mousePos, inputBox2)) activeBox = 2;
                else activeBox = 0;
            }

            // Handle text input
            if (activeBox != 0) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (activeBox == 1) {
                        int len = strlen(input1);
                        if ((key >= 32 && key <= 126) && len < 10) {
                            input1[len] = (char)key;
                            input1[len + 1] = '\0';
                        }
                    } else if (activeBox == 2) {
                        int len = strlen(input2);
                        if ((key >= 32 && key <= 126) && len < 10) {
                            input2[len] = (char)key;
                            input2[len + 1] = '\0';
                        }
                    }
                    key = GetCharPressed();
                }

                // Backspace
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (activeBox == 1) {
                        int len = strlen(input1);
                        if (len > 0) input1[len - 1] = '\0';
                    } else if (activeBox == 2) {
                        int len = strlen(input2);
                        if (len > 0) input2[len - 1] = '\0';
                    }
                    else if (activeBox == 0) {state = MENU;}
                }
            }

            // Press ENTER to confirm names
            if (IsKeyPressed(KEY_ENTER) && strlen(input1) > 0 && strlen(input2) > 0) {
                player_2_name = std::string(input1);
                player_1_name = std::string(input2);
                state = GAME;
                player2_score = 0;
                player1_score = 0;
                ball.Reset();
            }
        }

        else if (state == ENTER_NAME && selected == 2) {
            DrawText("Enter Player Names", width / 2 - MeasureText("Enter Player Names", 40) / 2, 150, 40, WHITE);

            // Draw labels
            DrawText("Player 1:", inputBox1.x - 140, inputBox1.y + 10, 30, RAYWHITE);

            // Draw boxes
            DrawRectangleRec(inputBox1, (activeBox == 1) ? LIGHTGRAY : GRAY);
           
            DrawRectangleLines(inputBox1.x, inputBox1.y, inputBox1.width, inputBox1.height, DARKGRAY);
            
            // Draw current input text
            DrawText(input1, inputBox1.x + 5, inputBox1.y + 10, 30, BLACK);

            // Check for mouse clicks to select box
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, inputBox1)) activeBox = 1;

                else activeBox = 0;
            }

            // Handle text input
            if (activeBox != 0) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (activeBox == 1) {
                        int len = strlen(input1);
                        if ((key >= 32 && key <= 126) && len < 10) {
                            input1[len] = (char)key;
                            input1[len + 1] = '\0';
                        }
                    } 
                    key = GetCharPressed();
                }

                // Backspace
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (activeBox == 1) {
                        int len = strlen(input1);
                        if (len > 0) input1[len - 1] = '\0';
                    }
                }
            }

            // Press ENTER to confirm names
            if (IsKeyPressed(KEY_ENTER) && strlen(input1) > 0) {
                player_2_name = std::string(input1);
                player_1_name = "AI";
                state = AI_MODE;
                player2_score = 0;
                player1_score = 0;
                ball.Reset();
            }
        }

        else if (state == SETTINGS) {
            DrawText("Settings", width / 2 - MeasureText("Settings", 40) / 2, 150, 40, WHITE);

            // Draw labels
            DrawText("Score Limit:", inputBox3.x - 180, inputBox3.y + 10, 30, RAYWHITE);

            // Draw boxes
            DrawRectangleRec(inputBox3, (activeBox == 1) ? LIGHTGRAY : GRAY);
           
            DrawRectangleLines(inputBox3.x, inputBox3.y, inputBox3.width, inputBox3.height, DARKGRAY);
            
            // Draw current input text
            DrawText(input3, inputBox3.x + 5, inputBox3.y + 10, 30, BLACK);

            // Check for mouse clicks to select box
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, inputBox3)) activeBox = 4;

                else activeBox = 0;
            }

            // Handle text input
            if (activeBox != 0) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (activeBox == 4) {
                        int len = strlen(input3);
                        if ((key >= 48 && key <= 57) && len < 10) {
                            input3[len] = (char)key;
                            input3[len + 1] = '\0';
                        }
                    } 
                    key = GetCharPressed();
                }

                // Backspace
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (activeBox == 4) {
                        int len = strlen(input3);
                        if (len > 0) input3[len - 1] = '\0';
                    }
                }
            }

            // Press ENTER to confirm names
            if (IsKeyPressed(KEY_ENTER) && strlen(input3) > 0) {
                std::string inp = std::string(input3);
                sl = std::stoi(inp);
                

                state = MENU;
                
                
            }
        }
        

        // ================= GAME =================
        else if (state == GAME) {
            ball.Update();
            p1.Update();
            p2.Update();

            // Collisions
            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { p1.x, p1.y, (float)p1.width, (float)p1.height }))
                ball.speedX *= -1;
            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { p2.x, p2.y, (float)p2.width, (float)p2.height }))
                ball.speedX *= -1;

            // Draw field
            Color background;
            background = { 24, 224, 180, 1 }; // Black background
            
            DrawLine(width / 2, 0, width / 2, height, WHITE);
            DrawText(std::to_string(player1_score).c_str(), width / 2 + 40, 20, 40, WHITE);
            DrawText(std::to_string(player2_score).c_str(), width / 2 - 80, 20, 40, WHITE);
            DrawCircle(width / 2, height / 2, 20, WHITE);
            ClearBackground(background);

            ball.Draw();
            p1.Draw();
            p2.Draw();

            DrawText(player_1_name.c_str(), width - 250, 50, 20, GRAY);
            DrawText(player_2_name.c_str(), 50, 50, 20, GRAY);

            DrawText("Press BACKSPACE to return to Menu", width / 2 - 200, height - 40, 20, GRAY);

            if (player1_score >= sl || player2_score >= sl) state = GAMEOVER;
            if (IsKeyPressed(KEY_BACKSPACE)) state = MENU;
        }

        // ================= GAME OVER =================
        else if (state == GAMEOVER) {
            std::string winner;
            if (player1_score > player2_score) winner = player_1_name + " WINS!";
            else if (player2_score > player1_score) winner = player_2_name + " WINS!";
            else winner = "TIE!";

            DrawText("Game Over", width / 2 - MeasureText("Game Over", 80) / 2, 200, 80, WHITE);
            DrawText(winner.c_str(), width / 2 - MeasureText(winner.c_str(), 40) / 2, 320, 40, YELLOW);
            DrawText("Press ENTER to return to menu", width / 2 - 250, 500, 30, GRAY);

            if (IsKeyPressed(KEY_ENTER)) state = MENU;
        }

        // ================= AI MODE =================
        else if (state == AI_MODE) {
            ball.Update();
            p1.Update();
            p2_ai.Update(ball.y);

            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { p1.x, p1.y, (float)p1.width, (float)p1.height }))
                ball.speedX *= -1;
            if (CheckCollisionCircleRec({ ball.x, ball.y }, ball.radius, { p2_ai.x, p2_ai.y, (float)p2_ai.width, (float)p2_ai.height }))
                ball.speedX *= -1;

            DrawLine(width / 2, 0, width / 2, height, WHITE);
            DrawText(std::to_string(player1_score).c_str(), width / 2 + 40, 20, 40, WHITE);
            DrawText(std::to_string(player2_score).c_str(), width / 2 - 80, 20, 40, WHITE);
            DrawText(player_1_name.c_str(), width - 250, 50, 20, GRAY);
            DrawText(player_2_name.c_str(), 50, 50, 20, GRAY);

            ball.Draw();
            p1.Draw();
            p2_ai.Draw();

            if (player1_score >= 5 || player2_score >= 5) state = GAMEOVER;
            if (IsKeyPressed(KEY_BACKSPACE)) state = MENU;
        }

        // ================= EXIT =================
        else if (state == EXIT) {
            CloseWindow();
            break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
