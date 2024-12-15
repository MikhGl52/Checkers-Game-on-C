
#include "bot.h"
#include "main.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// 7u
int evaluatePosition() {
    int score = 0;
    // ������ ������ �������: ���������� ����� �� �����
    for (int y = 0; y < numRows; y++) {
        for (int x = 0; x < numCols; x++) {
            if (field[y][x].checker == black) {
                score += 1;
            }
            else if (field[y][x].checker == white) {
                score -= 1;
            }
        }
    }
    return score;
}

// ����������� ������� ��� ������ ���������� ���� � �������������� ��������� ��������
int minimax(int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0 || HasValidCaptures() == false) {
        return evaluatePosition();
    }

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int fromY = 0; fromY < numRows; fromY++) {
            for (int fromX = 0; fromX < numCols; fromX++) {
                if (field[fromY][fromX].checker == black) {
                    for (int toY = 0; toY < numRows; toY++) {
                        for (int toX = 0; toX < numCols; toX++) {
                            if (IsValidMove(fromX, fromY, toX, toY) || IsValidCapture(fromX, fromY, toX, toY)) {
                                MoveChecker(fromX, fromY, toX, toY);
                                int eval = minimax(depth - 1, alpha, beta, false);
                                MoveChecker(toX, toY, fromX, fromY); // ���������� ����� �����
                                maxEval = std::max(eval, maxEval);
                                alpha = std::max(alpha, eval);
                                if (beta <= alpha) {
                                    return maxEval;
                                }
                            }
                        }
                    }
                }
            }
        }
        return maxEval;
    }
    else {
        int minEval = INT_MAX;
        for (int fromY = 0; fromY < numRows; fromY++) {
            for (int fromX = 0; fromX < numCols; fromX++) {
                if (field[fromY][fromX].checker == white) {
                    for (int toY = 0; toY < numRows; toY++) {
                        for (int toX = 0; toX < numCols; toX++) {
                            if (IsValidMove(fromX, fromY, toX, toY) || IsValidCapture(fromX, fromY, toX, toY)) {
                                MoveChecker(fromX, fromY, toX, toY);
                                int eval = minimax(depth - 1, alpha, beta, true);
                                MoveChecker(toX, toY, fromX, fromY); // ���������� ����� �����
                                minEval = std::min(eval, minEval);
                                beta = std::min(beta, eval);
                                if (beta <= alpha) {
                                    return minEval;
                                }
                            }
                        }
                    }
                }
            }
        }
        return minEval;
    }
}

void MakeBotMove() {
    int bestFromX = -1, bestFromY = -1, bestToX = -1, bestToY = -1;
    int bestScore = INT_MIN;
    bool mustCapture = HasValidCaptures();


    for (int fromY = 0; fromY < numRows; fromY++) {
        for (int fromX = 0; fromX < numCols; fromX++) {
            if (field[fromY][fromX].checker == black) {
                for (int toY = 0; toY < numRows; toY++) {
                    for (int toX = 0; toX < numCols; toX++) {
                        if (IsValidMove(fromX, fromY, toX, toY) || IsValidCapture(fromX, fromY, toX, toY)) {
                            MoveChecker(fromX, fromY, toX, toY);
                            int score = minimax(3, INT_MIN, INT_MAX, false); // ������� ������ 3
                            MoveChecker(toX, toY, fromX, fromY); // ���������� ����� �����
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

    if (bestFromX != -1 && bestFromY != -1 && bestToX != -1 && bestToY != -1) {
        MoveChecker(bestFromX, bestFromY, bestToX, bestToY);
        printf("Bot move: (%d, %d) to (%d, %d) with score %d\n", bestFromX, bestFromY, bestToX, bestToY, bestScore);
    }
    else {
        makeRandomMove(); // ���� �� ������ ������ ���, ������ ��������� ���
    }
}

// ������� ���������, �������� �� ����� ��� ������� ����� ���������� ����
BOOL WouldBeUnderThreatAfterMove(int fromX, int fromY, int toX, int toY) {
    int tempChecker = field[toY][toX].checker;
    bool tempQueen = field[toY][toX].queen;
    // ���������� �����
    field[toY][toX].checker = field[fromY][fromX].checker;
    field[toY][toX].queen = field[fromY][fromX].queen;
    field[fromY][fromX].checker = empty;
    field[fromY][fromX].queen = false;

    // ���������, ����� �� ����� ��� �������
    BOOL underThreat = WouldBeUnderThreat(toX, toY);

    // ���������� ���� � �������� ���������
    field[fromY][fromX].checker = field[toY][toX].checker;
    field[fromY][fromX].queen = field[toY][toX].queen;
    field[toY][toX].checker = tempChecker;
    field[toY][toX].queen = tempQueen;

    return underThreat;
}

// ���������, ��������� �� ����� ��� ������� �� ����������
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

// ���������, ��������� �� ����� ���������� ��� �������
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
