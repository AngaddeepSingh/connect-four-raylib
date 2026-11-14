#ifndef CONNECT_FOUR_H
#define CONNECT_FOUR_H

#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#define ROWS 6
#define COLS 7

#define CELL_SIZE 75
#define BOARD_PAD 12
#define BOARD_W (COLS * CELL_SIZE + BOARD_PAD * 2)
#define BOARD_H (ROWS * CELL_SIZE + BOARD_PAD * 2)

#define COL_BOARD   (Color){26, 18, 51, 255}
#define COL_BOARD_EDGE (Color){127, 119, 221, 255}
#define COL_HOLE    (Color){5, 3, 12, 255}
#define COL_PLAYER1 (Color){255, 61, 166, 220}
#define COL_PLAYER2 (Color){255, 213, 79, 220}
#define CELL_SHINE_PLAYER1 (Color){255, 246, 214, 220}
#define CELL_SHINE_PLAYER2 (Color){255, 209, 232, 220}

#define EMPTY 0
#define PLAYER_1 1
#define PLAYER_2 2

#define status_ongoing 0
#define status_draw 1
#define status_win 2

typedef struct {
    int board[ROWS][COLS];
    int currentPlayer;
    int gameStatus;
    int winner;
    int lastRow;
    int lastCol;
    int moveCount;
    int winMask[ROWS][COLS];
    double winTime;
} ConnectFourGame;

void InitGame(ConnectFourGame *game);
int DropPiece(ConnectFourGame *game, int col);
int checkWin(ConnectFourGame *game, int row, int col);
void DrawBoard(ConnectFourGame *game, int startX, int startY);

int CellCenterX(int boardX, int col);
int CellCenterY(int boardY, int row);

void SpawnLandingBurst(ConnectFourGame *game, int boardX, int boardY);
void UpdateParticles(float dt);
void DrawParticles(void);

int  StartDrop(ConnectFourGame *game, int col, int boardY);
int  UpdateFallingPiece(float dt);
int  IsPieceFalling(void);
void DrawFallingPiece(int boardX, int boardY);

#endif