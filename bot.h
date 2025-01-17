
#pragma once
#include <windows.h> 
#include <stdio.h>   
#include <stdlib.h>
#ifndef BOT_H
#define BOT_H

void MakeBotMove();
BOOL WouldBeUnderThreatAfterMove(int fromX, int fromY, int toX, int toY);
BOOL OpponentUnderThreatAfterMove(int fromX, int fromY, int toX, int toY);

#endif // BOT_H
