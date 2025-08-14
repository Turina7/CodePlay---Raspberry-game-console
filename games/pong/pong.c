#include "video.h"
#include "teclado.h"
#include <stddef.h>

static char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

static void clear_screen(void) {
    for (int r = 0; r < SCREEN_HEIGHT; ++r) {
        for (int c = 0; c < SCREEN_WIDTH; ++c) {
            screen[r][c] = ' ';
        }
    }
}

static void draw_string(int row, int col, const char *s) {
    for (int i = 0; s[i] && col + i < SCREEN_WIDTH; ++i) {
        char ch = s[i];
        if (ch >= 'a' && ch <= 'z') ch = (char)(ch - ('a' - 'A'));
        screen[row][col + i] = ch;
    }
}

static void draw_paddle(int center_row, int col, int height) {
    int half = height / 2;
    for (int dr = -half; dr <= half; ++dr) {
        int r = center_row + dr;
        if (r >= 0 && r < SCREEN_HEIGHT) {
            screen[r][col] = 'I';
        }
    }
}

static void wait_ticks(volatile unsigned long ticks) {
    for (volatile unsigned long i = 0; i < ticks; ++i) {
        __asm__ __volatile__("nop");
    }
}

void game_pong_run(void) {
    int running = 1;

    // Court
    const int left_col = 1;
    const int right_col = SCREEN_WIDTH - 2;
    const int paddle_height = 5; // must be odd for symmetric drawing

    int p1_center_row = SCREEN_HEIGHT / 2;
    int p2_center_row = SCREEN_HEIGHT / 2;

    int ball_row = SCREEN_HEIGHT / 2;
    int ball_col = SCREEN_WIDTH / 2;
    int ball_dr = -1; // up
    int ball_dc = -1; // left

    int score_p1 = 0;
    int score_p2 = 0;

    int tick = 0;

    while (running) {
        // Input handling
        if (g_key_pressed_p1) {
            char k = g_last_key_p1;
            g_key_pressed_p1 = 0;
            if (k == 'w') { if (p1_center_row > 0) p1_center_row--; }
            else if (k == 's') { if (p1_center_row < SCREEN_HEIGHT - 1) p1_center_row++; }
        }
        if (g_key_pressed_p2) {
            char k = g_last_key_p2; // already mapped to wasd in teclado.c
            g_key_pressed_p2 = 0;
            if (k == 'w') { if (p2_center_row > 0) p2_center_row--; }
            else if (k == 's') { if (p2_center_row < SCREEN_HEIGHT - 1) p2_center_row++; }
        }
        if (g_last_key == 'm' || g_last_key == 'M') { running = 0; break; }

        // Update ball every few ticks for playability
        tick++;
        if (tick >= 3) {
            tick = 0;

            int next_row = ball_row + ball_dr;
            int next_col = ball_col + ball_dc;

            // Bounce on top/bottom
            if (next_row < 0) { next_row = 0; ball_dr = -ball_dr; }
            if (next_row >= SCREEN_HEIGHT) { next_row = SCREEN_HEIGHT - 1; ball_dr = -ball_dr; }

            // Paddle collision - left
            if (next_col == left_col + 1) {
                int half = paddle_height / 2;
                if (next_row >= p1_center_row - half && next_row <= p1_center_row + half) {
                    next_col = left_col + 1; // just in front of paddle
                    ball_dc = -ball_dc;
                    // Add a bit of vertical change based on hit position
                    if (next_row < p1_center_row) ball_dr = -1;
                    else if (next_row > p1_center_row) ball_dr = 1;
                }
            }

            // Paddle collision - right
            if (next_col == right_col - 1) {
                int half = paddle_height / 2;
                if (next_row >= p2_center_row - half && next_row <= p2_center_row + half) {
                    next_col = right_col - 1;
                    ball_dc = -ball_dc;
                    if (next_row < p2_center_row) ball_dr = -1;
                    else if (next_row > p2_center_row) ball_dr = 1;
                }
            }

            // Score conditions
            if (next_col < 0) {
                // P2 scores
                score_p2 = (score_p2 + 1) % 10;
                ball_row = SCREEN_HEIGHT / 2;
                ball_col = SCREEN_WIDTH / 2;
                ball_dr = (score_p2 % 2 == 0) ? -1 : 1;
                ball_dc = -1; // serve to left player next
            } else if (next_col >= SCREEN_WIDTH) {
                // P1 scores
                score_p1 = (score_p1 + 1) % 10;
                ball_row = SCREEN_HEIGHT / 2;
                ball_col = SCREEN_WIDTH / 2;
                ball_dr = (score_p1 % 2 == 0) ? -1 : 1;
                ball_dc = 1; // serve to right player next
            } else {
                ball_row = next_row;
                ball_col = next_col;
            }
        }

        // Render
        clear_screen();

        // Center line (use sparse 'I' markers)
        for (int r = 1; r < SCREEN_HEIGHT; r += 2) {
            screen[r][SCREEN_WIDTH / 2] = 'I';
        }

        // Scores at top center
        screen[0][SCREEN_WIDTH/2 - 2] = (char)('0' + (score_p1 % 10));
        screen[0][SCREEN_WIDTH/2 + 2] = (char)('0' + (score_p2 % 10));

        // Paddles
        draw_paddle(p1_center_row, left_col, paddle_height);
        draw_paddle(p2_center_row, right_col, paddle_height);

        // Ball as 'O'
        if (ball_row >= 0 && ball_row < SCREEN_HEIGHT && ball_col >= 0 && ball_col < SCREEN_WIDTH) {
            screen[ball_row][ball_col] = 'O';
        }

        write_on_screen((const char (*)[SCREEN_WIDTH])screen);
        wait_ticks(40000);
    }
}
