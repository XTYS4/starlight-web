
#include "raylib.h"
#include <vector>

struct Enemy {
    float x = 0.0f, y = 0.0f;
    float speed = 4;
    float size = 20;
    int damage = 20;
    Color glow = { 255,255,255,255 };
};

int main()
{

    bool inMenu = true;

    bool gameEnd = false;

    double spawnTimer = 0;
    bool firstEspawned = false;
    double spawnInterval = 1.0;

    double startTime = GetTime();

    SetTargetFPS(60);  // Lock to 60 FPS

    InitWindow(0, 0, "Space Dodge"); // Let raylib pick screen resolution
    ToggleFullscreen(); // Immediately go fullscreen

    int wX = GetScreenWidth();
    int wY = GetScreenHeight();
    int mX = wX / 2;
    int mY = wY / 2;

    Color GameBCG = { 0,0,0, 255 };  // R, G, B, Alpha

    Color plrC = { 13, 181, 21, 255 };

    float plrSize = 40, posX = mX - plrSize / 3, posY = wY - plrSize - 80;
    float maxspeed = 12;
    float basespeed = 4.5;
    float speed = basespeed;
    float speedinc = 0.05;
    bool moving = false;

    bool playerDied = false;
    int plrHealth = 100;

    float eAverageSpeed = 5;
    float eAverageSpeedInc = 0.1f;

    int enemycount = 0;

    std::vector<Enemy> enemies;

    Texture2D earth = LoadTexture("earth.png");

    int life = 6;

    double prevtime = 0.0;

    double bestTime = 0.0;

    while (!WindowShouldClose())
    {
        if (inMenu)
        {
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("STARLIGHT", mX - 300, mY - 150, 100, GREEN);
            DrawText("BY PEDRO", mX - 170, mY - 10, 60, {0,255,0,200});
            DrawText("Press [ENTER] to Play", mX - 240, mY + 150, 40, LIGHTGRAY);

            if (IsKeyPressed(KEY_ENTER))
            {
                inMenu = false;               // ✅ Switch to game
                startTime = GetTime();        // Reset the timer
                spawnTimer = GetTime();       // Optional: reset spawn delay
            }

            EndDrawing();
            continue;  // ⛔ Skip the rest of the game loop if still in menu
        }


        double currentTime = GetTime() - startTime;

        // UPDATE prevtime while alive
        if (life > 0 && !gameEnd) {
            prevtime = currentTime;
        }

        if (life <= 0 && prevtime > bestTime) {
            bestTime = prevtime;
        }



        BeginDrawing();

        DrawFPS(wX - 80, 20);

        ClearBackground(GameBCG);

        //GAME INFO
        DrawText(TextFormat("STAR COUNT: %d", enemycount), 20, 50, 30, GREEN);
        DrawText(TextFormat("SPEED: %.2f", speed), 20, 90, 30, GREEN);
        DrawText(TextFormat("MOVING: %s", moving ? "Yes" : "No"), 20, 130, 30, GREEN);
        DrawText(TextFormat("ENEMY AVERAGE SPEED: %.1f", eAverageSpeed), 20, 170, 30, GREEN);
        DrawText(TextFormat("LIVES: %d", life), 20, 210, 30, RED);

        DrawText("TIME", mX - 130, mY - 50, 100, { 0,255,0,35 });
        DrawText(TextFormat("%.2f", prevtime), mX - 80, mY + 50, 80, { 0,255,0,35 });

        DrawText(TextFormat("BEST: %.2f", bestTime), wX - 250, 50, 30, GREEN);

        // GROUND

        DrawRectangle(0, wY - 50, wX, 50, { 20,20,20 });

        // PLAYER

        DrawRectangle(posX, posY, plrSize, plrSize, plrC);
        //DrawText(TextFormat("X: %.2f   Y: %.2f", posX, posY), mX - 70, wY - 40, 20, WHITE);

        // PLAYER HEALTH TEXT
        //DrawText(TextFormat("%d", plrHealth), posX + 5, posY + 11, 20, plrHealthC);

        // Movement input

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            posX -= speed;
            moving = true;
            if (speed <= maxspeed) { speed += speedinc; }
            else { speed = maxspeed; }
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            posX += speed;
            moving = true;
            if (speed <= maxspeed) { speed += speedinc; }
            else { speed = maxspeed; }
        }
        if (!(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))) {
            speed = basespeed;
            moving = false;
        }

        if (posX < 10) posX = 10;
        if (posX > wX - 10 - plrSize) posX = wX - 10 - plrSize;

        // SPAWN ENEMY FOR EVERY ENEMY COUNT

        if (((GetTime() - spawnTimer >= spawnInterval) || !(firstEspawned)) && !gameEnd) {
            spawnTimer = GetTime(); // reset timer
            eAverageSpeed += eAverageSpeedInc + GetRandomValue(1, 3) / 10.0f;
            firstEspawned = true;

            // Spawn a new enemy
            Enemy newEnemy;
            newEnemy.size = (float)GetRandomValue(3, 10);
            newEnemy.x = GetRandomValue(10, wX - newEnemy.size * 2);
            newEnemy.y = 0;
            newEnemy.speed = (float)GetRandomValue(eAverageSpeed, eAverageSpeed + 3);

            newEnemy.damage = 20;

            Color newGlow = { 255, 255, 255, (unsigned char)GetRandomValue(230, 255) };
            newEnemy.glow = newGlow;

            enemies.push_back(newEnemy);

            enemycount++; // increase the count (optional)
        }


        // MOVE DOWN EVERY ENEMY BLOCK IN ARRAY
        for (int i = 0; i < enemies.size(); i++) {
            DrawCircle(enemies[i].x, enemies[i].y, enemies[i].size, enemies[i].glow);

            // MOVE ENEMY
            enemies[i].y += enemies[i].speed;

            Vector2 enemyCenter = { enemies[i].x, enemies[i].y };
            Rectangle playerRect = { posX, posY, plrSize, plrSize };
            float radius = enemies[i].size;

            //TRACK DAMAGE/COLISSION
            if (CheckCollisionCircleRec(enemyCenter, radius, playerRect)) {
                DrawText("COLLISION!", 100, 100, 30, RED);

                plrHealth -= enemies[i].damage;

                life -= 1;

                if (!gameEnd) {
                    enemies[i].y = 0;
                    enemies[i].x = GetRandomValue(0, wX - enemies[i].size * 2);
                    enemies[i].speed = GetRandomValue(eAverageSpeed, eAverageSpeed + 3);
                }
            }

            if (enemies[i].y >= wY) {
                if (!gameEnd) {
                    enemies[i].y = 0;
                    enemies[i].x = GetRandomValue(0, wX - enemies[i].size * 2);
                    enemies[i].speed = GetRandomValue(eAverageSpeed, eAverageSpeed + 3);
                }
            }

        }

        // PLAYER HEALTH TEXT COLOR MANAGER

        switch (plrHealth) {
        case 100:
            plrC = { 13, 181, 21,255 };
            break;
        case 80:
            plrC = { 125, 181, 13,255 };
            break;
        case 60:
            plrC = { 181, 181, 13,255 };
            break;
        case 40:
            plrC = { 181, 117, 13, 255 };
            break;
        case 20:
            plrC = { 181, 83, 13,255 };
            break;
        case 0:
            plrC = { 255,0,0,255 };
            break;

        default:
            plrC = { 255,255,255,0 };
            break;
        }

        if (life <= 0) {
            life = 0; speed = 0; gameEnd = true; moving = false;
            DrawText("GAME OVER", mX - 585, mY - 300, 200, { 255,0,0,200 });
            DrawText("Press [R] to Retry", mX - 885, mY + 300, 200, { 255,0,0,150 });

            if (IsKeyPressed(KEY_R)) {
                // RESET EVERYTHING
                enemies.clear();
                life = 6;
                plrHealth = 100;
                speed = basespeed;
                posX = mX;
                enemycount = 0;
                eAverageSpeed = 5;
                spawnTimer = GetTime();
                startTime = GetTime();
                gameEnd = false;
            }
        }

        



        EndDrawing();
    }

    UnloadTexture(earth);
    CloseWindow();
    return 0;
}
