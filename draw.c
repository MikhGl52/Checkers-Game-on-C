#include <GLFW/glfw3.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>�
#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <GL\freeglut.h>
#include "bot.h"
#include "main.h"
#include <GL/glut.h>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

void drawChecker(int color, bool queen) {
    float x, y;
    float cnt = 100;
    float l = 0.45;
    float a = M_PI * 2 / cnt;

    glBegin(GL_TRIANGLE_FAN);
    if (color == black) // ������ ����
        glColor3f(0.4588, 0.02, 0.02);
    else if (color == white) // ����� ����
        glColor3f(0.99609375, 0.96875, 0.859375);

    glVertex2f(0.5, 0.5); // ����� �����

    for (int i = -1; i < cnt; i++) {
        x = sin(a * i) * l + 0.5;
        y = cos(a * i) * l + 0.5;
        glVertex2f(x, y);
    }

    glEnd();

    if (queen) {
        glBegin(GL_TRIANGLES);

        glColor3f(0.51, 0.43, 0.02);

        glVertex2f(0.5, 0.25);
        glVertex2f(0.25, 0.25);
        glVertex2f(0.25, 0.75);

        glVertex2f(0.5, 0.25);
        glVertex2f(0.75, 0.25);
        glVertex2f(0.75, 0.75);

        glVertex2f(0.375, 0.25);
        glVertex2f(0.625, 0.25);
        glVertex2f(0.5, 0.75);

        glEnd();
    }

}

//void drawText(float x, float y, const char* text) {
//    glRasterPos2f(x, y); // ������������� ������� ������
//    for (const char* c = text; *c != '\0'; c++) {
//        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
//    }
//}

void drawWinScreen() {
    glMatrixMode(GL_PROJECTION);  // ������� � ������� ��������
    glLoadIdentity();            // ����� �������
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);  // ������������� ���������� �� 0 �� 1

    glMatrixMode(GL_MODELVIEW);  // ������� � ������� ����
    glLoadIdentity();            // ����� �������

    glClear(GL_COLOR_BUFFER_BIT); // ������� ������

    // ������ ������� ������� �� ���� �����
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); // ������� ����
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0);
    glEnd();

    //// ������ �����
    //glColor3f(1.0, 1.0, 1.0); // ����� ���� ������
    //drawText(0.4f, 0.5f, "Hello, OpenGL!"); // ������� ������ (x, y)

    glFlush(); // ��������� ���������
}



void drawSquare(int color) {
    if (color == black) // ������ ����
        glColor3f(0.0f, 0.0f, 0.0f);
    else if (color == white) // ����� ����
        glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0);
    glEnd();
}

void drawRSquare() {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0);
    glEnd();
}

