#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>
#include "connect.h"

#define MAX_PARTICLES 128
typedef struct {
    float x, y, vx, vy;
    float life, maxLife, size;
    Color color;
} Particle;
static Particle particles[MAX_PARTICLES];

#define PIECE_SIZE 55
typedef struct {
    int active;
    int col;
    int player;
    float y;
    float vy;
    float targetY;
} FallingPiece;
static FallingPiece fall;

int CellCenterX(int boardX, int col) {
    return boardX + BOARD_PAD + col * CELL_SIZE + CELL_SIZE / 2;
}

int CellCenterY(int boardY, int row) {
    return boardY + BOARD_PAD + row * CELL_SIZE + CELL_SIZE / 2;
}

static int FindLandingRow(ConnectFourGame *game, int col) {
    for (int row = ROWS - 1; row >= 0; row--) {
        if (game->board[row][col] == EMPTY) return row;
    }
    return -1;
}

int StartDrop(ConnectFourGame *game, int col, int boardY) {
    if (fall.active || game->gameStatus != status_ongoing) return 0;
    if (col < 0 || col >= COLS) return 0;

    int row = FindLandingRow(game, col);
    if (row < 0) return 0;

    fall.active  = 1;
    fall.col     = col;
    fall.player  = game->currentPlayer;
    fall.y       = boardY - PIECE_SIZE / 2;
    fall.vy      = 0;
    fall.targetY = CellCenterY(boardY, row);
    return 1;
}

int UpdateFallingPiece(float dt) {
    if (!fall.active) return -1;

    fall.vy += 2200.0f * dt;
    fall.y  += fall.vy * dt;

    if (fall.y >= fall.targetY) {
        fall.active = 0;
        return fall.col;
    }
    return -1;
}

int IsPieceFalling(void) {
    return fall.active;
}

void InitGame(ConnectFourGame *game) {
    game->currentPlayer = (rand() % 2 == 0) ? PLAYER_1 : PLAYER_2;
    game->gameStatus = status_ongoing;
    game->winner = EMPTY;
    game->lastRow = -1;
    game->lastCol = -1;
    game->moveCount = 0;
    game->winTime = 0;

    fall.active = 0;
    
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            game->board[row][col] = EMPTY;
            game->winMask[row][col] = 0;
        }
    }
}

int DropPiece(ConnectFourGame *game, int col) {
    if (game->gameStatus != status_ongoing || col < 0 || col >= COLS) {
        return 0;
    }

    for (int row = ROWS - 1; row >= 0; row--) {
        if (game->board[row][col] == EMPTY) {
            game->board[row][col] = game->currentPlayer;
            game->lastRow = row;
            game->lastCol = col;
            game->moveCount++;

            if (checkWin(game, row, col)) {
                game->gameStatus = status_win;
                game->winTime = GetTime();
                game->winner = game->currentPlayer;
                return game->gameStatus;
            }
            
            int countCol = 0;
            for (int i = 0; i < COLS; i++) {
                if (game->board[0][i] != EMPTY) {
                    countCol++;
                }
            }

            if (countCol == COLS) {
                game->gameStatus = status_draw;
                return game->gameStatus;
            }

            game->currentPlayer = (game->currentPlayer == PLAYER_1) ? PLAYER_2 : PLAYER_1;
            return status_ongoing;
            
        }
    }

    return status_ongoing;
}

static void DrawPiece(int cx, int cy, Color color, Color shine, float glowAlpha) {
    int size = PIECE_SIZE;

    if (glowAlpha > 0.0f) {
        for (int i = 3; i >= 1; i--) {
            int grow = i * 2;
            Rectangle g = { cx - size / 2 - grow, cy - size / 2 - grow, size + grow * 2, size + grow * 2 };
            DrawRectangleRounded(g, 0.2f, 8, Fade(color, glowAlpha));
        }
    }

    Rectangle cell = { cx - size / 2, cy - size / 2, size, size };
    DrawRectangleRounded(cell, 0.2f, 8, color);

    Rectangle cell_shine = { cx - size / 2 + 4, cy - size / 2 + 4, 10, 10 };
    DrawRectangleRounded(cell_shine, 0.2f, 8, shine);
}

void DrawFallingPiece(int boardX, int boardY) {
    if (!fall.active) return;

    int cx = CellCenterX(boardX, fall.col);
    Color color = (fall.player == PLAYER_1) ? COL_PLAYER1 : COL_PLAYER2;
    Color shine = (fall.player == PLAYER_1) ? CELL_SHINE_PLAYER1 : CELL_SHINE_PLAYER2;

    BeginScissorMode(boardX, boardY, BOARD_W, BOARD_H);
    DrawPiece(cx, (int)fall.y, color, shine, 0.08f);
    EndScissorMode();
}

static void MarkLine(ConnectFourGame *game, int row, int col, int dx, int dy) {
    game->winMask[row][col] = 1;

    for (int dir = -1; dir <= 1; dir += 2) {
        int r = row + dy * dir;
        int c = col + dx * dir;

        while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
            game->winMask[r][c] = 1;
            r += dy * dir;
            c += dx * dir;
        }
    }
}

int checkWin(ConnectFourGame *game, int row, int col) {
    
    int found = 0;

    //Checking Right
    int dx = 1; 
    int dy = 0;
    int count = 1;
    int r = row + dy;
    int c = col + dx;

    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
        count++;
        r += dy;
        c += dx;
    }

    //Checking Left
    r = row - dy;
    c = col - dx;

    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
        count++;
        r -= dy;
        c -= dx;
    }
    
    if (count >= 4) {
        MarkLine(game, row, col, dx, dy);
        found = 1;
    }

    //Checking Down
    dx = 0;
    dy = 1;
    count = 1;
    r = row + dy;
    c = col + dx;

    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
        count++;
        r += dy;
        c += dx;
    }
    
    //Checking Up
    r = row - dy;
    c = col - dx;

    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
        count++;
        r -= dy;
        c -= dx;
    }
    
    if (count >= 4) {
        MarkLine(game, row, col, dx, dy);
        found = 1;
    }

    //Checking Up and Right
    dx = 1;
    dy = -1;
    count = 1;
    r = row + dy;
    c = col + dx;

    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
        count++;
        r += dy;
        c += dx;
    }

    //Checking Down and Left
    r = row - dy;
    c = col - dx;
    
    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
        count++;
        r -= dy;
        c -= dx;
    }
    
    if (count >= 4) {
        MarkLine(game, row, col, dx, dy);
        found = 1;
    }
    
    //Checking Up and Left
    dx = -1;
    dy = -1;
    count = 1;
    r = row + dy;
    c = col + dx;

    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
        count++;
        r += dy;
        c += dx;
    }

    //Checking Down and Right
    r = row - dy;
    c = col - dx;
    
    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && game->board[r][c] == game->currentPlayer) {
        count++;
        r -= dy;
        c -= dx;
    }
    
    if (count >= 4) {
        MarkLine(game, row, col, dx, dy);
        found = 1;
    }

    return found ? 2 : 0;
}

void DrawBoard(ConnectFourGame *game, int startX, int startY) {
    Rectangle Board = {startX, startY, BOARD_W, BOARD_H};

    DrawRectangleRounded(Board, 0.1f, 12, COL_BOARD);
    DrawRectangleRoundedLines(Board, 0.1f, 12, COL_BOARD_EDGE);

    int won = (game->gameStatus == status_win);
    float elapsed = (float)(GetTime() - game->winTime);
    float blink = 0.5f + 0.5f * sinf(elapsed * 12.0f);

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int centerX = CellCenterX(startX, col);
            int centerY = CellCenterY(startY, row);

            Color color = COL_HOLE;
            Color shine = COL_HOLE;
            if (game->board[row][col] == PLAYER_1) {
                color = COL_PLAYER1;
                shine = CELL_SHINE_PLAYER1;
            }
            else if (game->board[row][col] == PLAYER_2) {
                color = COL_PLAYER2;
                shine = CELL_SHINE_PLAYER2;
            }

            int filled = (game->board[row][col] != EMPTY);
            int isWinner = won && game->winMask[row][col];
            float glowAlpha = 0.08f;

            if (!filled) {
                glowAlpha = 0.0f;
            } else if (isWinner) {
                color = ColorLerp(color, WHITE, blink * 0.7f);
                glowAlpha = 0.12f + 0.18f * blink;
            } else if (won) {
                color = Fade(color, 0.35f);
                shine = Fade(shine, 0.35f);
                glowAlpha = 0.0f;
            }

            DrawPiece(centerX, centerY, color, shine, glowAlpha);
        }
    }
}

void SpawnLandingBurst(ConnectFourGame *game, int boardX, int boardY) {
    if (game->lastRow < 0) return;

    float cx = CellCenterX(boardX, game->lastCol);
    float cy = CellCenterY(boardY, game->lastRow);
    Color color = (game->board[game->lastRow][game->lastCol] == PLAYER_1) ? COL_PLAYER1 : COL_PLAYER2;

    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES && spawned < 20; i++) {
        if (particles[i].life > 0) continue;

        particles[i].x = cx;
        particles[i].y = cy;
        particles[i].vx = GetRandomValue(-160, 160);
        particles[i].vy = GetRandomValue(-260, -60);
        particles[i].maxLife = GetRandomValue(40, 80) / 100.0f;
        particles[i].life = particles[i].maxLife;
        particles[i].size = GetRandomValue(3, 10);
        particles[i].color = color;

        spawned++;
    }
}

void UpdateParticles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0) continue;
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
        particles[i].vy += 400 * dt;
        particles[i].life -= dt;
    }
}

void DrawParticles(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0) continue;
        float t = particles[i].life / particles[i].maxLife;
        float s = particles[i].size * t;
        DrawRectangle((int)(particles[i].x - s / 2), (int)(particles[i].y - s / 2 + 30), (int)s, (int)s, Fade(particles[i].color, t));
    }
}