#ifndef CABINET_H
#define CABINET_H

#include <raylib.h>
#include "connect.h"

#define WINDOW_W 800
#define WINDOW_H 800

#define HEADER_Y  0
#define HEADER_H  40

#define SCREEN_Y  40
#define SCREEN_H  560

#define PANEL_Y   610
#define PANEL_H   150

#define BASE_Y    760
#define BASE_H    40

void DrawCabinet(int currentPlayer, const char *message, Color messageColor, int glow);

void DrawScreenEffects(void);

Rectangle GetScreenRect(void);

void DrawRoundedBox(Rectangle rect, float radius, float border, Color fill, Color edge);

#endif
