#include "bot.h"
#include "main.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>


void makeRandomMove() {
    srand(time(NULL)); // инициализация генератора случайных чисел

    // Пробуем сделать случайный ход, пока не найдем допустимый
    int fromX, fromY, toX, toY;
    bool moveMade = false;

    while (!moveMade) {
        fromX = rand() % numCols;
        fromY = rand() % numRows;
        toX = rand() % numCols;
        toY = rand() % numRows;

        if (field[fromY][fromX].checker == black) {
            if (IsValidMove(fromX, fromY, toX, toY)) {
                MoveChecker(fromX, fromY, toX, toY);
                moveMade = true;
                printf("Random move: (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);
            }
            else if (IsValidCapture(fromX, fromY, toX, toY)) {
                MoveChecker(fromX, fromY, toX, toY);
                moveMade = true;
                printf("Random capture: (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);
            }
        }
    }
}


void MakeBotMove() {
    srand(time(NULL));

    int bestFromX = -1, bestFromY = -1, bestToX = -1, bestToY = -1;
    int bestScore = INT_MIN;
    bool mustCapture = HasValidCaptures();

    for (int fromY = 0; fromY < numRows; fromY++) {
        for (int fromX = 0; fromX < numCols; fromX++) {
            if (field[fromY][fromX].checker == black) {
                for (int toY = 0; toY < numRows; toY++) {
                    for (int toX = 0; toX < numCols; toX++) {
                        int score = 0; // Инициализация переменной score
                        if (mustCapture && IsValidCapture(fromX, fromY, toX, toY)) {
                            score = 100; // Захват шашки даёт +100 очков
                            printf("Capture move: (%d, %d) to (%d, %d) with score %d\n", fromX, fromY, toX, toY, score);
                            if (score > bestScore) {
                                bestScore = score;
                                bestFromX = fromX;
                                bestFromY = fromY;
                                bestToX = toX;
                                bestToY = toY;
                            }
                        }
                        else if (!mustCapture && IsValidMove(fromX, fromY, toX, toY)) {
                            score += 1; // Обычный ход даёт +1 очко
                            if ((player == black && toY == 0) || (player == white && toY == numRows - 1)) {
                                score = 5; // Движение в сторону дамки даёт +5 очков
                            }
                            if (WouldBeUnderThreatAfterMove(fromX, fromY, toX, toY)) {
                                score = -10; // Если шашка окажется под угрозой, -10 очков
                            }
                            else if (OpponentUnderThreat(toX, toY) && !WouldBeUnderThreatAfterMove(fromX, fromY, toX, toY)) {
                                score = 10; // Если шашка соперника окажется под угрозой, +10 очков
                            }
                            //printf("Move: (%d, %d) to (%d, %d) with score %d\n", fromX, fromY, toX, toY, score);
                            if (score > bestScore) {
                                bestScore = score;
                                bestFromX = fromX;
                                bestFromY = fromY;
                                bestToX = toX;
                                bestToY = toY;
                            }
                        }
                    }
                }
            }
        }
    }

    if (bestScore <= 1) {
        makeRandomMove(); // Выполняем случайный ход, если все ходы имеют низкий приоритет
    }
    else {
        if (bestFromX != -1 && bestFromY != -1 && bestToX != -1 && bestToY != -1) {
            bool captured = MoveChecker(bestFromX, bestFromY, bestToX, bestToY);
            printf("Bot move priority: %d\n", bestScore); // Вывод приоритета хода


            while (captured && HasValidCapturesFrom(bestToX, bestToY)) {
                int nextFromX = bestToX;
                int nextFromY = bestToY;
                bestScore = INT_MIN;
                for (int toY = 0; toY < numRows; toY++) {
                    for (int toX = 0; toX < numCols; toX++) {
                        if (IsValidCapture(nextFromX, nextFromY, toX, toY)) {
                            int score = 100;
                            if (score > bestScore) {
                                bestScore = score;
                                bestFromX = nextFromX;
                                bestFromY = nextFromY;
                                bestToX = toX;
                                bestToY = toY;
                            }
                        }
                    }
                }
                captured = MoveChecker(bestFromX, bestFromY, bestToX, bestToY);
                printf("Bot move priority: %d\n", bestScore); // Вывод приоритета хода
            }

            if (!captured || !HasValidCapturesFrom(bestToX, bestToY)) {
                player = white; // Переключаем ход к белым
            }
        }
    }
}





BOOL WouldBeUnderThreatAfterMove(int fromX, int fromY, int toX, int toY) {
    int tempChecker = field[toY][toX].checker;
    bool tempQueen = field[toY][toX].queen;
    field[toY][toX].checker = field[fromY][fromX].checker;
    field[toY][toX].queen = field[fromY][fromX].queen;
    field[fromY][fromX].checker = empty;
    field[fromY][fromX].queen = false;

    BOOL underThreat = WouldBeUnderThreat(toX, toY);

    field[fromY][fromX].checker = field[toY][toX].checker;
    field[fromY][fromX].queen = field[toY][toX].queen;
    field[toY][toX].checker = tempChecker;
    field[toY][toX].queen = tempQueen;

    return underThreat;
}

BOOL WouldBeUnderThreat(int x, int y) {
    int enemy = (player == white) ? black : white;

    for (int dy = -2; dy <= 2; dy += 4) {
        for (int dx = -2; dx <= 2; dx += 4) {
            int middleX = x + dx / 2;
            int middleY = y + dy / 2;
            int endX = x + dx;
            int endY = y + dy;

            if (InMap(middleX, middleY) && InMap(endX, endY) &&
                field[middleY][middleX].checker == enemy &&
                field[endY][endX].checker == empty) {
                return true;
            }
        }
    }

    return false;
}



BOOL OpponentUnderThreat(int x, int y) {
    int enemy = (player == white) ? black : white;

    for (int dy = -2; dy <= 2; dy += 4) {
        for (int dx = -2; dx <= 2; dx += 4) {
            int middleX = x + dx / 2;
            int middleY = y + dy / 2;
            int endX = x + dx;
            int endY = y + dy;

            if (InMap(middleX, middleY) && InMap(endX, endY) &&
                field[middleY][middleX].checker == empty &&
                field[endY][endX].checker == enemy &&
                field[y][x].checker != empty) {
                return true;
            }
        }
    }

    return false;
}
