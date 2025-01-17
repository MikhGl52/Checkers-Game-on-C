#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>

// ������� ��������� �����
void drawChecker(int color, bool queen);

// ������� ��������� ������ ������
void drawWinScreen();

// ������� ��������� ������ �����
void drawSquare(int color);

// ������� ��������� ����������� ��������
void drawRSquare();

void drawText(float x, float y, const char* text);

#endif // DRAW_H
