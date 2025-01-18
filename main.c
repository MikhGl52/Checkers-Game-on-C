/*These checkers are made by @MG52 based on the rules of Nigerian checkers*/

#include <GLFW/glfw3.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <GL\freeglut.h>
#include "bot.h"
#include "draw.h"
#include "main.h"
#include "button.h"


#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

bool PvP = false;
bool inMenu = false;
bool winner;

bool ended = false;

// Глобальные переменные для хранения размера окна
int windowWidth = 1000;
int windowHeight = 1000;
float cellSize = 1.0f / numRows;


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


void ShowMenu() {
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    /*Button_Show(startBtn);
    Button_Show(pvpBtn);*/

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
    glLoadIdentity();
}


// Проверка на то, что координаты находятся в пределах поля
int InMap(int x, int y) {
    return (x >= 0) && (y >= 0) && (x < numRows) && (y < numCols);
}

bool IsValidMove(int fromX, int fromY, int toX, int toY) {
    if (!InMap(toX, toY) || field[toY][toX].checker != empty)
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


bool HasValidCaptures() {
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


bool HasValidCapturesFrom(int x, int y) {
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

bool GameOver(bool* winner) {
    if (whiteAmount == 0) {
        printf("Black wins!\n");
        *winner = false;
        ended = true;
        return true;
    }
    else if (blackAmount == 0) {
        printf("White wins!\n");
        *winner = true;
        ended = true;
        return true;
    }
    return false;
}

int checkWin() {

    if (whiteAmount == 0) 
        return black;
    
    else if (blackAmount == 0)
        return white;

    return empty;
}

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(time(NULL)); // Инициализация генератора случайных чисел
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

            if (ended == false)
                ShowGame();
            else drawWinScreen(winner);

            glPopMatrix();
            SwapBuffers(hDC);

            if (!PvP && !inMenu) {
                if (player == black) {
                    MakeBotMove();
                    player = white;
                    if (GameOver(winner)) {
                        drawWinScreen();
                        Sleep(2);
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
            /*HandleButtonClick(x, y);
            inMenu = false;*/
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
                    if (GameOver(winner)) {
                        Sleep(2);
                        drawWinScreen();
                        Sleep(2);
                    }


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