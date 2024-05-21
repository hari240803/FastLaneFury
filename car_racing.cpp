#include <GL/freeglut.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <fstream>

bool updateGame = true;

int i, q;
unsigned long long score = 0;
unsigned long long highScore = 0;
int screen = 0;
int speed = 10;
int originalTimerInterval = 20;
int speedIncrease = 10;
bool collide = false;
char buffer[10];
const char* highScoreFileName = "highscore.txt";

int vehicleX = 200, vehicleY = 70;
int ovehicleX[4], ovehicleY[4];
int divx = 250, divy = 4, movd;

bool keyStates[256];
bool gameRunning = true;
bool pauseScreen = false;
bool gameOver = false;
bool gameStarted = false;

void drawText(const char ch[], int xpos, int ypos) {
    int numofchar = strlen(ch);
    glLoadIdentity();
    glRasterPos2f(xpos, ypos);
    for (i = 0; i <= numofchar - 1; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch[i]);
    }
}

void drawLargeText(const char* text, int x, int y, void* font, float scale) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0);

    int len = strlen(text);
    glRasterPos2f(0, 0);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(font, text[i]);
    }

    glPopMatrix();
}

void drawTextNum(char ch[], int numtext, int xpos, int ypos) {
    int len;
    int k;
    k = 0;
    len = numtext - strlen(ch);
    glLoadIdentity();
    glRasterPos2f(xpos, ypos);
    for (i = 0; i <= numtext - 1; i++) {
        if (i < len)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '0');
        else {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch[k]);
            k++;
        }
    }

    glRasterPos2f(xpos - 60, ypos - 40);
    char speedBuffer[10];
    sprintf(speedBuffer, "        %d", speed);
    for (i = 0; i < strlen("Speed:"); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, "Speed:"[i]);
    }
    for (i = 0; i < strlen(speedBuffer); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, speedBuffer[i]);
    }
}

void saveHighScore(int highScore) {
    std::ofstream file(highScoreFileName);
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

int loadHighScore() {
    std::ifstream file(highScoreFileName);
    int loadedHighScore = 0;
    if (file.is_open()) {
        file >> loadedHighScore;
        file.close();
    }
    return loadedHighScore;
}

void updateHighScore() {

    highScore = loadHighScore();

    if (score > highScore) {
        highScore = score;
        saveHighScore(highScore); 
    }
}

void ovpos() {
    glClearColor(0, 0, 0, 0);
    for (i = 0; i < 4; i++) {
        if (rand() % 2 == 0) {
            ovehicleX[i] = 200;
        } else {
            ovehicleX[i] = 300;
        }
        ovehicleY[i] = 1000 - i * 160;
    }
}

int calculateAdjustedInterval(unsigned long long score) {
    // For every 200 rise in score, decrease the interval by 1 millisecond
    int adjustedInterval = originalTimerInterval - (score / 200);
    return (adjustedInterval < 0) ? 0 : adjustedInterval;
}

void updateSpeed(unsigned long long score) {
    // For every 200 rise in score, increase the speed by 10
    speed = 10 + (score / 200) * speedIncrease;
}

void resetGame() {
    score = 0;
    vehicleX = 200;
    vehicleY = 70;
    ovpos();
    movd = 0;
    gameRunning = true;
    updateGame = true;
    collide = false;
    pauseScreen = false;
    gameOver = false; 
    glutPostRedisplay();
}

void resetPos() {
    vehicleX = vehicleX;
    vehicleY = vehicleY;
    movd = movd;
    glutPostRedisplay();
}

void exitGame(int value) {
    if (collide && gameRunning) {
        resetGame();
        glutPostRedisplay();
    } else {
        exit(0);
    }
}

void drawRoad() {
    glLoadIdentity();
    glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5);
    glVertex2f(250 - 100, 500);
    glVertex2f(250 - 100, 0);
    glVertex2f(250 + 100, 0);
    glVertex2f(250 + 100, 500);
    glEnd();
}

void drawDivider() {
    glLoadIdentity();
    glTranslatef(0, movd, 0);
    for (i = 1; i <= 10; i++) {
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(divx - 5, divy * 15 * i + 18);
        glVertex2f(divx - 5, divy * 15 * i - 18);
        glVertex2f(divx + 5, divy * 15 * i - 18);
        glVertex2f(divx + 5, divy * 15 * i + 18);
        glEnd();
    }
    glLoadIdentity();
}

void drawVehicle() {
    glLoadIdentity();
    glPointSize(8.0);
    glBegin(GL_POINTS);
    glColor3f(0, 0, 0);
    glVertex2f(vehicleX - 25, vehicleY + 16);
    glVertex2f(vehicleX + 25, vehicleY + 16);
    glVertex2f(vehicleX - 25, vehicleY - 16);
    glVertex2f(vehicleX + 25, vehicleY - 16);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0, 150, 255);
    glVertex2f(vehicleX - 22, vehicleY + 20);
    glVertex2f(vehicleX - 22, vehicleY - 20);
    glVertex2f(vehicleX + 22, vehicleY - 20);
    glVertex2f(vehicleX + 22, vehicleY + 20);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0, 0, 0);
    glVertex2f(vehicleX - 23, vehicleY + 20);
    glVertex2f(vehicleX - 19, vehicleY + 45);
    glVertex2f(vehicleX + 19, vehicleY + 45);
    glVertex2f(vehicleX + 23, vehicleY + 20);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0, 0, 0);
    glVertex2f(vehicleX - 23, vehicleY - 20);
    glVertex2f(vehicleX - 19, vehicleY - 35);
    glVertex2f(vehicleX + 19, vehicleY - 35);
    glVertex2f(vehicleX + 23, vehicleY - 20);
    glEnd();

}

void drawOVehicle() {
    for (i = 0; i < 4; i++) {
        glLoadIdentity();
        glPointSize(8.0);
        glBegin(GL_POINTS);
        glColor3f(0, 0, 0);
        glVertex2f(ovehicleX[i] - 25, ovehicleY[i] + 16);
        glVertex2f(ovehicleX[i] + 25, ovehicleY[i] + 16);
        glVertex2f(ovehicleX[i] - 25, ovehicleY[i] - 16);
        glVertex2f(ovehicleX[i] + 25, ovehicleY[i] - 16);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0, 0, 1);
        glVertex2f(ovehicleX[i] - 22, ovehicleY[i] + 20);
        glVertex2f(ovehicleX[i] - 22, ovehicleY[i] - 20);
        glVertex2f(ovehicleX[i] + 22, ovehicleY[i] - 20);
        glVertex2f(ovehicleX[i] + 22, ovehicleY[i] + 20);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex2f(ovehicleX[i] - 23, ovehicleY[i] + 20);
        glVertex2f(ovehicleX[i] - 19, ovehicleY[i] + 45);
        glVertex2f(ovehicleX[i] + 19, ovehicleY[i] + 45);
        glVertex2f(ovehicleX[i] + 23, ovehicleY[i] + 20);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex2f(ovehicleX[i] - 23, ovehicleY[i] - 20);
        glVertex2f(ovehicleX[i] - 19, ovehicleY[i] - 35);
        glVertex2f(ovehicleX[i] + 19, ovehicleY[i] - 35);
        glVertex2f(ovehicleX[i] + 23, ovehicleY[i] - 20);
        glEnd();

        ovehicleY[i] = ovehicleY[i] - 2;

        if (ovehicleY[i] > vehicleY - 25 - 25 && ovehicleY[i] < vehicleY + 25 + 25 && ovehicleX[i] == vehicleX) {
            collide = true;
        }

        if (ovehicleY[i] < -25) {
            if (rand() % 2 == 0) {
                ovehicleX[i] = 200;
            } else {
                ovehicleX[i] = 300;
            }
            ovehicleY[i] = 600;
        }
    }
}

void Specialkey(int key, int x, int y) {
    keyStates[key] = true;

    if (key == GLUT_KEY_F1) {
        exitGame(0);
    }

    if (key == GLUT_KEY_F2) {
        screen = 0;
        collide = false;
        glutPostRedisplay();
    }

    if (key == GLUT_KEY_F3) {
        screen = 1;
        collide = false;
        glutPostRedisplay();
    }

    if (key == GLUT_KEY_F4) {
        screen = 2;
        gameRunning = true;
        collide = false;
        glutPostRedisplay();
    }

    if (key == GLUT_KEY_F5) {
        exit(0);
    }

    if (key == GLUT_KEY_UP) {
        vehicleY += 5;
            if (vehicleY > 450) {
                vehicleY = 450;
            }
    }

    if (key == GLUT_KEY_DOWN) {
        vehicleY -= 5;
            if (vehicleY < 0) {
                vehicleY = 0;
            }
    }

    if (key == GLUT_KEY_LEFT) {
        if (vehicleX > 200) {
            vehicleX -= 20;
        }
    }

    if (key == GLUT_KEY_RIGHT) {
        if (vehicleX < 300) {
            vehicleX += 20;
        }
    }

    glutPostRedisplay();
}

void keyUp(int key, int x, int y) {
    keyStates[key] = false;
    glutPostRedisplay();
}

void timer(int);

void Normalkey(unsigned char key, int x, int y) {
    keyStates[key] = true;

    if (!gameStarted && (key == 13)) {  // 13 is the ASCII code for 'Enter' key
        gameStarted = true;  // Set gameStarted to true when the user presses 'x' or 'Enter'
        screen = 2;
        gameRunning = true;
        gameOver = false;
    }

    if (key == 27 || (gameStarted && key == ' ')) { // 27 is the ASCII code for the 'ESC' key
        if (gameRunning && !pauseScreen && !gameOver) {
            pauseScreen = true;
            updateGame = false;
        } else if (gameRunning && !gameOver) {
            pauseScreen = false;
            updateGame = true;
            glutTimerFunc(20, timer, 0); // Resume the timer when unpaused
        }
    }

    if (screen == 0) {
        screen = 1;
    } else if (key == 13 && screen == 1) {
        screen = 2;
        gameRunning = true;
        gameOver = false;
    }

    if (key == 'r') {
        if (!gameRunning) {
            resetGame();
        } else if (pauseScreen) {
            resetGame(); // Restart the game on 'R' key press during the pause menu
        }
    }

    glutPostRedisplay();
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = false;

    if (key == 27) {  // 'ESC' key
        if (collide && gameRunning) {
            // If the game is over, pressing 'ESC' key quits the game
            resetGame();
            glutPostRedisplay();
        } else {
            exit(0);
        }
    }

    glutPostRedisplay();
}

void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 500, 0, 500);
    glMatrixMode(GL_MODELVIEW);

    highScore = loadHighScore();
}

void timer(int) {

    static int accumulatedPauseTime = 0;

    if(gameStarted && !pauseScreen){
        if (updateGame && !gameOver && !collide) {
        const int UpDown = 5;
        const int Side = 20;

        if (keyStates['w']) {
            vehicleY += UpDown;
            if (vehicleY > 450) {
                vehicleY = 450;
            }
        }
        if (keyStates['s']) {
            vehicleY -= UpDown;
            if (vehicleY < 0) {
                vehicleY = 0;
            }
        }
        if (keyStates['a'] && vehicleX > 200) {
            vehicleX -= Side;
        }
        if (keyStates['d'] && vehicleX < 300) {
            vehicleX += Side;
        }

        glutPostRedisplay();
    }
        int currentTimerInterval = calculateAdjustedInterval(score);
        updateSpeed(score);
        glutTimerFunc(currentTimerInterval, timer, 0);
    }
    else {
        accumulatedPauseTime += 16;

        if (accumulatedPauseTime > 500) {
            movd = 0;
            accumulatedPauseTime = 500;
        }

        glutTimerFunc(calculateAdjustedInterval(score), timer, 0);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (screen == 0) {
        glColor3f(1, 1, 1);
        drawLargeText("FAST LANE FURY", 120, 300, GLUT_BITMAP_TIMES_ROMAN_24, 5.0);
        drawText("Press any key to continue", 120, 150);
        glutSwapBuffers();
    } else if (screen == 1) {
        keyStates['2'] = false;
        glColor3f(1, 1, 1);
        drawLargeText("How to Play", 170, 390, GLUT_BITMAP_TIMES_ROMAN_24, 5.0);
        drawText("Use 'W' to accelerate", 170, 290);
        drawText("Use 'A' to move left", 170, 250);
        drawText("Use 'D' to move right", 170, 210);
        drawText("Use 'S' to move down", 170, 170);
        drawText("Press 'Enter' to play", 170, 120);
        glutSwapBuffers();
    } else {
        drawRoad();
        drawDivider();
        drawVehicle();
        drawOVehicle();
        movd = movd - 16;
        if (movd < -60) {
            movd = 0;
        }

        if (gameRunning) {
            score = score + 1;
        }

        glColor3f(1, 1, 1);
        drawText("Score:", 360, 405);
        sprintf(buffer, "%llu", score);
        drawTextNum(buffer, 6, 420, 405);

        glColor3f(1, 1, 1);
        drawText("High Score:", 360, 445);
        sprintf(buffer, "%llu", highScore);
        drawLargeText(buffer, 440, 445, GLUT_BITMAP_TIMES_ROMAN_24, 3.0);

        if (pauseScreen == true) {

            glColor4f(0.0, 0.0, 0.0, 0.5); // Lighter color with alpha set to 0.5 (adjust as needed)
            glBegin(GL_POLYGON);
            glVertex2f(100, 50);
            glVertex2f(400, 50);
            glVertex2f(400, 450);
            glVertex2f(100, 450);
            glEnd();

            glColor3f(1, 1, 1);
            drawLargeText("Pause Menu", 215, 400, GLUT_BITMAP_TIMES_ROMAN_24, 5.0);
            drawText("Press 'SPACEBAR' to Resume", 150, 270);
            drawText("Press 'R' to Restart", 180, 290);
            drawText("Press 'ESC' to Quit game", 180, 250);

            glColor3f(1, 1, 1);
            drawText("Your Score:", 200, 350);
            sprintf(buffer, "%llu", score);
            drawLargeText(buffer, 280, 350, GLUT_BITMAP_TIMES_ROMAN_24, 3.0);

            glutSwapBuffers();
            return;
        }

        else if (collide) {

            updateHighScore();

            if (highScore <= score) {
                glColor4f(0.0, 0.0, 0.0, 0.5);
                glBegin(GL_POLYGON);
                glVertex2f(100, 50);
                glVertex2f(400, 50);
                glVertex2f(400, 450);
                glVertex2f(100, 450);
                glEnd();

                glColor3f(1, 1, 1);
                drawLargeText("Congratulations", 200, 400, GLUT_BITMAP_TIMES_ROMAN_24, 5.0);
                drawText("High Score!!!", 210, 350);
                drawText("Press R to Restart", 180, 220);
                drawText("Press 'ESC' to quit the game", 170, 200);

                glColor3f(1, 1, 1);
                drawText("Your Score:", 200, 300);
                sprintf(buffer, "%llu", score);
                drawLargeText(buffer, 280, 300, GLUT_BITMAP_TIMES_ROMAN_24, 3.0);
            
                gameRunning = false;
            
            } else {
                glColor4f(0.0, 0.0, 0.0, 0.5); // Lighter color with alpha set to 0.5 (adjust as needed)
                glBegin(GL_POLYGON);
                glVertex2f(100, 50);
                glVertex2f(400, 50);
                glVertex2f(400, 450);
                glVertex2f(100, 450);
                glEnd();

                glColor3f(1, 1, 1);
                drawLargeText("Game Over", 215, 400, GLUT_BITMAP_TIMES_ROMAN_24, 5.0);
                drawText("Press R to Restart", 180, 220);
                drawText("Press 'ESC' to quit the game", 170, 200);

                glColor3f(1, 1, 1);
                drawText("Your Score:", 200, 300);
                sprintf(buffer, "%llu", score);
                drawLargeText(buffer, 280, 300, GLUT_BITMAP_TIMES_ROMAN_24, 3.0);

                gameRunning = false;
            }
            glutSwapBuffers();
        } else {
            glutSwapBuffers();
        }
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 500);
    glutCreateWindow("FASTLANEFURY");
    ovpos();
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(Specialkey);
    glutSpecialUpFunc(keyUp);  // Use glutSpecialUpFunc for special keys
    glutKeyboardFunc(Normalkey);
    glutKeyboardUpFunc(keyUp);  // Use glutKeyboardUpFunc for regular keys
    glutTimerFunc(20, timer, 0);
    glutMainLoop();

    return 0;
}