#include "bot.h"
#include "main.h"
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <limits.h>

/*
whites ++
blacks --
*/

int EvaluateMove(int fromX, int fromY, int toX, int toY, bool mustCapture) {
    int score = 0;

    // 
    if (mustCapture && IsValidCapture(fromX, fromY, toX, toY))
        return 100; // 


    if (!mustCapture && IsValidMove(fromX, fromY, toX, toY)) {
        score += 1; //

        // 
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


int evaluatePosition(Tcell field[numRows][numCols]) {
    printf("---------------------------------\n");
    int winner = checkWin();
    int score = 0;

    if (winner != empty) 
        return (winner == white) ? INT_MAX : INT_MIN;
    

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            Tcell cell = field[i][j];

            if (cell.checker == empty) continue;


            if (cell.checker == white) {
                if (cell.queen == true) {
                    if (i == j)
                        score += 15;
                    else
                        score += 10;
                }
                if (cell.queen == false)
                    score += 1;
                if (WouldBeUnderThreat(j, i))
                    score -= 5;
                if (OpponentUnderThreat(j, i) && !WouldBeUnderThreat(j, i))
                    score += 5;
            }

            if (cell.checker == black) {
                if (cell.queen == true) {
                    if (i == j)
                        score -= 15;
                    else
                        score -= 10;
                }
                if (cell.queen == false)
                    score -= 1;
                if (WouldBeUnderThreat(j, i))
                    score += 5;
                if (OpponentUnderThreat(j, i) && !WouldBeUnderThreat(j, i))
                    score -= 5;
            }
        }
    }
    printf("%d\n", score);
    return score;
}
