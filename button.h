#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <windows.h>
#include "main.h"
#include "draw.h"

// Структура кнопки
typedef struct {
    char name[20];      // Название кнопки
    float vert[8];      // Координаты вершин (в формате x1, y1, x2, y2, ...)
    char text[20];      // Текст, отображаемый на кнопке
} Button;

// Внешние переменные для кнопок
extern Button startBtn;  // Кнопка "Start Game"
extern Button pvpBtn;    // Кнопка "PvP Mode"

// Функция для проверки, находится ли точка внутри кнопки
BOOL PointInButton(int x, int y, Button btn);

// Функция для отрисовки кнопки
void Button_Show(Button btn);

// Функция для обработки нажатий на кнопки
void HandleButtonClick(int x, int y);

#endif // BUTTON_H
