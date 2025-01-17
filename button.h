#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <windows.h>
#include "main.h"
#include "draw.h"

// ��������� ������
typedef struct {
    char name[20];      // �������� ������
    float vert[8];      // ���������� ������ (� ������� x1, y1, x2, y2, ...)
    char text[20];      // �����, ������������ �� ������
} Button;

// ������� ���������� ��� ������
extern Button startBtn;  // ������ "Start Game"
extern Button pvpBtn;    // ������ "PvP Mode"

// ������� ��� ��������, ��������� �� ����� ������ ������
BOOL PointInButton(int x, int y, Button btn);

// ������� ��� ��������� ������
void Button_Show(Button btn);

// ������� ��� ��������� ������� �� ������
void HandleButtonClick(int x, int y);

#endif // BUTTON_H
