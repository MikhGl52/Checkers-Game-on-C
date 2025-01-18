#pragma once
#ifndef EVALUATE_H
#define EVALUATE_H

int EvaluateMove(int fromX, int fromY, int toX, int toY, bool mustCapture);
int evaluatePosition(Tcell field[numRows][numCols]);

#endif // EVALUATE_H