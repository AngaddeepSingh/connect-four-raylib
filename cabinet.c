#include <raylib.h>
#include <math.h>
#include <string.h>
#include "cabinet.h"
#include "connect.h"

/* Main UI Color */
static const Color COL_BODY       = {18, 14, 28, 255};
static const Color COL_PINK       = {255, 61, 166, 255};
static const Color COL_TEAL       = {0, 255, 200, 255};
static const Color COL_SCREEN     = {4, 26, 28, 255};
static const Color COL_PLATE      = {34, 26, 53, 255};
static const Color COL_PLATE_EDGE = {61, 47, 94, 255};

/* Joy Stick Colors */
static const Color Plate_Fill     = {23, 18, 31, 255};
static const Color Plate_Edge     = {61, 47, 94, 255};
static const Color Corner_Screws  = {107, 101, 128, 255};
static const Color Base_Circle    = {13, 13, 20, 255};
static const Color Shaft          = {154, 154, 168, 255};
static const Color Ball_Handle    = {229, 57, 53, 255};

/* Other Colors */
static const Color Select_Pills     = {138, 138, 160, 255};
static const Color Speaker_Grill  = {13, 13, 20, 255};

/* Each Pannel Peices Position */
#define JOY_X      108
#define JOY_Y      680
#define PILLS_X    260
#define PILLS_Y    700
#define LIGHTS_X   260
#define LIGHTS_Y   660
#define GRILLE_X   392
#define GRILLE_Y   680
#define MINI_X     598
#define MINI_Y     680
#define MINI_W     304
#define MINI_H     80

/* Pulse for glows */
static float GetPulse(float speed) {
    return 0.65f + 0.35f * sinf((float)GetTime() * speed);
}

void DrawRoundedBox(Rectangle rect, float radius, float border, Color fill, Color edge) {
    float shorter = (rect.width < rect.height) ? rect.width : rect.height;
    DrawRectangleRounded(rect, radius / (shorter / 2.0f), 12, edge);

    Rectangle inner = {
        rect.x + border,
        rect.y + border,
        rect.width - border * 2,
        rect.height - border * 2
    };
    shorter = (inner.width < inner.height) ? inner.width : inner.height;
    DrawRectangleRounded(inner, (radius - border) / (shorter / 2.0f), 12, fill);
}

Rectangle GetScreenRect(void) {
    Rectangle r = { 48, SCREEN_Y + 8, WINDOW_W - 96, SCREEN_H - 16 };
    return r;
}

static void DrawJoystick(int cx, int cy) {
    int size = 83;
    Rectangle plate = { cx - size / 2, cy - size / 2, size, size };

    DrawRectangleRounded(plate, 0.3f, 12, Plate_Fill);
    DrawRectangleRoundedLines(plate, 0.3f, 12, Plate_Edge);

    DrawCircle(cx + 32, cy + 32, 3, Corner_Screws);
    DrawCircle(cx + 32, cy - 32, 3, Corner_Screws);
    DrawCircle(cx - 32, cy - 32, 3, Corner_Screws);
    DrawCircle(cx - 32, cy + 32, 3, Corner_Screws);

    DrawCircle(cx, cy, 23, Base_Circle);
    
    Vector2 start = {cx, cy};
    Vector2 end   = {cx, cy - 28};
    DrawLineEx(start, end, 8.0f, Shaft);

    DrawCircle(cx, cy - 37, 18, Ball_Handle);
    DrawCircle(cx - 7, cy - 44, 5, Fade(WHITE, 0.5f));

}

static void DrawStartSelect(int cx, int cy) {
    Rectangle pill_1 = { cx - 26 - 43 / 2.0f, cy - 12 / 2.0f, 43, 12 };
    Rectangle pill_2 = { cx + 26 - 43 / 2.0f, cy - 12 / 2.0f, 43, 12 };

    DrawRectangleRounded(pill_1, 1.0f, 16, Select_Pills);
    DrawRectangleRounded(pill_2, 1.0f, 16, Select_Pills);
}

static void DrawIndicatorLights(int currentPlayer, int cx, int cy) {
    int size = 5;

    DrawCircle(cx - 27, cy, size, ColorBrightness(COL_PLAYER1, -0.7f));
    DrawCircle(cx + 27, cy, size, ColorBrightness(COL_PLAYER2, -0.7f));

    if (currentPlayer == PLAYER_1) {
        DrawCircle(cx - 27, cy, size, COL_PLAYER1);
        DrawCircle(cx - 27, cy, size + 3, Fade(COL_PLAYER1, 0.25f));
    } else {
        DrawCircle(cx + 27, cy, size, COL_PLAYER2);
        DrawCircle(cx + 27, cy, size + 3, Fade(COL_PLAYER2, 0.25f));
    }
}

static void DrawSpeakerGrille(int cx, int cy) {
    int size_x = 67;
    int size_y = 15;
    int x_change = 10;
    
    Vector2 start_1 = {cx - (size_x / 2) - x_change, cy - (size_y * 0.5f)};
    Vector2 start_2 = {cx - (size_x / 2) - x_change, cy - (size_y * 1.5f)};
    Vector2 start_3 = {cx - (size_x / 2) - x_change, cy + (size_y * 0.5f)};
    Vector2 start_4 = {cx - (size_x / 2) - x_change, cy + (size_y * 1.5f)};

    Vector2 end_1   = {cx + (size_x / 2) - x_change, cy - (size_y * 0.5f)};
    Vector2 end_2   = {cx + (size_x / 2) - x_change, cy - (size_y * 1.5f)};
    Vector2 end_3   = {cx + (size_x / 2) - x_change, cy + (size_y * 0.5f)};
    Vector2 end_4   = {cx + (size_x / 2) - x_change, cy + (size_y * 1.5f)};

    DrawLineEx(start_1, end_1, 5.0f, Speaker_Grill);
    DrawLineEx(start_2, end_2, 5.0f, Speaker_Grill);
    DrawLineEx(start_3, end_3, 5.0f, Speaker_Grill);
    DrawLineEx(start_4, end_4, 5.0f, Speaker_Grill);
}

static void DrawMiniScreen(int cx, int cy, const char *message, Color messageColor, int glow) {
    Rectangle frame = { cx - MINI_W / 2, cy - MINI_H / 2, MINI_W, MINI_H };
    Rectangle inner = { frame.x + 6, frame.y + 6, frame.width - 12, frame.height - 12 };
    Rectangle view  = { inner.x + 3, inner.y + 3, inner.width - 6, inner.height - 6 };

    static char lastMessage[64] = "";
    static double startTime = 0;
    if (message != NULL && strcmp(message, lastMessage) != 0) {
        strncpy(lastMessage, message, sizeof(lastMessage) - 1);
        lastMessage[sizeof(lastMessage) - 1] = '\0';
        startTime = GetTime();
    }

    float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 6.0f);

    if (glow) {
        for (int i = 1; i <= 10; i++) {
            Rectangle r = { frame.x - i, frame.y - i, frame.width + i * 2, frame.height + i * 2 };
            float radius = 12 + i;
            float roundness = radius / (r.height / 2.0f);
            float alpha = (0.35f - i * 0.032f) * (0.5f + 0.5f * pulse);
            DrawRectangleRoundedLinesEx(r, roundness, 12, 1, Fade(messageColor, alpha));
        }
    }

    DrawRectangleRounded((Rectangle){ frame.x, frame.y + 2, frame.width, frame.height }, 0.30f, 12, (Color){ 90, 75, 130, 255 });
    DrawRectangleRounded((Rectangle){ frame.x, frame.y - 2, frame.width, frame.height }, 0.30f, 12, (Color){ 4, 3, 8, 255 });

    DrawRoundedBox(frame, 12, 4, (Color){ 20, 16, 30, 255 }, (Color){ 61, 47, 94, 255 });

    Color screenFill = (Color){ 3, 5, 8, 255 };
    Color screenEdge = (Color){ 0, 90, 75, 255 };
    if (glow) {
        screenFill = ColorLerp(screenFill, messageColor, 0.10f * pulse);
        screenEdge = messageColor;
    }
    DrawRoundedBox(inner, 8, 2, screenFill, screenEdge);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

    if (message != NULL) {
        int fontSize = 26;
        int textW = MeasureText(message, fontSize);
        float period = textW + 60;
        float dist = (float)(GetTime() - startTime) * 100.0f;
        float offset = fmodf(dist, period);
        int passes = (int)(dist / period);
        float x0 = view.x + view.width - offset;
        int textY = (int)(view.y + (view.height - fontSize) / 2);

        for (int k = 0; k <= passes; k++) {
            float x = x0 - k * period;
            if (x + textW < view.x) break;
            DrawText(message, (int)x, textY, fontSize, messageColor);
        }
    }

    for (int y = (int)view.y; y < (int)(view.y + view.height); y += 3) {
        DrawLine((int)view.x, y, (int)(view.x + view.width), y, Fade(BLACK, 0.35f));
    }

    DrawRectangleGradientV((int)view.x, (int)view.y, (int)view.width, (int)(view.height * 0.45f), Fade(WHITE, 0.10f), Fade(WHITE, 0.0f));

    EndScissorMode();
}

static void DrawPanel(int currentPlayer, const char *message, Color messageColor, int glow) {
    Rectangle plate = { 30, PANEL_Y + 10, WINDOW_W - 60, PANEL_H - 30 };
    DrawRoundedBox(plate, 20, 2, COL_PLATE, COL_PLATE_EDGE);

    DrawJoystick(JOY_X, JOY_Y);
    DrawStartSelect(PILLS_X, PILLS_Y);
    DrawIndicatorLights(currentPlayer,LIGHTS_X, LIGHTS_Y);
    DrawSpeakerGrille(GRILLE_X, GRILLE_Y);
    DrawMiniScreen(MINI_X, MINI_Y, message, messageColor, glow);
}

static void DrawHeader(void) {
    int midY = HEADER_Y + HEADER_H / 2;

    DrawLineEx((Vector2){ 70, midY },  (Vector2){ 300, midY }, 4, COL_PINK);
    DrawLineEx((Vector2){ 500, midY }, (Vector2){ 730, midY }, 4, COL_PINK);

    const char *title = "CONNECT FOUR";

    int fontSize = 24;
    int textW = MeasureText(title, fontSize);

    DrawText(title, WINDOW_W / 2 - textW / 2, midY - fontSize / 2, fontSize, COL_PINK);
} 

static void DrawBezel(void) {
    Rectangle screen = GetScreenRect();
    DrawRoundedBox(screen, 30, 3, COL_SCREEN, ColorBrightness(COL_TEAL, -0.4f * (1.0f - GetPulse(2.0f))));
}

void DrawCabinet(int currentPlayer, const char *message, Color messageColor, int glow) {
    Rectangle body = { 0, 0, WINDOW_W, WINDOW_H };
    DrawRoundedBox(body, 48, 3, COL_BODY, COL_PINK);

    DrawHeader();
    DrawBezel();
    DrawPanel(currentPlayer, message, messageColor, glow);
}

void DrawScreenEffects(void) {
    Rectangle s = GetScreenRect();

    int depth = 30;
    for (int i = 0; i < depth; i += 2) {
        Rectangle r = { s.x + 3 + i, s.y + 3 + i, s.width - 6 - 2 * i, s.height - 6 - 2 * i };

        float radius = 27 - i;
        if (radius < 2) radius = 2;              
        float roundness = radius / (r.height / 2.0f);

        float fade = 1.0f - (float)i / depth;
        DrawRectangleRoundedLinesEx(r, roundness, 12, 2, Fade(BLACK, 0.4f * fade * fade));
    }

    int offset = (int)(GetTime() * 10) % 4;
    for (int y = (int)s.y + 3 + offset; y < (int)(s.y + s.height) - 3; y += 4) {
    DrawLine((int)s.x + 10, y, (int)(s.x + s.width) - 10, y, Fade(BLACK, 0.25f));
    }

    for (int i = 1; i <= 8; i++) {
    Rectangle r = { s.x + 3 + i, s.y + 3 + i, s.width - 6 - 2 * i, s.height - 6 - 2 * i };
    float radius = 27 - i;                          
    float roundness = radius / (r.height / 2.0f);
    float alpha = (0.25f - i * 0.03f) * GetPulse(2.0f);
    DrawRectangleRoundedLinesEx(r, roundness, 12, 1, Fade(COL_TEAL, alpha));
    }
}
