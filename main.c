/*These checkers are made by @MG52 based on the rules of Nigerian checkers*/

#include <GLFW/glfw3.h>
#include <windows.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <GL\freeglut.h>
#include "bot.h"
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

bool PvP = false;
bool inMenu = false;


#define M_PI 3.14159265358979323846  /* pi */
#define numRows 10 //W
#define numCols 10 //H

// Глобальные переменные для хранения размера окна
int windowWidth = 1000;
int windowHeight = 1000;
float cellSize = 1.0f / numRows;

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

typedef struct {
    char name[20];
    float vert[8];
    char text[20];
} Button;



Tcell field[numRows][numCols];
int player = white;
POINT selectedChecker = { -1, -1 };
bool isCheckerSelected = false;
bool possibleToChange = true;
int whiteAmount;
int blackAmount;

// Инициализация новой игры
void NewGame() {
    whiteAmount = 0;
    blackAmount = 0;
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if ((i + j) % 2)
                field[i][j].color = black;
            else
                field[i][j].color = white;

            field[i][j].checker = empty;
            field[i][j].queen = false;
            field[i][j].border = false;
            field[i][j].selected = 0;
            if (field[i][j].color == black) {
                if (i < 4) {
                    field[i][j].checker = white;
                    whiteAmount++;
                }
                else if (i >= numRows - 4) {
                    field[i][j].checker = black;
                    blackAmount++;
                }
            }
        }
    }
}

void ScreenToOpenGL(HWND hwnd, int x, int y, float* ox, float* oy) {
    RECT rct;
    GetClientRect(hwnd, &rct); // Клиентская область
    *ox = (x / (float)rct.right) * numCols;
    *oy = numRows - (y / (float)rct.bottom) * numRows;
}

void drawChecker(int color, bool queen) {
    float x, y;
    float cnt = 100;
    float l = 0.45;
    float a = M_PI * 2 / cnt;

    glBegin(GL_TRIANGLE_FAN);
    if (color == black) // Черный цвет
        glColor3f(0.4588, 0.02, 0.02);
    else if (color == white) // Белый цвет
        glColor3f(0.99609375, 0.96875, 0.859375);

    glVertex2f(0.5, 0.5); // Центр круга

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


void drawSquare(int color) {
    if (color == black) // Черный цвет
        glColor3f(0.0f, 0.0f, 0.0f);
    else if (color == white) // Белый цвет
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


BOOL PointInButton(int x, int y, Button btn) {
    return (x > btn.vert[0]) && (x < btn.vert[4]) && (y > btn.vert[1]) && (y < btn.vert[5]);
}

Button startBtn = { "start",{0,0,100,0,100,30,0,30}, "Start Game" };
Button pvpBtn = { "pvp",{110,0,210,0,210,30,110,30}, "PvP Mode" };

void drawText(float x, float y, char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

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

void ShowMenu() {
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    Button_Show(startBtn);
    Button_Show(pvpBtn);

    glPopMatrix();
}

// Отображение игрового поля
void ShowGame() {
    glLoadIdentity();
    glScalef(2.0 / numRows, 2.0 / numCols, 1);
    glTranslatef(-numRows * 0.5, -numCols * 0.5, 0);

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            glPushMatrix();
            glTranslatef(j, i, 0);

            drawSquare(field[i][j].color);

            if (field[i][j].checker != empty) {
                drawChecker(field[i][j].checker, field[i][j].queen);
            }
            if (field[i][j].selected) {
                drawRSquare();
            }

            glPopMatrix();
        }
    }

}



// Проверка на то, что координаты находятся в пределах поля
int InMap(int x, int y) {
    return (x >= 0) && (y >= 0) && (x < numRows) && (y < numCols);
}

bool IsValidMove(int fromX, int fromY, int toX, int toY) {
    if (field[toY][toX].checker != empty)
        return false;

    int dx = toX - fromX;
    int dy = toY - fromY;

    if (!HasValidCaptures()) {
    
        if (field[fromY][fromX].queen == false) {
            if (abs(dx) == 1 && abs(dy) == 1) {
                if ((player == white && dy == 1) || (player == black && dy == -1)) {
                    return true;
                }
            }
        }

        else {
            if (abs(dx) == abs(dy))
                return true;
        }
    }
    return false;
}

// Проверка допустимости рубки
bool IsValidCapture(int fromX, int fromY, int toX, int toY) {
    if (!InMap(toX, toY) || field[toY][toX].checker != empty)
        return false;

    int dx = toX - fromX;
    int dy = toY - fromY;


    if (field[fromY][fromX].queen == false) { //Обычные
        if (abs(dx) == 2 && abs(dy) == 2) {
            int middleX = fromX + dx / 2;
            int middleY = fromY + dy / 2;
            if (field[middleY][middleX].checker != empty && field[middleY][middleX].checker != player)
                return true;
        }
    }

    else { //Дамки
        if (abs(dx) == abs(dy)) {
            int steps = abs(dx);
            int middleX = -1, middleY = -1;
            bool foundCapture = false;
            for (int i = 1; i < steps; i++) {
                int currentX = fromX + i * (dx / steps);
                int currentY = fromY + i * (dy / steps);
                if (field[currentY][currentX].checker != empty) {
                    if (field[currentY][currentX].checker == player || foundCapture) {
                        return false; // Нельзя прыгать через свои шашки или через несколько шашек
                    }
                    foundCapture = true;
                    middleX = currentX;
                    middleY = currentY;
                }
            }
            if (foundCapture) {
                printf("It is possible to make a capture from (%d, %d) to (%d, %d) by %d\n", fromX, fromY, toX, toY, player);
                return true;
            }
        }
    }

    return false;
}




BOOL HasValidCaptures() {
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (field[i][j].checker == player) {
                if (!field[i][j].queen) { // Проверка для обычных шашек
                    if (IsValidCapture(j, i, j + 2, i + 2) ||
                        IsValidCapture(j, i, j + 2, i - 2) ||
                        IsValidCapture(j, i, j - 2, i + 2) ||
                        IsValidCapture(j, i, j - 2, i - 2)) {
                        return true;
                    }
                }
                else { // Проверка для дамок
                    for (int dx = -numRows; dx <= numRows; dx++) {
                        if (dx == 0) continue;
                        if (IsValidCapture(j, i, j + dx, i + dx) ||
                            IsValidCapture(j, i, j + dx, i - dx)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


bool MoveChecker(int fromX, int fromY, int toX, int toY) {
    bool hasCaptured = false;
    field[toY][toX].checker = field[fromY][fromX].checker;
    field[toY][toX].queen = field[fromY][fromX].queen;
    field[fromY][fromX].checker = empty;
    field[fromY][fromX].queen = false;
    field[fromY][fromX].selected = 0;

    // Проверка на рубку
    int dx = toX - fromX;
    int dy = toY - fromY;

    if (field[toY][toX].queen == false) {
        if (abs(dx) == 2 && abs(dy) == 2) {
            int middleX = fromX + dx / 2;
            int middleY = fromY + dy / 2;
            if (field[middleY][middleX].checker == white) whiteAmount--;
            if (field[middleY][middleX].checker == black) blackAmount--;
            field[middleY][middleX].checker = empty;
            hasCaptured = true;
        }
    }
    else {
        if (abs(dx) == abs(dy)) { // Дамка движется по диагонали
            int steps = abs(dx);
            int middleX = -1, middleY = -1;
            bool foundCapture = false;
            for (int i = 1; i < steps; i++) {
                int currentX = fromX + i * (dx / steps);
                int currentY = fromY + i * (dy / steps);
                if (field[currentY][currentX].checker != empty) {
                    if (field[currentY][currentX].checker == player || foundCapture) {
                        return false; // Нельзя прыгать через свои шашки или через несколько шашек
                    }
                    foundCapture = true;
                    middleX = currentX;
                    middleY = currentY;
                }
            }
            if (foundCapture) {
                if (field[middleY][middleX].checker == white) whiteAmount--;
                if (field[middleY][middleX].checker == black) blackAmount--;
                field[middleY][middleX].checker = empty;
                hasCaptured = true;
            }
        }
    }

    // Преобразование в дамку
    if (((player == white && toY == numRows - 1) && (!HasValidCapturesFrom(toX, toY))) || ((player == black && toY == 0)&&(!HasValidCapturesFrom(toX, toY)))) {
        field[toY][toX].queen = true;
        printf("Checker at (%d, %d) has become a queen\n", toX, toY);
    }

    isCheckerSelected = false;
    return hasCaptured;
}



BOOL HasValidCapturesFrom(int x, int y) {
    if (!field[y][x].queen) { // Проверка для обычных шашек
        return IsValidCapture(x, y, x + 2, y + 2) ||
            IsValidCapture(x, y, x + 2, y - 2) ||
            IsValidCapture(x, y, x - 2, y + 2) ||
            IsValidCapture(x, y, x - 2, y - 2);
    }
    else { // Проверка для дамок
        for (int dx = -numRows; dx <= numRows; dx++) {
            if (dx == 0) continue;
            if (IsValidCapture(x, y, x + dx, y + dx) ||
                IsValidCapture(x, y, x + dx, y - dx)) {
                return true;
            }
        }
    }
    return false;
}

bool GameOver() {
    if (whiteAmount == 0) {
        printf("Black wins!\n");
        return true;
    }
    else if (blackAmount == 0) {
        printf("White wins!\n");
        return true;
    }
    return false;
}

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Инициализация консоли
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

    // Initialize GLUT

    
    


    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(
        0,                      // dwExStyle
        "GLSample",             // lpClassName
        "",                     // lpWindowName (строка заголовка окна)
        WS_OVERLAPPEDWINDOW,    // dwStyle
        CW_USEDEFAULT,          // x
        CW_USEDEFAULT,          // y
        windowWidth,            // nWidth
        windowHeight,           // nHeight
        NULL,                   // hWndParent
        NULL,                   // hMenu
        hInstance,              // hInstance
        NULL                    // lpParam
    );

    if (!hwnd)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    NewGame();
    printf("%dx%d\n", whiteAmount, blackAmount);

    /* program main loop */
    while (!bQuit) {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT) {
                bQuit = TRUE;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            /* OpenGL animation code goes here */

            glClearColor(0.375f, 0.375f, 0.375f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();
            glClear(GL_COLOR_BUFFER_BIT);

            /*if (inMenu) {
                ShowMenu();
            }
            else {
                ShowGame();
            }*/


            ShowGame();

            glPopMatrix();
            SwapBuffers(hDC);

            if (!PvP && !inMenu) {
                if (player == black) {
                    MakeBotMove();
                    player = white;
                    if (GameOver()) {
                        PostQuitMessage(0);
                    }
                }
            }
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    FreeConsole();

    return msg.wParam;
}



// Обработчик событий WindowProc
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_SIZE:
        windowWidth = LOWORD(lParam);
        windowHeight = HIWORD(lParam);
        glViewport(0, 0, windowWidth, windowHeight);
        glLoadIdentity();
        float k = windowWidth / (float)windowHeight;
        glOrtho(-k, k, -1, 1, -1, 1);
    break;



    case WM_LBUTTONDOWN: {
        POINTFLOAT pf;
        ScreenToOpenGL(hwnd, LOWORD(lParam), HIWORD(lParam), &pf.x, &pf.y);
        int x = (int)pf.x;
        int y = (int)pf.y;

        if (inMenu) {
            HandleButtonClick(x, y);
            inMenu = false;
        }
        else {
            if (InMap(x, y)) {
                bool mustCapture = HasValidCaptures();

                if (isCheckerSelected) {
                    if (IsValidCapture(selectedChecker.x, selectedChecker.y, x, y) || (!mustCapture && IsValidMove(selectedChecker.x, selectedChecker.y, x, y))) {
                        bool captured = MoveChecker(selectedChecker.x, selectedChecker.y, x, y);
                        if (captured) {
                            const char* playerName = (player == white) ? "white" : "black";
                            printf("Capture made at (%d, %d) by player %s\n", x, y, playerName);

                            if (HasValidCapturesFrom(x, y)) {
                                selectedChecker.x = x;
                                selectedChecker.y = y;
                                field[y][x].selected = 1;
                                isCheckerSelected = true;
                            }
                            else {
                                player = (player == white) ? black : white;
                                isCheckerSelected = false;
                            }
                        }
                        else {
                            const char* playerName = (player == white) ? "white" : "black";
                            player = (player == white) ? black : white;
                            isCheckerSelected = false;
                            printf("Move made from %d %d to %d %d by player %s\n", selectedChecker.x, selectedChecker.y, x, y, playerName);
                        }
                        field[selectedChecker.y][selectedChecker.x].selected = 0;
                    }
                    else {
                        printf("Invalid move\n");
                        field[selectedChecker.y][selectedChecker.x].selected = 0;
                        selectedChecker.x = x;
                        selectedChecker.y = y;
                        field[y][x].selected = 1;
                        isCheckerSelected = true;
                    }

                    // Проверка окончания игры
                    if (GameOver())
                        PostQuitMessage(0);

                }
                else {
                    if (field[y][x].checker == player) {
                        if (!mustCapture || HasValidCapturesFrom(x, y)) {
                            selectedChecker.x = x;
                            selectedChecker.y = y;
                            field[y][x].selected = 1;
                            isCheckerSelected = true;
                        }
                    }
                }
            }
        }
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}



void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC) {
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}