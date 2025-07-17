#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>   // For string formatting
#include <cstring>  // For string operations
#include <algorithm> // For std::max

/*

Team Members:
    Mariam Alaa - 211002295
    Youmna Yasser - 211008957
    Nourhan Hamdy - 211001466

Teaching Assistant:
    Menna Marghany

*/

// ========================
// GLOBAL CONSTANTS & STATE
// ========================

// Game window dimensions
const int phyWidth = 700;   // Physical window width in pixels
const int phyHeight = 700;  // Physical window height in pixels
const int logWidth = 100;   // Logical coordinate system width
const int logHeight = 100;  // Logical coordinate system height

// Game state variables
bool isGameRunning = false; // Whether game is active
int gameTime = 0;           // Game duration in seconds
float accumulatedTime = 0.0f;
int status = 0;             // 0 = home screen, 1 = gameplay
bool gameOver = false;      // Game over state

// Player ship position
float shipX = 0;            // Ship X position offset
float shipY = 0;            // Ship Y position offset
int mouseXX, mouseYY;       // Last mouse position

// Chicken enemy state
const int MAX_CHICKEN_HEALTH = 300;
int chickenHealth = MAX_CHICKEN_HEALTH;
bool chickenHit = false;    // Chicken was recently hit
float chickenHitTimer = 0.0f;
float chickenX = 50.0f;     // Chicken position
float chickenSpeed = 0.5f;  // Chicken movement speed
float chickenDirection = 1.0f; // Current movement direction
const float CHICKEN_MIN_X = 20.0f; // Left movement boundary
const float CHICKEN_MAX_X = 80.0f; // Right movement boundary

// Hit feedback
bool showOuch = false;      // Show damage indicator
float ouchTimer = 0.0f;     // Damage indicator timer
float ouchX = 0.0f, ouchY = 0.0f; // Damage position

// UI elements
int score = 0;              // Player score
const int HIT_SCORE = 100;  // Points per chicken hit
int spaceshipHealth = 100;  // Player health

// Menu buttons
float playBtnX = 50, playBtnY = 40, playBtnW = 30, playBtnH = 10;
float exitBtnX = 50, exitBtnY = 25, exitBtnW = 30, exitBtnH = 10;

// Background stars
const int NUM_STARS = 100;  // Number of background stars
float starX[NUM_STARS];     // Star positions
float starY[NUM_STARS];

// Projectile systems
struct Bullet {
    float x, y;             // Position
    float speed;            // Movement speed
    bool active;            // Is currently active
};

struct ChickenAttack {
    float x, y;             // Position
    float speed;            // Movement speed
    bool active;            // Is currently active
};

// Bullet management
const int MAX_BULLETS = 100;          // Max player bullets
Bullet bullets[MAX_BULLETS];          // Player bullet array
const int MAX_CHICKEN_EGGS = 50;      // Max enemy projectiles
ChickenAttack chickenBullets[MAX_CHICKEN_EGGS]; // Enemy projectiles
float lastChickenShotTime = 0.0f;     // Time since last enemy shot

// =====================
// FUNCTION DECLARATIONS
// =====================
void drawText(float x, float y, const char *text, void *font);
void drawStars();
void drawHomeScreen();
void DisplayGameOver();
void mouseClick(int btn, int state, int x, int y);
void DrawCircle(float cx, float cy, float r, int num_segments);
void drawFilledOval(float cx, float cy, float rx, float ry, float r, float g, float b, float angleDegrees);
void drawOvalOutline(float cx, float cy, float rx, float ry, float angle, float r, float g, float b);
float scaleAround(float origin, float value, float scale);
void drawShip(float originX, float originY, float scale, float shipX, float shipY);
void drawChicken(float originX, float originY, float scale);
void handleKeypress(unsigned char key, int x, int y);
void handleSpecialKeypress(int key, int x, int y);
void initStars();
void updateStars();
void updateBullets();
bool checkCollisionWithShip(const ChickenAttack& bullet);
void updateChickenBullets(float deltaTime);
void DrawChickenEggs();
void timer(int);
void printSome(char *str, int x, int y);
void DisplaySpaceshipHealth();
void DrawBullets();
void DisplayHealth();
void DisplayScore();
void OUCH();
void GameOverMessage();
void drawGameplay();
void init();
void Display();

// ======================
// CORE GAME FUNCTIONALITY
// ======================

// Draw text at specified position
void drawText(float x, float y, const char *text, void *font) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
}

// Initialize background stars
void initStars() {
    srand(time(0)); // Seed random number generator
    for (int i = 0; i < NUM_STARS; ++i) {
        starX[i] = static_cast<float>(rand() % 100);
        starY[i] = static_cast<float>(rand() % 100);
    }
}

// Draw background stars
void drawStars() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f); // White stars
    for (int i = 0; i < NUM_STARS; ++i) {
        glVertex2f(starX[i], starY[i]);
    }
    glEnd();
}

// Update star positions (scrolling effect)
void updateStars() {
    for (int i = 0; i < NUM_STARS; ++i) {
        starY[i] -= 0.5f; // Move stars downward
        // Reset stars that move off-screen
        if (starY[i] < 0) {
            starY[i] = 100;
            starX[i] = static_cast<float>(rand() % 100);
        }
    }
}

// Draw home screen with menu
void drawHomeScreen() {
    // Blue gradient background
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.1f, 0.3f); // Top color
    glVertex2f(0, logHeight);
    glVertex2f(logWidth, logHeight);
    glColor3f(0.0f, 0.3f, 0.6f); // Bottom color
    glVertex2f(logWidth, 0);
    glVertex2f(0, 0);
    glEnd();

    drawStars(); // Draw background stars

    // Title text
    glColor3f(1.0f, 1.0f, 0.2f); // Yellow title
    drawText(30, 70, "Welcome to Chicken Invaders!", GLUT_BITMAP_TIMES_ROMAN_24);

    // Subtitle
    glColor3f(1.0f, 1.0f, 1.0f); // White text
    drawText(30, 60, "Are you ready to roast some chickens?", GLUT_BITMAP_HELVETICA_18);

    // Play Button
    glColor3f(0.9f, 0.2f, 0.2f); // Red button
    glBegin(GL_QUADS);
    glVertex2f(playBtnX - playBtnW/2, playBtnY - playBtnH/2);
    glVertex2f(playBtnX + playBtnW/2, playBtnY - playBtnH/2);
    glVertex2f(playBtnX + playBtnW/2, playBtnY + playBtnH/2);
    glVertex2f(playBtnX - playBtnW/2, playBtnY + playBtnH/2);
    glEnd();

    // Play Button Text
    glColor3f(1, 1, 1); // White text
    drawText(playBtnX - 6, playBtnY, "Play Now", GLUT_BITMAP_HELVETICA_18);

    // Exit Button
    glColor3f(0.9f, 0.2f, 0.2f); // Red button
    glBegin(GL_QUADS);
    glVertex2f(exitBtnX - exitBtnW/2, exitBtnY - exitBtnH/2);
    glVertex2f(exitBtnX + exitBtnW/2, exitBtnY - exitBtnH/2);
    glVertex2f(exitBtnX + exitBtnW/2, exitBtnY + exitBtnH/2);
    glVertex2f(exitBtnX - exitBtnW/2, exitBtnY + exitBtnH/2);
    glEnd();

    // Exit Button Text
    glColor3f(1, 1, 1); // White text
    drawText(exitBtnX - 3, exitBtnY - 1, "Exit", GLUT_BITMAP_HELVETICA_18);
}

// Draw game over screen
void DisplayGameOver() {
    // Semi-transparent overlay
    glColor4f(0.0, 0.0, 0.0, 0.7);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(logWidth, 0);
    glVertex2f(logWidth, logHeight);
    glVertex2f(0, logHeight);
    glEnd();

    // Game over message
    if (spaceshipHealth <= 0) {
        glColor3f(1.0, 0.2, 0.2); // Red text
        drawText(20, 60, "YOU LOST AGAINST THE CHICKEN!", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(1.0, 1.0, 1.0); // White text
        drawText(30, 40, "Try again? (Press R to restart)", GLUT_BITMAP_HELVETICA_18);
    }

    // Final score display
    char finalScore[50];
    sprintf(finalScore, "Final Score: %d", score);
    glColor3f(1.0, 1.0, 0.0); // Yellow text
    drawText(40, 30, finalScore, GLUT_BITMAP_HELVETICA_18);
}

// Handle mouse clicks
void mouseClick(int btn, int state, int x, int y) {
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert to logical coordinates
        mouseXX = x;
        mouseYY = phyHeight - y; // Flip y-coordinate
        float logX = static_cast<float>(mouseXX) * logWidth / phyWidth;
        float logY = static_cast<float>(mouseYY) * logHeight / phyHeight;

        // Play button click
        if (logX >= playBtnX - playBtnW/2 && logX <= playBtnX + playBtnW/2 &&
            logY >= playBtnY - playBtnH/2 && logY <= playBtnY + playBtnH/2) {
            status = 1; // Switch to gameplay
            isGameRunning = true;
            gameTime = 0;
        }
        // Exit button click
        else if (logX >= exitBtnX - exitBtnW/2 && logX <= exitBtnX + exitBtnW/2 &&
                 logY >= exitBtnY - exitBtnH/2 && logY <= exitBtnY + exitBtnH/2) {
            exit(0); // Quit application
        }

        glutPostRedisplay();
    }
}

// =================
// DRAWING UTILITIES
// =================

// Draw a filled circle
void DrawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * i / num_segments;
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
    }
    glEnd();
}

// Draw a filled oval/ellipse
void drawFilledOval(float cx, float cy, float rx, float ry,
                   float r, float g, float b, float angleDegrees) {
    float angleRadians = angleDegrees * (3.14159f / 180.0f);
    float cosA = cosf(angleRadians);
    float sinA = sinf(angleRadians);

    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy); // Center point

    // Generate oval points
    for (int i = 0; i <= 360; i++) {
        float rad = i * (3.14159f / 180.0f);
        float x = rx * cosf(rad);
        float y = ry * sinf(rad);
        // Rotate point
        float xr = x * cosA - y * sinA;
        float yr = x * sinA + y * cosA;
        glVertex2f(cx + xr, cy + yr);
    }
    glEnd();
}

// Draw oval outline
void drawOvalOutline(float cx, float cy, float rx, float ry,
                    float angle, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
    float radAngle = angle * 3.14159f / 180.0f;
    for (int i = 0; i < 360; i++) {
        float rad = i * (3.14159f / 180.0f);
        float x = rx * cosf(rad);
        float y = ry * sinf(rad);
        // Rotate point
        float rotatedX = cosf(radAngle) * x - sinf(radAngle) * y;
        float rotatedY = sinf(radAngle) * x + cosf(radAngle) * y;
        glVertex2f(cx + rotatedX, cy + rotatedY);
    }
    glEnd();
}

// Scale point relative to an origin
float scaleAround(float origin, float value, float scale) {
    return origin + (value - origin) * scale;
}

// Draw player spaceship
void drawShip(float originX, float originY, float scale, float shipX, float shipY) {
    // Main body (gray)
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_QUADS);
    glVertex2f(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glVertex2f(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 50 + shipY, scale));
    glVertex2f(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 50 + shipY, scale));
    glVertex2f(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    // Top fin
    glBegin(GL_TRIANGLES);
    glVertex2f(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 50 + shipY, scale));
    glVertex2f(scaleAround(originX, 50 + shipX, scale), scaleAround(originY, 65 + shipY, scale));
    glVertex2f(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 50 + shipY, scale));
    glEnd();

    // Left wing
    glBegin(GL_TRIANGLES);
    glVertex2f(scaleAround(originX, 25 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glVertex2f(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 35 + shipY, scale));
    glVertex2f(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    // Right wing
    glBegin(GL_TRIANGLES);
    glVertex2f(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glVertex2f(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 35 + shipY, scale));
    glVertex2f(scaleAround(originX, 75 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    // Left engine details (black)
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    // Left engine exhaust 1
    glVertex2f(scaleAround(originX, 33 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glVertex2f(scaleAround(originX, 33 + shipX, scale), scaleAround(originY, 30 + shipY, scale));
    glVertex2f(scaleAround(originX, 36 + shipX, scale), scaleAround(originY, 30 + shipY, scale));
    glVertex2f(scaleAround(originX, 36 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    // Left engine exhaust 2
    glVertex2f(scaleAround(originX, 27 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glVertex2f(scaleAround(originX, 27 + shipX, scale), scaleAround(originY, 20 + shipY, scale));
    glVertex2f(scaleAround(originX, 30 + shipX, scale), scaleAround(originY, 20 + shipY, scale));
    glVertex2f(scaleAround(originX, 30 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    // Right engine exhaust 1
    glVertex2f(scaleAround(originX, 63 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glVertex2f(scaleAround(originX, 63 + shipX, scale), scaleAround(originY, 30 + shipY, scale));
    glVertex2f(scaleAround(originX, 67 + shipX, scale), scaleAround(originY, 30 + shipY, scale));
    glVertex2f(scaleAround(originX, 67 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    // Right engine exhaust 2
    glVertex2f(scaleAround(originX, 69 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glVertex2f(scaleAround(originX, 69 + shipX, scale), scaleAround(originY, 20 + shipY, scale));
    glVertex2f(scaleAround(originX, 73 + shipX, scale), scaleAround(originY, 20 + shipY, scale));
    glVertex2f(scaleAround(originX, 73 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    // Cockpit area
    glBegin(GL_TRIANGLES);
    glVertex2f(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glVertex2f(scaleAround(originX, 50 + shipX, scale), scaleAround(originY, 25 + shipY, scale));
    glVertex2f(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    // Engine glow effects (red)
    drawFilledOval(
        scaleAround(originX, 50 + shipX, scale),
        scaleAround(originY, 13 + shipY, scale),
        6 * scale, 3 * scale,
        1, 0, 0, 0
    );
    drawFilledOval(
        scaleAround(originX, 50 + shipX, scale),
        scaleAround(originY, 43 + shipY, scale),
        4 * scale, 7 * scale,
        1, 0, 0, 0
    );
    drawOvalOutline(
        scaleAround(originX, 50 + shipX, scale),
        scaleAround(originY, 43 + shipY, scale),
        4 * scale, 7 * scale,
        0, 0, 0, 0
    );
}

// Draw chicken enemy
void drawChicken(float originX, float originY, float scale) {
    // Legs - left side
    drawFilledOval(scaleAround(originX, 47, scale), scaleAround(originY, 20, scale),
                  2 * scale, 0.9 * scale, 1, 0.69, 0.12, 60);
    drawFilledOval(scaleAround(originX, 45.5, scale), scaleAround(originY, 19, scale),
                  1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, 20);
    drawFilledOval(scaleAround(originX, 46, scale), scaleAround(originY, 18.5, scale),
                  1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, 60);
    drawFilledOval(scaleAround(originX, 47, scale), scaleAround(originY, 18.5, scale),
                  1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, 90);

    // Legs - right side
    drawFilledOval(scaleAround(originX, 53, scale), scaleAround(originY, 20, scale),
                  2 * scale, 0.9 * scale, 1, 0.69, 0.12, -60);
    drawFilledOval(scaleAround(originX, 54.5, scale), scaleAround(originY, 19, scale),
                  1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, -20);
    drawFilledOval(scaleAround(originX, 54, scale), scaleAround(originY, 18.5, scale),
                  1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, -60);
    drawFilledOval(scaleAround(originX, 53, scale), scaleAround(originY, 18.5, scale),
                  1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, -90);

    // Wings (layered effect)
    for (int i = 0; i < 4; ++i) {
        float y = 45 - i * 2;
        // Left wing
        drawFilledOval(scaleAround(originX, 40, scale), scaleAround(originY, y, scale),
                      15 * scale, 4 * scale, 1, 1, 1, i * 20 - 5);
        // Right wing
        drawFilledOval(scaleAround(originX, 60, scale), scaleAround(originY, y, scale),
                      15 * scale, 4 * scale, 1, 1, 1, -(i * 20 - 5));
    }

    // Main body (white)
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 37, scale),
                  9 * scale, 17 * scale, 1, 1, 1, 0);
    drawOvalOutline(scaleAround(originX, 50, scale), scaleAround(originY, 37, scale),
                  9 * scale, 17 * scale, 0, 0, 0, 0);

    // Face details

    // Face
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 69, scale), 1 * scale, 2 * scale, 1, 0, 0, 0);
    drawOvalOutline(scaleAround(originX, 50, scale), scaleAround(originY, 69, scale), 1 * scale, 2 * scale, 0, 0, 0, 0);
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 60, scale), 4.5 * scale, 8 * scale, 1, 1, 1, 0);
    drawOvalOutline(scaleAround(originX, 50, scale), scaleAround(originY, 60, scale), 4.5 * scale, 8 * scale, 0, 0, 0, 0);
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 57, scale), 9 * scale, 4 * scale, 1, 1, 1, 0);
    drawOvalOutline(scaleAround(originX, 50, scale), scaleAround(originY, 57, scale), 9 * scale, 4 * scale, 0, 0, 0, 0);

    // Beak base
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 53, scale),
                  1.5 * scale, 2.5 * scale, 1, 0, 0, 0);
    // Beak top
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 57, scale),
                  4 * scale, 2 * scale, 1, 0.69, 0.12, 0);
    // Beak opening
    glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(scaleAround(originX, 46, scale), scaleAround(originY, 56.5, scale));
    glVertex2f(scaleAround(originX, 46, scale), scaleAround(originY, 57.5, scale));
    glVertex2f(scaleAround(originX, 54, scale), scaleAround(originY, 57.5, scale));
    glVertex2f(scaleAround(originX, 54, scale), scaleAround(originY, 56.5, scale));
    glEnd();

    // Eyes
    for (int dx = -1; dx <= 1; dx += 2) { // dx = -1 for left, +1 for right
        float eyeX = scaleAround(originX, 50 + dx, scale);
        float eyeY = scaleAround(originY, 62, scale);
        // Outer eye (black)
        drawFilledOval(eyeX, eyeY, 1.5 * scale, 3 * scale, 0, 0, 0, 0);
        // Eye white
        drawFilledOval(eyeX, eyeY, 1.3 * scale, 2.8 * scale, 1, 1, 1, 0);
        // Pupil
        drawFilledOval(eyeX, eyeY, 1 * scale, 2 * scale, 0, 0, 0, 0);
    }

    // Eyebrows (angry look)
    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2f(scaleAround(originX, 50, scale), scaleAround(originY, 63, scale));
    glVertex2f(scaleAround(originX, 53, scale), scaleAround(originY, 65, scale));
    glVertex2f(scaleAround(originX, 47, scale), scaleAround(originY, 65, scale));
    glVertex2f(scaleAround(originX, 50, scale), scaleAround(originY, 63, scale));
    glEnd();
}

// =================
// INPUT HANDLING
// =================

// Keyboard input handler
void handleKeypress(unsigned char key, int x, int y) {
    const float moveSpeed = 2.0f; // Ship movement speed

    switch (key) {
        case ' ': // Spacebar - fire weapon
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) {
                    // Create new bullet at ship position
                    bullets[i].x = shipX + 50;
                    bullets[i].y = shipY + 30;
                    bullets[i].speed = 3.0f;
                    bullets[i].active = true;
                    break;
                }
            }
            break;

        case 'x': case 'X': // Exit game
            exit(0);
            break;

        case 'm': case 'M': // Exit game
            gameOver = false;
            spaceshipHealth = 100;
            score = 0;
            chickenHealth = MAX_CHICKEN_HEALTH;
            shipX = shipY = 0;
            // Clear all projectiles
            for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
            for (int i = 0; i < MAX_CHICKEN_EGGS; i++) chickenBullets[i].active = false;
            status = 0;
            break;

        case 'r': case 'R': // Restart game
            gameOver = false;
            spaceshipHealth = 100;
            score = 0;
            chickenHealth = MAX_CHICKEN_HEALTH;
            shipX = shipY = 0;
            // Clear all projectiles
            for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
            for (int i = 0; i < MAX_CHICKEN_EGGS; i++) chickenBullets[i].active = false;
            status = 1; // Switch to gameplay
            break;

        // Movement controls
        case 'a': case 'A': shipX -= moveSpeed; break; // Left
        case 'd': case 'D': shipX += moveSpeed; break; // Right
        case 'w': case 'W': shipY += moveSpeed; break; // Up
        case 's': case 'S': shipY -= moveSpeed; break; // Down
    }
    glutPostRedisplay();
}

// Special key input (arrow keys)
void handleSpecialKeypress(int key, int x, int y) {
    const float moveSpeed = 2.0f;
    switch (key) {
        case GLUT_KEY_LEFT:  shipX -= moveSpeed; break;
        case GLUT_KEY_RIGHT: shipX += moveSpeed; break;
        case GLUT_KEY_UP:    shipY += moveSpeed; break;
        case GLUT_KEY_DOWN:  shipY -= moveSpeed; break;
    }
    glutPostRedisplay();
}

// ===================
// GAME LOGIC FUNCTIONS
// ===================

// Update player bullets
void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].y += bullets[i].speed; // Move upward

            // Collision detection with chicken
            float dx = bullets[i].x - chickenX;
            float dy = bullets[i].y - 90; // Chicken position
            float distance = sqrt(dx*dx + dy*dy);

            if (distance < 15.0f) { // Hit radius
                bullets[i].active = false;
                chickenHealth = std::max(0, chickenHealth - 10);

                // Update score if chicken still alive
                if (chickenHealth > 0) {
                    score += HIT_SCORE;
                    chickenHit = true;
                    chickenHitTimer = 0.3f;

                    // Show damage indicator
                    showOuch = true;
                    ouchTimer = 0.5f;
                    ouchX = bullets[i].x;
                    ouchY = bullets[i].y;
                }

                // Clear enemy projectiles if chicken defeated
                if (chickenHealth <= 0) {
                    for (int j = 0; j < MAX_CHICKEN_EGGS; j++) {
                        chickenBullets[j].active = false;
                    }
                }
            }

            // Deactivate off-screen bullets
            if (bullets[i].y > logHeight) {
                bullets[i].active = false;
            }
        }
    }
}

// Check collision between enemy projectile and ship
bool checkCollisionWithShip(const ChickenAttack& bullet) {
    // Ship collision box
    float shipLeft = 50 + shipX - 15;
    float shipRight = 50 + shipX + 15;
    float shipTop = 10 + shipY + 30;
    float shipBottom = 10 + shipY;

    return (bullet.x > shipLeft && bullet.x < shipRight &&
            bullet.y > shipBottom && bullet.y < shipTop);
}

// Update enemy projectiles
void updateChickenBullets(float deltaTime) {
    // Only update if chicken is alive
    if (chickenHealth <= 0) return;

    lastChickenShotTime += deltaTime;

    // Shoot every 2 seconds
    if (lastChickenShotTime >= 2.0f) {
        lastChickenShotTime = 0.0f;

        // Find inactive projectile slot
        for (int i = 0; i < MAX_CHICKEN_EGGS; i++) {
            if (!chickenBullets[i].active) {
                chickenBullets[i].x = chickenX;
                chickenBullets[i].y = 70;
                chickenBullets[i].speed = -3.0f; // Move downward
                chickenBullets[i].active = true;
                break;
            }
        }
    }

    // Update active projectiles
    for (int i = 0; i < MAX_CHICKEN_EGGS; i++) {
        if (chickenBullets[i].active) {
            chickenBullets[i].y += chickenBullets[i].speed;

            // Ship collision
            if (checkCollisionWithShip(chickenBullets[i])) {
                chickenBullets[i].active = false;
                spaceshipHealth = std::max(0, spaceshipHealth - 10);

                // Check game over conditions
                if (spaceshipHealth <= 0 || chickenHealth <= 0) {
                    gameOver = true;
                }
            }

            // Deactivate off-screen projectiles
            if (chickenBullets[i].y < 0) {
                chickenBullets[i].active = false;
            }
        }
    }
}

// Draw enemy projectiles
void DrawChickenEggs() {
    glColor3f(1.0, 1.0, 1.0); // White eggs
    for (int i = 0; i < MAX_CHICKEN_EGGS; i++) {
        if (chickenBullets[i].active) {
            DrawCircle(chickenBullets[i].x, chickenBullets[i].y, 2.0, 20);
        }
    }
}

// Game timer callback
void timer(int) {
    // Update game elements
    updateStars();
    updateBullets();

    static float lastTime = 0.0f;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Update chicken position
    chickenX += chickenSpeed * chickenDirection;

    // Reverse direction at boundaries
    if (chickenX > CHICKEN_MAX_X) {
        chickenDirection = -1.0f;
        chickenX = CHICKEN_MAX_X;
    }
    else if (chickenX < CHICKEN_MIN_X) {
        chickenDirection = 1.0f;
        chickenX = CHICKEN_MIN_X;
    }

    updateChickenBullets(deltaTime);

    // Update game timer
    if (isGameRunning) {
        accumulatedTime += 16.0f;
        if (accumulatedTime >= 1000.0f) {
            gameTime++;
            accumulatedTime = 0.0f;
        }
    }

    // Update damage indicator timer
    if (showOuch) {
        ouchTimer -= deltaTime;
        if (ouchTimer <= 0) showOuch = false;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

// =================
// UI DISPLAY FUNCTIONS
// =================

// Draw text string at position
void printSome(char *str, int x, int y) {
    glColor3f(0, 1.0, 0.0); // Green text
    glRasterPos2d(x, y);
    for (size_t i = 0; i < strlen(str); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
    }
}

// Display player health
void DisplaySpaceshipHealth() {
    char healthStr[20];
    sprintf(healthStr, "Your Health: %d", spaceshipHealth);
    printSome(healthStr, 2, 2);
}

// Draw player bullets
void DrawBullets() {
    glColor3f(1.0, 0.0, 0.0); // Red bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawCircle(bullets[i].x, bullets[i].y, 2.0, 20);
        }
    }
}

// Display chicken health
void DisplayHealth() {
    char healthStr[30];
    sprintf(healthStr, "Health: %d/%d", chickenHealth, MAX_CHICKEN_HEALTH);
    printSome(healthStr, 80, 96); // Top-right position
}

// Display player score
void DisplayScore() {
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score);
    printSome(scoreStr, 80, 2); // Bottom-right position
}

// Display "OUCH!" damage indicator
void OUCH() {
    if (showOuch && chickenHealth > 0) {
        glColor3f(1.0, 0.0, 0.0); // Red text
        drawText(ouchX - 10, ouchY + 10, "OUCH! +100", GLUT_BITMAP_HELVETICA_18);
    }
}

// Display victory message
void GameOverMessage() {
    if (chickenHealth <= 0) {
        // Semi-transparent overlay
        glColor4f(0.0, 0.0, 0.0, 0.7);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(logWidth, 0);
        glVertex2f(logWidth, logHeight);
        glVertex2f(0, logHeight);
        glEnd();

        // Victory message
        glColor3f(0.0, 1.0, 0.0); // Green text
        drawText(33, 50, "CHICKEN DEFEATED!", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(1.0, 1.0, 1.0); // White text
        drawText(33, 40, "Play again? (Press R to restart)", GLUT_BITMAP_HELVETICA_18);

        // Final score display
        char finalScore[50];
        sprintf(finalScore, "Final Score: %d", score);
        drawText(40, 30, finalScore, GLUT_BITMAP_HELVETICA_18);
    }
}

// =================
// MAIN GAME SCREENS
// =================

// Draw gameplay screen
void drawGameplay() {
    // Background elements
    drawStars();

    // Game objects
    drawShip(50, 10, 0.5, shipX, shipY); // Player ship
    DrawBullets();                        // Player projectiles
    DrawChickenEggs();                    // Enemy projectiles
    drawChicken(chickenX, 90, 0.4f);      // Chicken enemy

    // UI elements
    DisplayHealth();            // Enemy health
    DisplayScore();             // Player score
    OUCH();                     // Damage indicators
    DisplaySpaceshipHealth();   // Player health

    // Game timer
    char timeStr[20];
    sprintf(timeStr, "Time: %02d:%02d", gameTime / 60, gameTime % 60);
    printSome(timeStr, 2, 96); // Top-left position

    GameOverMessage(); // Victory screen
}

// Main display callback
void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Select screen based on game state
    if (status == 0) {
        drawHomeScreen(); // Main menu
    }
    else if (status == 1) {
        drawGameplay(); // Game screen
    }

    // Show game over overlay if needed
    if (gameOver) {
        DisplayGameOver();
    }

    glutSwapBuffers();
    glFlush();
}

// Game initialization
void init() {
    glClearColor(0, 0, 0.3, 0.0); // Dark blue background
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, logWidth, 0.0, logHeight); // Set coordinate system

    // Initialize projectile arrays
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
    for (int i = 0; i < MAX_CHICKEN_EGGS; i++) {
        chickenBullets[i].active = false;
    }
}

// =============
// MAIN FUNCTION
// =============
int main(int argc, char *argv[]) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(phyWidth, phyHeight);
    glutCreateWindow("Chicken Invaders");

    // Initialize game state
    init();
    initStars(); // Create background stars

    // Register callbacks
    glutDisplayFunc(Display);
    glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(handleSpecialKeypress);
    glutMouseFunc(mouseClick);
    glutTimerFunc(0, timer, 0); // Start game timer

    glutMainLoop(); // Start main loop
    return 0;
}
