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
        fromX = rand() % numCols;  // генерация случайных координат клеток, откуда будет сделан ход и куда
        fromY = rand() % numRows;
        toX = rand() % numCols;
        toY = rand() % numRows;

        if (field[fromY][fromX].checker == black) {
            if (IsValidMove(fromX, fromY, toX, toY)) { //проверка на допустимый ход
                if (!WouldBeUnderThreatAfterMove(fromX, fromY, toX, toY)) {  //проверка: не окажется ли шашка под угрозой после выполнения хода
                    MoveChecker(fromX, fromY, toX, toY);
                    moveMade = true;
                    printf("Random move: (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);
                }
            }
            else if (IsValidCapture(fromX, fromY, toX, toY)) { // проверка на захват
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

    for (int fromY = 0; fromY < numRows; fromY++) {     //проход по всем клеткам поля
        for (int fromX = 0; fromX < numCols; fromX++) {
            if (field[fromY][fromX].checker == black) {
                for (int toY = 0; toY < numRows; toY++) {   //проход по всем возможным клеткам назначения для текущей шашки
                    for (int toX = 0; toX < numCols; toX++) {
                        int score = 0; // Инициализация переменной score
                        if (mustCapture && IsValidCapture(fromX, fromY, toX, toY)) {
                            score = 100; // Захват шашки даёт +100 очков
                            printf("Capture move: (%d, %d) to (%d, %d) with score %d\n", fromX, fromY, toX, toY, score);
                            if (score > bestScore) {  // Если текущая оценка выше предыдущей лучшей оценки, 
                                bestScore = score;    // обновляются координаты лучшего хода
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
                            printf("Move: (%d, %d) to (%d, %d) with score %d\n", fromX, fromY, toX, toY, score);
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
    else { // Если найден хороший ход, то выполняем его
        if (bestFromX != -1 && bestFromY != -1 && bestToX != -1 && bestToY != -1) {
            bool captured = MoveChecker(bestFromX, bestFromY, bestToX, bestToY);
            printf("Bot move priority: %d\n", bestScore); // Вывод приоритета хода

            while (captured && HasValidCapturesFrom(bestToX, bestToY)) {      // Если после первого захвата есть возможность продолжить захватывать, 
                int nextFromX = bestToX;                                      // начинается цикл для поиска следующего лучшего захвата
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




// Функция проверяет, окажется ли шашка под угрозой после выполнения хода
BOOL WouldBeUnderThreatAfterMove(int fromX, int fromY, int toX, int toY) {
    int tempChecker = field[toY][toX].checker;
    bool tempQueen = field[toY][toX].queen;
    // Перемещаем шашку
    field[toY][toX].checker = field[fromY][fromX].checker;
    field[toY][toX].queen = field[fromY][fromX].queen;
    field[fromY][fromX].checker = empty;
    field[fromY][fromX].queen = false;

    // Проверяем, будет ли шашка под угрозой
    BOOL underThreat = WouldBeUnderThreat(toX, toY);

    // Возвращаем поле в исходное состояние
    field[fromY][fromX].checker = field[toY][toX].checker;
    field[fromY][fromX].queen = field[toY][toX].queen;
    field[toY][toX].checker = tempChecker;
    field[toY][toX].queen = tempQueen;

    return underThreat;
}

// проверяет, находится ли шашка под угрозой от противника
BOOL WouldBeUnderThreat(int x, int y) {
    int enemy = (player == white) ? black : white;

    for (int dy = -2; dy <= 2; dy += 4) {                       // Если средняя клетка содержит шашку противника, 
        for (int dx = -2; dx <= 2; dx += 4) {                   // а конечная клетка пуста, это означает, что шашка под угрозой захвата
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


// проверяет, находится ли шашка противника под угрозой
BOOL OpponentUnderThreat(int x, int y) {
    int enemy = (player == white) ? black : white;

    for (int dy = -2; dy <= 2; dy += 4) {                       // Проверяет, есть ли пустая клетка между текущей клеткой и клеткой с шашкой 
        for (int dx = -2; dx <= 2; dx += 4) {                   // противника. Если такая угроза обнаружена, то шашка противника в опасности
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


