#include <iostream>
#include <raylib.h>

using namespace std;

Color Green = {38, 185, 154, 255};
Color DarkGreen = {20, 160, 133, 255};
Color LightGreen = {129, 204, 184, 255};
Color Yellow = {243, 213, 91, 255};

int player_score = 0;
int cpu_score = 0;
int boost_timer = 0;           // Added
bool boost_active = false;      // Added
int countdown = 0;              // Added

class Ball {
public:
    float x, y;
    int speedX, speedY;
    int radius;
    int originalSpeedX, originalSpeedY;

    void Draw() {
        DrawCircle(x, y, radius, Yellow);
    }

    void Update() {
        x += speedX;
        y += speedY;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speedY *= -1;
        }
        if (x + radius >= GetScreenWidth()) { // CPU wins
            speedX *= -1;
            cpu_score++;
            ResetBall();
        }

        if (x - radius <= 0) { // Player wins
            speedX *= -1;
            player_score++;
            ResetBall();
        }

        // Handle boost activation
        if (IsKeyPressed(KEY_B) && !boost_active) {    // Boost activation by 'B'
            countdown = 3; // Start countdown from 3 when 'B' is pressed
        }

        if (countdown > 0) {    // Countdown logic
            boost_timer++;
            if (boost_timer % 60 == 0) { // Countdown timer, decrease every second
                countdown--;
            }
        } else if (countdown == 0 && !boost_active && boost_timer != 0) {  // When countdown ends
            Boost(); // Boost the ball after countdown
            boost_timer = 0; // Reset timer for boost duration
        }

        // Disable boost after 10 seconds
        if (boost_active && boost_timer >= 600) {  // 600 frames = 10 seconds at 60 FPS
            speedX = originalSpeedX;
            speedY = originalSpeedY;
            boost_active = false; // Reset boost state
            boost_timer = 0; // Reset timer after boost ends
        }

        if (boost_active) {   // If boost is active, increment the timer
            boost_timer++;
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = { -1, 1 };
        speedX = originalSpeedX * speed_choices[GetRandomValue(0, 1)];
        speedY = originalSpeedY * speed_choices[GetRandomValue(0, 1)];

        boost_timer = 0;    // Reset boost timer when ball resets
        boost_active = false; // Reset boost when ball resets
    }

    void Boost() {   // Boost function
        boost_active = true;
        speedX *= 1.5;
        speedY *= 1.5;
    }
};

class Paddle {
protected:
    void LimitMovement() {
        if (y <= 0) y = 0;
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{ x, y, width, height }, 0.8, 0, WHITE);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) {
            y = y - speed;
        } else if (IsKeyDown(KEY_DOWN)) {
            y = y + speed;
        }

        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
public:
    void Update(int ball_y) {
        // Check if boost is active and adjust speed accordingly
        int currentSpeed = boost_active ? speed * 1.5 : speed;  // Modified: Speed boost if active

        if (y + height / 2 > ball_y) {
            y = y - currentSpeed;  // Updated to use currentSpeed
        }
        if (y + height / 2 <= ball_y) {
            y = y + currentSpeed;  // Updated to use currentSpeed
        }
        LimitMovement();
    }
};


Paddle player;
CpuPaddle cpu;
Ball ball;

int main() {
    cout << "Starting the game" << endl;

    const int screenWidth = 1280;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "My Pong Game!");
    SetTargetFPS(60);

    ball.radius = 20;
    ball.x = screenWidth / 2;
    ball.y = screenHeight / 2;
    ball.speedX = 7;
    ball.speedY = 7;
    ball.originalSpeedX = ball.speedX;   // Store original speed
    ball.originalSpeedY = ball.speedY;   // Store original speed

    player.width = 25;
    player.height = 120;
    player.x = screenWidth - player.width - 10;
    player.y = screenHeight / 2 - player.height / 2;
    player.speed = 6;

    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screenHeight / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (!WindowShouldClose()) {  // Main game loop
        BeginDrawing();

        // Updating...
        ball.Update();
        player.Update();
        cpu.Update(ball.y);

        // Checking for collision
        if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, Rectangle{ player.x, player.y, player.width, player.height })) {
            ball.speedX *= -1;
        }

        if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, Rectangle{ cpu.x, cpu.y, cpu.width, cpu.height })) {
            ball.speedX *= -1;
        }

        // Drawing 
        ClearBackground(DarkGreen);
        DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, Green);
        DrawCircle(screenWidth / 2, screenHeight / 2, 150, LightGreen);
        ball.Draw();
        cpu.Draw();
        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
        player.Draw();
        DrawText(TextFormat("%i", cpu_score), screenWidth / 4, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screenWidth / 4, 20, 80, WHITE);

        // Display countdown before boost
        if (countdown > 0) {    // Drawing countdown on screen
            DrawText(TextFormat("%i", countdown), screenWidth / 2 - 20, screenHeight / 2 - 100, 80, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
