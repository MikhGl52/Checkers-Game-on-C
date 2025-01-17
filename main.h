#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <GLFW/glfw3.h>
#include <windows.h>
#include <stdbool.h>
#include <math.h>
#include <GL\freeglut.h>
#include <stdlib.h>
#include <stdio.h> 

#define numRows 10 //W
#define numCols 10 //H
#define M_PI 3.14159265358979323846  /* pi */
typedef enum {
    empty,
    white,
    black
} Color;

typedef struct {
    int checker; // 0 - empty, 1 - white, 2 - black
    int color;   // 1 - white, 2 - black
    bool queen;
    bool border;
    int selected;
} Tcell;

extern Tcell field[numRows][numCols];
extern bool PvP;
extern bool inMenu;
extern int player;
extern int whiteAmount;
extern int blackAmount;
extern bool isCheckerSelected;
extern POINT selectedChecker;

void ScreenToOpenGL(HWND hwnd, int x, int y, float* ox, float* oy);
void ShowGame();
void NewGame();
int InMap(int x, int y);
bool IsValidMove(int fromX, int fromY, int toX, int toY);
bool IsValidCapture(int fromX, int fromY, int toX, int toY);
bool HasValidCaptures();
bool MoveChecker(int fromX, int fromY, int toX, int toY);
bool HasValidCapturesFrom(int x, int y);
bool GameOver();

//bool OpponentUnderThreat(int x, int y);


#endif // MAIN_H

