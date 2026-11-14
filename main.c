#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include "connect.h"
#include "cabinet.h"

static const Color COL_TEAL = {0, 255, 200, 255};
static const Color COL_BODY = {18, 14, 28, 255};

int main(void) {
    srand(time(NULL));

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_W, WINDOW_H, "Connect Four - Raylib");

    SetTargetFPS(60);

    ConnectFourGame game;
    InitGame(&game);

    while (!WindowShouldClose()) {
        
        Rectangle screen = GetScreenRect();
        int boardX = screen.x + (screen.width - BOARD_W) / 2;
        int boardY = screen.y + (screen.height - BOARD_H) / 2;

        if (IsKeyPressed(KEY_R)) {
            InitGame(&game);
        }

        if (game.gameStatus == status_ongoing && !IsPieceFalling()) {
            int selectedCol = -1;
            for (int i = 0; i < COLS; i++ ) {
                if (IsKeyPressed(KEY_ONE + i) || IsKeyPressed(KEY_KP_1 + i)) selectedCol = i;
            }

            if (selectedCol != -1) StartDrop(&game, selectedCol, boardY);

        }
        int landedCol = UpdateFallingPiece(GetFrameTime());

        if (landedCol >= 0) {
            int result = DropPiece(&game, landedCol);
            SpawnLandingBurst(&game, boardX, boardY);

            if (result == status_win)  printf("Player %d wins!!\n", game.winner);
            if (result == status_draw) printf("Its a draw!!\n");
        }

        UpdateParticles(GetFrameTime());

        char message[48];
        Color messageColor = (Color){ 0, 140, 115, 255 };
        int glow = 0;

        if (game.gameStatus == status_ongoing) {
            snprintf(message, sizeof(message), "PICK 1 - 7 TO DROP   R = RESTART");
        } else if (game.gameStatus == status_win) {
            snprintf(message, sizeof(message), "PLAYER %s WINS!", (game.winner == PLAYER_1) ? "PINK" : "YELLOW");
            messageColor = (game.winner == PLAYER_1) ? COL_PLAYER1 : COL_PLAYER2;
            glow = 1;
        } else {
            snprintf(message, sizeof(message), "ITS A DRAW!");
            messageColor = (Color){ 0, 255, 200, 255 };
            glow = 1;
        }

        BeginDrawing();
            ClearBackground((Color){ 10, 8, 16, 255 });
 
            DrawCabinet(game.currentPlayer, message, messageColor, glow);
            DrawBoard(&game, boardX, boardY);
            DrawFallingPiece(boardX, boardY);
            DrawParticles();
            DrawScreenEffects();

        EndDrawing();
    }
 
    CloseWindow();
    return 0;
}