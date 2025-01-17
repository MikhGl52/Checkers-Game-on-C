#include <GLFW/glfw3.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <GL\freeglut.h>
#include "main.h"

typedef struct {
    char name[20];
    float vert[8];
    char text[20];
} Button;

BOOL PointInButton(int x, int y, Button btn) {
    return (x > btn.vert[0]) && (x < btn.vert[4]) && (y > btn.vert[1]) && (y < btn.vert[5]);
}

Button startBtn = { "start",{0,0,100,0,100,30,0,30}, "Start Game" };
Button pvpBtn = { "pvp",{110,0,210,0,210,30,110,30}, "PvP Mode" };



void Button_Show(Button btn) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(1, 1, 0);
    glVertexPointer(2, GL_FLOAT, 0, btn.vert);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    // Отрисовка текста на кнопке
    drawText(btn.vert[0] + 10, btn.vert[1] + 20, btn.text);
}


void HandleButtonClick(int x, int y) {
    if (PointInButton(x, y, startBtn)) {
        inMenu = false;
        printf("Start button clicked\n");
        // Начать новую игру
        NewGame();
    }
    else if (PointInButton(x, y, pvpBtn)) {
        PvP = !PvP;
        printf("PvP button clicked. PvP is now %s\n", PvP ? "on" : "off");
    }
}