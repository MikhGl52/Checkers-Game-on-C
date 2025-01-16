#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>

// Функция рисования шашки
void drawChecker(int color, bool queen);

// Функция рисования экрана победы
void drawWinScreen();

// Функция рисования клеток доски
void drawSquare(int color);

// Функция рисования выделенного квадрата
void drawRSquare();

// Функция для вывода текста на экран
void drawText(float x, float y, char* text);

#endif // DRAW_H
