
#pragma once
#include <windows.h> 
#include <stdio.h>   
#include <stdbool.h>
#include <stdlib.h>
#ifndef BOT_H
#define BOT_H

typedef struct {
    int fromX, fromY;
    int toX, toY;
} Move;

typedef struct MoveState {
    int fromX, fromY, toX, toY; // ���������� ����
    bool captured;             // ��� �� ������
    int capturedX, capturedY;  // ���������� ����������� ����� (���� ��� ������)
    int prevChecker;           // ����� � ��������� �������
    bool prevQueen;            // ������ ����� � ��������� �������
    int capturedChecker;       // ��� ����������� �����
    bool becameQueen;          // ����� �� ����� ������ ����� ����
}MoveState;

void MakeBotMove();
BOOL WouldBeUnderThreatAfterMove(int fromX, int fromY, int toX, int toY);
BOOL OpponentUnderThreatAfterMove(int fromX, int fromY, int toX, int toY);
BOOL WouldBeUnderThreat(int x, int y);
BOOL OpponentUnderThreat(int x, int y);

#endif // BOT_H
