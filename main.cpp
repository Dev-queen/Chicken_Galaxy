#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

/*

Team Members:
    Mariam Alaa - 211002295
    Youmna Yasser - 211008957
    Nourhan Hamdy - 211001466

Teaching Assistant:
    Menna Marghany

*/

int phyWidth = 700;
int phyHeight = 700;
int logWidth = 100;
int logHeight = 100;
int centerX = logWidth / 2;
int centerY = logHeight / 2;
float shipX = 0;
float shipY = 0;

void DrawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);//get the current angle
        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component
        glVertex2f(x + cx, y + cy);//output vertex
    }
    glEnd();
}

void drawFilledOval(float cx, float cy, float rx, float ry, float r, float g, float b, float angleDegrees) {
    float angleRadians = angleDegrees * (3.14159f / 180.0f);
    float cosA = cosf(angleRadians);
    float sinA = sinf(angleRadians);

    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy); // center

    for (int i = 0; i <= 360; i++) {
        float theta = (3.14159f / 180.0f) * i;
        float x = rx * cosf(theta);
        float y = ry * sinf(theta);

        // Rotate (x, y) around center
        float xr = x * cosA - y * sinA;
        float yr = x * sinA + y * cosA;

        glVertex2f(cx + xr, cy + yr);
    }

    glEnd();
}

void drawOvalOutline(float cx, float cy, float rx, float ry, float angle, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float theta = (3.14159f / 180.0f) * i;
        float x = rx * cosf(theta);
        float y = ry * sinf(theta);

        float rotatedX = cosf(angle * 3.14159f / 180.0f) * x - sinf(angle * 3.14159f / 180.0f) * y;
        float rotatedY = sinf(angle * 3.14159f / 180.0f) * x + cosf(angle * 3.14159f / 180.0f) * y;

        glVertex2f(cx + rotatedX, cy + rotatedY);
    }
    glEnd();
}

float scaleAround(float origin, float value, float scale) {
    return origin + (value - origin) * scale;
}

void drawShip(float originX, float originY, float scale, float shipX, float shipY) {
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_QUADS);
        glVertex2i(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
        glVertex2i(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 50 + shipY, scale));
        glVertex2i(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 50 + shipY, scale));
        glVertex2i(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2i(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 50 + shipY, scale));
        glVertex2i(scaleAround(originX, 50 + shipX, scale), scaleAround(originY, 65 + shipY, scale));
        glVertex2i(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 50 + shipY, scale));
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2i(scaleAround(originX, 25 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
        glVertex2i(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 35 + shipY, scale));
        glVertex2i(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2i(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
        glVertex2i(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 35 + shipY, scale));
        glVertex2i(scaleAround(originX, 75 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
        glVertex2i(scaleAround(originX, 33 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
        glVertex2i(scaleAround(originX, 33 + shipX, scale), scaleAround(originY, 30 + shipY, scale));
        glVertex2i(scaleAround(originX, 36 + shipX, scale), scaleAround(originY, 30 + shipY, scale));
        glVertex2i(scaleAround(originX, 36 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    glBegin(GL_QUADS);
        glVertex2i(scaleAround(originX, 27 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
        glVertex2i(scaleAround(originX, 27 + shipX, scale), scaleAround(originY, 20 + shipY, scale));
        glVertex2i(scaleAround(originX, 30 + shipX, scale), scaleAround(originY, 20 + shipY, scale));
        glVertex2i(scaleAround(originX, 30 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    glBegin(GL_QUADS);
        glVertex2i(scaleAround(originX, 63 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
        glVertex2i(scaleAround(originX, 63 + shipX, scale), scaleAround(originY, 30 + shipY, scale));
        glVertex2i(scaleAround(originX, 67 + shipX, scale), scaleAround(originY, 30 + shipY, scale));
        glVertex2i(scaleAround(originX, 67 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    glBegin(GL_QUADS);
        glVertex2i(scaleAround(originX, 69 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
        glVertex2i(scaleAround(originX, 69 + shipX, scale), scaleAround(originY, 20 + shipY, scale));
        glVertex2i(scaleAround(originX, 73 + shipX, scale), scaleAround(originY, 20 + shipY, scale));
        glVertex2i(scaleAround(originX, 73 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2i(scaleAround(originX, 40 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
        glVertex2i(scaleAround(originX, 50 + shipX, scale), scaleAround(originY, 25 + shipY, scale));
        glVertex2i(scaleAround(originX, 60 + shipX, scale), scaleAround(originY, 10 + shipY, scale));
    glEnd();

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
        1, 0, 0, 0
    );
}



void drawChicken(float originX, float originY, float scale) {
    // Legs
    double lx1 = scaleAround(originX, 47, scale);
    double ly1 = scaleAround(originY, 20, scale);
    drawFilledOval(lx1, ly1, 2 * scale, 0.9 * scale, 1, 0.69, 0.12, 60);

    double lx2 = scaleAround(originX, 45.5, scale);
    double ly2 = scaleAround(originY, 19, scale);
    drawFilledOval(lx2, ly2, 1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, 20);

    double lx3 = scaleAround(originX, 46, scale);
    double ly3 = scaleAround(originY, 18.5, scale);
    drawFilledOval(lx3, ly3, 1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, 60);

    double lx4 = scaleAround(originX, 47, scale);
    drawFilledOval(lx4, ly3, 1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, 90);

    double rx1 = scaleAround(originX, 53, scale);
    double ry1 = scaleAround(originY, 20, scale);
    drawFilledOval(rx1, ry1, 2 * scale, 0.9 * scale, 1, 0.69, 0.12, -60);

    double rx2 = scaleAround(originX, 54.5, scale);
    double ry2 = scaleAround(originY, 19, scale);
    drawFilledOval(rx2, ry2, 1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, -20);

    double rx3 = scaleAround(originX, 54, scale);
    double ry3 = scaleAround(originY, 18.5, scale);
    drawFilledOval(rx3, ry3, 1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, -60);

    double rx4 = scaleAround(originX, 53, scale);
    drawFilledOval(rx4, ry3, 1.4 * scale, 0.8 * scale, 1, 0.69, 0.12, -90);

    // Wings (Left and Right)
    for (int i = 0; i < 4; ++i) {
        float y = 45 - i * 2;
        drawFilledOval(scaleAround(originX, 40, scale), scaleAround(originY, y, scale), 15 * scale, 4 * scale, 1, 1, 1, i == 0 ? 0 : i * 20 - 5);
        drawFilledOval(scaleAround(originX, 60, scale), scaleAround(originY, y, scale), 15 * scale, 4 * scale, 1, 1, 1, i == 0 ? 0 : -(i * 20 - 5));
    }

    // Body
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 37, scale), 9 * scale, 17 * scale, 1, 1, 1, 0);
    drawOvalOutline(scaleAround(originX, 50, scale), scaleAround(originY, 37, scale), 9 * scale, 17 * scale, 0, 0, 0, 0);

    // Face
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 69, scale), 1 * scale, 2 * scale, 1, 0, 0, 0);
    drawOvalOutline(scaleAround(originX, 50, scale), scaleAround(originY, 69, scale), 1 * scale, 2 * scale, 0, 0, 0, 0);
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 60, scale), 4.5 * scale, 8 * scale, 1, 1, 1, 0);
    drawOvalOutline(scaleAround(originX, 50, scale), scaleAround(originY, 60, scale), 4.5 * scale, 8 * scale, 0, 0, 0, 0);
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 57, scale), 9 * scale, 4 * scale, 1, 1, 1, 0);
    drawOvalOutline(scaleAround(originX, 50, scale), scaleAround(originY, 57, scale), 9 * scale, 4 * scale, 0, 0, 0, 0);

    // Eyes
    for (int dx = -1; dx <= 1; dx += 2) {
        float eyeX = scaleAround(originX, 50 + dx, scale);
        float eyeY = scaleAround(originY, 62, scale);
        drawFilledOval(eyeX, eyeY, 1.5 * scale, 3 * scale, 0, 0, 0, 0);
        drawFilledOval(eyeX, eyeY, 1.3 * scale, 2.8 * scale, 1, 1, 1, 0);
        drawFilledOval(eyeX, eyeY, 1 * scale, 2 * scale, 0, 0, 0, 0);
    }

    // Eyebrows
    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(scaleAround(originX, 50, scale), scaleAround(originY, 63, scale));
        glVertex2f(scaleAround(originX, 53, scale), scaleAround(originY, 65, scale));
        glVertex2f(scaleAround(originX, 47, scale), scaleAround(originY, 65, scale));
        glVertex2f(scaleAround(originX, 50, scale), scaleAround(originY, 63, scale));
    glEnd();

    // Beak
    float x1 = scaleAround(originX, 46, scale);
    float x2 = scaleAround(originX, 54, scale);
    float y1 = scaleAround(originY, 56.5, scale);
    float y2 = scaleAround(originY, 57.5, scale);
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 53, scale), 1.5 * scale, 2.5 * scale, 1, 0, 0, 0);
    drawFilledOval(scaleAround(originX, 50, scale), scaleAround(originY, 57, scale), 4 * scale, 2 * scale, 1, 0.69, 0.12, 0);
    glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x1, y2);
        glVertex2f(x2, y2);
        glVertex2f(x2, y1);
    glEnd();
}


void handleKeypress(unsigned char key, int x, int y) {
    float moveSpeed = 2.0f;
    if (key == 'a' || key == 'A') shipX -= moveSpeed;
    if (key == 'd' || key == 'D') shipX += moveSpeed;
    if (key == 'w' || key == 'W') shipY += moveSpeed;
    if (key == 's' || key == 'S') shipY -= moveSpeed;
    glutPostRedisplay();
}

void handleSpecialKeypress(int key, int x, int y) {
    float moveSpeed = 2.0f;
    if (key == GLUT_KEY_LEFT) shipX -= moveSpeed;
    if (key == GLUT_KEY_RIGHT) shipX += moveSpeed;
    if (key == GLUT_KEY_UP) shipY += moveSpeed;
    if (key == GLUT_KEY_DOWN) shipY -= moveSpeed;
    glutPostRedisplay();
}

const int NUM_STARS = 100;
float starX[NUM_STARS];
float starY[NUM_STARS];

void initStars() {
    srand(time(0));
    for (int i = 0; i < NUM_STARS; ++i) {
        starX[i] = (float)(rand() % 100);
        starY[i] = (float)(rand() % 100);
    }
}

void updateStars() {
    for (int i = 0; i < NUM_STARS; ++i) {
        starY[i] -= 0.5f;
        if (starY[i] < 0) {
            starY[i] = 100;
            starX[i] = (float)(rand() % 100);
        }
    }
}

void drawStars() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < NUM_STARS; ++i) {
        glVertex2f(starX[i], starY[i]);
    }
    glEnd();
}

void timer(int) {
    updateStars();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void printSome(char *str,int x,int y) {
    glColor3f (0, 1.0, 0.0);
    glRasterPos2d(x,y);
    for (int i=0;i<strlen(str);i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
    glFlush();
}

void init() {
    glClearColor(0, 0, 0.3, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, logWidth, 0.0, logHeight);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawStars();
    drawChicken(50,90,0.4);
    drawShip(50,10,0.5,shipX,shipY);
    printSome("Timer: 0", 2, 96);
    printSome("Score: 0", 88, 2);
    glutSwapBuffers();
    glFlush();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(phyWidth, phyHeight);
    glutCreateWindow("Chicken Invaders");
    init();
    initStars();
    glutDisplayFunc(Display);
    glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(handleSpecialKeypress);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
