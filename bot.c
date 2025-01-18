#include "bot.h"
#include "main.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include "evaluate.h"

#define MAX_MOVES 100 // Максимальное количество возможных ходов
#define MAX_DEPTH 5

/*
whites ++
blacks --
*/

bool hasAvailableMoves(Tcell field[numRows][numCols], bool isMaximizing) {
    for (int fromY = 0; fromY < numRows; fromY++) {
        for (int fromX = 0; fromX < numCols; fromX++) {
            if (field[fromY][fromX].checker == (isMaximizing ? white : black)) {
                for (int toY = 0; toY < numRows; toY++) {
                    for (int toX = 0; toX < numCols; toX++) {
                        if (IsValidMove(fromX, fromY, toX, toY) || IsValidCapture(fromX, fromY, toX, toY)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

MoveState simulateMove(int fromX, int fromY, int toX, int toY) {
    MoveState moveState = {
        .fromX = fromX,
        .fromY = fromY,
        .toX = toX,
        .toY = toY,
        .captured = false,
        .capturedX = -1,
        .capturedY = -1,
        .prevChecker = field[fromY][fromX].checker,
        .prevQueen = field[fromY][fromX].queen,
        .capturedChecker = empty,
        .becameQueen = false
    };

    // Перемещение шашки
    int movingChecker = field[fromY][fromX].checker;
    bool movingQueen = field[fromY][fromX].queen;
    field[toY][toX].checker = movingChecker;
    field[toY][toX].queen = movingQueen;
    field[fromY][fromX].checker = empty;
    field[fromY][fromX].queen = false;

    // Проверка на захват
    int dx = toX - fromX;
    int dy = toY - fromY;

    if (!movingQueen && abs(dx) == 2 && abs(dy) == 2) {
        // Захват для обычной шашки
        int middleX = fromX + dx / 2;
        int middleY = fromY + dy / 2;
        moveState.captured = true;
        moveState.capturedX = middleX;
        moveState.capturedY = middleY;
        moveState.capturedChecker = field[middleY][middleX].checker;
        field[middleY][middleX].checker = empty;
    }
    else if (movingQueen && abs(dx) == abs(dy)) {
        // Захват для дамки
        int steps = abs(dx);
        for (int i = 1; i < steps; i++) {
            int currentX = fromX + i * (dx / steps);
            int currentY = fromY + i * (dy / steps);
            if (field[currentY][currentX].checker != empty) {
                moveState.captured = true;
                moveState.capturedX = currentX;
                moveState.capturedY = currentY;
                moveState.capturedChecker = field[currentY][currentX].checker;
                field[currentY][currentX].checker = empty;
                break;
            }
        }
    }

    // Преобразование в дамку
    if ((movingChecker == white && toY == numRows - 1) ||
        (movingChecker == black && toY == 0)) {
        if (!movingQueen) {
            moveState.becameQueen = true;
            field[toY][toX].queen = true;
        }
    }

    return moveState;
}

void undoMove(const MoveState& moveState) {
    // Восстановление шашки на начальную позицию
    field[moveState.fromY][moveState.fromX].checker = moveState.prevChecker;
    field[moveState.fromY][moveState.fromX].queen = moveState.prevQueen;

    // Очистка конечной позиции
    field[moveState.toY][moveState.toX].checker = empty;
    field[moveState.toY][moveState.toX].queen = false;

    // Восстановление захваченной шашки (если был захват)
    if (moveState.captured) {
        field[moveState.capturedY][moveState.capturedX].checker = moveState.capturedChecker;
    }

    // Удаление дамки, если шашка стала дамкой после хода
    if (moveState.becameQueen) {
        field[moveState.fromY][moveState.fromX].queen = false;
    }
}



int minimax(Tcell field[numRows][numCols], int depth, bool isMaximizing) {

    int score = evaluatePosition(field);

    if (depth == MAX_DEPTH || score == INT_MIN || score == INT_MAX)
        return score;

    bool isMovesAvailable = hasAvailableMoves(field, isMaximizing);
    if (!isMovesAvailable)
        return evaluatePosition(field); // Если ходов нет, возвращаем оценку позиции

    int best = isMaximizing ? INT_MIN : INT_MAX;

    for (int fromY = 0; fromY < numRows; fromY++) {
        for (int fromX = 0; fromX < numCols; fromX++) {
            if (field[fromY][fromX].checker == (isMaximizing ? white : black)) {
                for (int toY = 0; toY < numRows; toY++) {
                    for (int toX = 0; toX < numCols; toX++) {
                        if (IsValidMove(fromX, fromY, toX, toY) || IsValidCapture(fromX, fromY, toX, toY)) {
                            Tcell tempFrom = field[fromY][fromX];
                            Tcell tempTo = field[toY][toX];

                            simulateMove(fromX, fromY, toX, toY);

                            int currentScore = minimax(field, depth + 1, !isMaximizing);

                            if (isMaximizing) 
                                best = max(best, currentScore);
                            
                            else 
                                best = min(best, currentScore);
                            

                            // Откат изменений
                            field[fromY][fromX] = tempFrom;
                            field[toY][toX] = tempTo;
                        }
                    }
                }
            }
        }
    }

    return best;
}


void makeRandomMove(int best) {

    int fromX, fromY, toX, toY;
    bool moveMade = false;

    while (!moveMade) {
        fromX = rand() % numCols;
        fromY = rand() % numRows;
        toX = rand() % numCols;
        toY = rand() % numRows;

        if (field[fromY][fromX].checker == black) {
            if (IsValidMove(fromX, fromY, toX, toY)) {
                if (best == 1) {
                    if (!WouldBeUnderThreatAfterMove(fromX, fromY, toX, toY)) {
                        MoveChecker(fromX, fromY, toX, toY);
                        moveMade = true;
                        printf("Random move: (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);
                    }
                }
                else {
                    MoveChecker(fromX, fromY, toX, toY);
                    moveMade = true;
                    printf("Random move with lower prio: (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);
                }
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

    int score;
    int bestFromX = -1, bestFromY = -1, bestToX = -1, bestToY = -1;
    int bestScore = INT_MIN;
    bool mustCapture = HasValidCaptures();

    for (int fromY = 0; fromY < numRows; fromY++) {
        for (int fromX = 0; fromX < numCols; fromX++) {
            if (field[fromY][fromX].checker == black) {
                for (int toY = 0; toY < numRows; toY++) {
                    for (int toX = 0; toX < numCols; toX++) {
                        int score = EvaluateMove(fromX, fromY, toX, toY, mustCapture);
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
    printf("best score %d\n", bestScore);
    if (bestScore <= 1)
        makeRandomMove(bestScore); // Выполняем случайный ход, если все ходы имеют низкий приоритет

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

    for (int dy = -1; dy <= 1; dy += 2) {
        for (int dx = -1; dx <= 1; dx += 2) {

            //Cell from where opponent can capture us
            int fromX = x - dx;
            int fromY = y - dy;

            //Cell to where opponent can capture us
            int toX = x + dx;
            int toY = y + dy;

            if (InMap(fromX, fromY) && InMap(toX, toY) &&
                field[fromY][fromX].checker == enemy &&
                field[toY][toX].checker == empty) {
                return true;
            }
        }
    }
    return false;
}


BOOL OpponentUnderThreatAfterMove(int fromX, int fromY, int toX, int toY) {

    int tempChecker = field[toY][toX].checker;
    bool tempQueen = field[toY][toX].queen;
    field[toY][toX].checker = field[fromY][fromX].checker;
    field[toY][toX].queen = field[fromY][fromX].queen;
    field[fromY][fromX].checker = empty;
    field[fromY][fromX].queen = false;

    BOOL underThreat = OpponentUnderThreat(toX, toY);

    field[fromY][fromX].checker = field[toY][toX].checker;
    field[fromY][fromX].queen = field[toY][toX].queen;
    field[toY][toX].checker = tempChecker;
    field[toY][toX].queen = tempQueen;
    return underThreat;
}

BOOL OpponentUnderThreat(int x, int y) {
    int enemy = (player == white) ? black : white;

    for (int dy = -2; dy <= 2; dy += 4) {
        for (int dx = -2; dx <= 2; dx += 4) {
            int middleX = x + dx / 2;
            int middleY = y + dy / 2;
            int endX = x + dx;
            int endY = y + dy;

            // Проверка: захват возможен, если средняя клетка содержит врага,
            // а конечная клетка пуста
            if (InMap(middleX, middleY) && InMap(endX, endY) &&
                field[middleY][middleX].checker == enemy && // Средняя клетка — враг
                field[endY][endX].checker == empty) {      // Конечная клетка — пуста
                return true;
            }
        }
    }
    return false;
}