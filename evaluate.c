#include "bot.h"
#include "main.h"
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <limits.h>

int EvaluateMove(int fromX, int fromY, int toX, int toY, bool mustCapture) {
    int score = 0;

    // Высокий приоритет для захватов
    if (mustCapture && IsValidCapture(fromX, fromY, toX, toY)) 
        return 100; // Захват даёт высокий приоритет
    

    if (!mustCapture && IsValidMove(fromX, fromY, toX, toY)) {
        score += 1; // Базовый ход

        // Движение к дамке
        if ((player == black && toY == 0) || (player == white && toY == numRows - 1))
            score += 5;

        if (WouldBeUnderThreatAfterMove(fromX, fromY, toX, toY))
            score -= 10;

        if (OpponentUnderThreatAfterMove(fromX, fromY, toX, toY) &&
            !WouldBeUnderThreatAfterMove(fromX, fromY, toX, toY))
            score += 10;

    }

    return score;
}

