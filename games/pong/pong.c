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

// Colored 5x5 emojis (borrowed concept from Snake)
static const unsigned int COLOR_BLACK = 0xFF000000;
static const unsigned int COLOR_WHITE = 0xFFFFFFFF;
static const unsigned int COLOR_GREEN = 0xFF00FF00; // P1 paddle
static const unsigned int COLOR_BLUE  = 0xFF0000FF; // P2 paddle
static const unsigned int COLOR_RED   = 0xFFFF0000; // Ball

static unsigned int font_paddle_green[5][5] = {
    {COLOR_BLACK, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_BLACK},
    {COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN},
    {COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN},
    {COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN},
    {COLOR_BLACK, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_BLACK}
};

static unsigned int font_paddle_blue[5][5] = {
    {COLOR_BLACK, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLACK},
    {COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE},
    {COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE},
    {COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE},
    {COLOR_BLACK, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE, COLOR_BLACK}
};

static unsigned int font_ball_red[5][5] = {
    {COLOR_BLACK, COLOR_BLACK, COLOR_RED,   COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_BLACK},
    {COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED  },
    {COLOR_BLACK, COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_RED,   COLOR_BLACK, COLOR_BLACK}
};

// Center net (:) glyph (white dots)
static unsigned int font_colon[5][5] = {
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}
};

// White digits 0..9 for scoreboard
static unsigned int font_d0[5][5] = {
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK}
};
static unsigned int font_d1[5][5] = {
    {COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK}
};
static unsigned int font_d2[5][5] = {
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE}
};
static unsigned int font_d3[5][5] = {
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK}
};
static unsigned int font_d4[5][5] = {
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE}
};
static unsigned int font_d5[5][5] = {
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK}
};
static unsigned int font_d6[5][5] = {
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK},
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK}
};
static unsigned int font_d7[5][5] = {
    {COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK},
    {COLOR_BLACK, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK},
    {COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK}
};
static unsigned int font_d8[5][5] = {
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK}
};
static unsigned int font_d9[5][5] = {
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_BLACK, COLOR_WHITE}
};

static void get_pong_font(unsigned int (**font)[5], char c)
{
    switch (c)
    {
    case 'A':
        *font = font_paddle_green;
        break;
    case 'B':
        *font = font_paddle_blue;
        break;
    case 'a':
        *font = font_ball_red;
        break;
    case ':':
        *font = font_colon;
        break;
    case '0': *font = font_d0; break;
    case '1': *font = font_d1; break;
    case '2': *font = font_d2; break;
    case '3': *font = font_d3; break;
    case '4': *font = font_d4; break;
    case '5': *font = font_d5; break;
    case '6': *font = font_d6; break;
    case '7': *font = font_d7; break;
    case '8': *font = font_d8; break;
    case '9': *font = font_d9; break;
    default:
        *font = NULL;
        break;
    }
}

static void plot_emoji_pong(const char text_matrix[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    static uint32_t pixel_matrix[MATRIX_HEIGHT][MATRIX_WIDTH];
    // Clear to black
    for (int i = 0; i < MATRIX_HEIGHT; ++i) {
        for (int j = 0; j < MATRIX_WIDTH; ++j) {
            pixel_matrix[i][j] = COLOR_BLACK;
        }
    }
    // Draw known emoji glyphs
    for (int row = 0; row < SCREEN_HEIGHT; ++row) {
        for (int col = 0; col < SCREEN_WIDTH; ++col) {
            char ch = text_matrix[row][col];
            if (ch == '\0' || ch == ' ') continue;
            unsigned int (*font)[5];
            get_pong_font(&font, ch);
            if (!font) continue; // unknown glyphs ignored (saves code size)

            int px = col * CHAR_WIDTH;
            int py = row * CHAR_HEIGHT;
            if (px + 5 <= MATRIX_WIDTH && py + 5 <= MATRIX_HEIGHT) {
                for (int i = 0; i < 5; ++i) {
                    for (int j = 0; j < 5; ++j) {
                        pixel_matrix[py + i][px + j] = font[i][j];
                    }
                }
            }
        }
    }
    fill_screen_from_matrix((uint32_t *)pixel_matrix, MATRIX_WIDTH, MATRIX_HEIGHT);
}

static void draw_string(int row, int col, const char *s) {
    for (int i = 0; s[i] && col + i < SCREEN_WIDTH; ++i) {
        char ch = s[i];
        if (ch >= 'a' && ch <= 'z') ch = (char)(ch - ('a' - 'A'));
        screen[row][col + i] = ch;
    }
}

static void draw_paddle(int center_row, int col, int height, char glyph) {
    int half = height / 2;
    for (int dr = -half; dr <= half; ++dr) {
        int r = center_row + dr;
        if (r >= 0 && r < SCREEN_HEIGHT) {
            screen[r][col] = glyph;
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

    // Court: expand play area by using edge columns
    const int left_col = 0;
    const int right_col = SCREEN_WIDTH - 1;
    const int paddle_height = 5; // slightly bigger paddles for easier play

    int p1_center_row = SCREEN_HEIGHT / 2;
    int p2_center_row = SCREEN_HEIGHT / 2;

    int ball_row = SCREEN_HEIGHT / 2;
    int ball_col = SCREEN_WIDTH / 2;
    int ball_dr = -1; // up
    int ball_dc = -1; // left

    int score_p1 = 0;
    int score_p2 = 0;

    int tick = 0;
    const int target_score = 5;
    // Initial speeds
    const int initial_tick_threshold = 8;
    const int initial_frame_delay_ticks = 45000;
    int tick_threshold = initial_tick_threshold;
    int frame_delay_ticks = initial_frame_delay_ticks;
    int updates_since_serve = 0; // linear ramp driver during a rally

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
        // Update cadence with difficulty
        if (tick >= tick_threshold) {
            tick = 0;

            int next_row = ball_row + ball_dr;
            int next_col = ball_col + ball_dc;

            // Linear difficulty ramp within a rally (every N updates)
            updates_since_serve++;
            if (updates_since_serve % 8 == 0) {
                if (tick_threshold > 1) tick_threshold -= 1;
                if (frame_delay_ticks > 20000) {
                    frame_delay_ticks  *= 0.8;
                    if (frame_delay_ticks < 20000) frame_delay_ticks = 20000;
                }
            }

            // Bounce on top/bottom and clamp
            if (next_row < 0) { next_row = 0; ball_dr = -ball_dr; }
            else if (next_row >= SCREEN_HEIGHT) { next_row = SCREEN_HEIGHT - 1; ball_dr = -ball_dr; }

            // Paddle collision - left (ball approaches from right)
            if (next_col <= left_col + 1) {
                int half = paddle_height / 2;
                if (next_row >= p1_center_row - half && next_row <= p1_center_row + half) {
                    next_col = left_col + 1; // just in front of paddle
                    ball_dc = -ball_dc;
                    // Add a bit of vertical change based on hit position
                    if (next_row < p1_center_row) ball_dr = -1;
                    else if (next_row > p1_center_row) ball_dr = 1;
                }
            }

            // Paddle collision - right (ball approaches from left)
            if (next_col >= right_col - 1) {
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
                score_p2 = score_p2 + 1;
                ball_row = SCREEN_HEIGHT / 2;
                ball_col = SCREEN_WIDTH / 2;
                ball_dr = (score_p2 % 2 == 0) ? -1 : 1;
                ball_dc = -1; // serve to left player next
                // Reset difficulty at each score
                tick_threshold = initial_tick_threshold;
                frame_delay_ticks = initial_frame_delay_ticks;
                updates_since_serve = 0;

                if (score_p2 >= target_score) {
                    // Winner: P2
                    clear_screen();
                    draw_string(SCREEN_HEIGHT/2 - 1, 3, "PLAYER 2 WINS!");
                    draw_string(SCREEN_HEIGHT/2 + 1, 2, "PRESS M FOR MENU");
                    write_on_screen((const char (*)[SCREEN_WIDTH])screen);
                    // Wait for menu key
                    while (1) {
                        if (g_last_key == 'm' || g_last_key == 'M') break;
                        wait_ticks(60000);
                    }
                    running = 0;
                }
            } else if (next_col >= SCREEN_WIDTH) {
                // P1 scores
                score_p1 = score_p1 + 1;
                ball_row = SCREEN_HEIGHT / 2;
                ball_col = SCREEN_WIDTH / 2;
                ball_dr = (score_p1 % 2 == 0) ? -1 : 1;
                ball_dc = 1; // serve to right player next
                // Reset difficulty at each score
                tick_threshold = initial_tick_threshold;
                frame_delay_ticks = initial_frame_delay_ticks;
                updates_since_serve = 0;

                if (score_p1 >= target_score) {
                    // Winner: P1
                    clear_screen();
                    draw_string(SCREEN_HEIGHT/2 - 1, 3, "PLAYER 1 WINS!");
                    draw_string(SCREEN_HEIGHT/2 + 1, 2, "PRESS M FOR MENU");
                    write_on_screen((const char (*)[SCREEN_WIDTH])screen);
                    while (1) {
                        if (g_last_key == 'm' || g_last_key == 'M') break;
                        wait_ticks(60000);
                    }
                    running = 0;
                }
            } else {
                ball_row = next_row;
                ball_col = next_col;
            }
        }

        // Render
        clear_screen();

        // Optional: very sparse center guide
        for (int r = 2; r < SCREEN_HEIGHT; r += 3) {
            screen[r][SCREEN_WIDTH / 2] = ':';
        }

        // Scores as digits (white)
        screen[0][2] = (char)('0' + (score_p1 % 10));
        screen[0][SCREEN_WIDTH - 3] = (char)('0' + (score_p2 % 10));

        // Paddles (colored using custom emoji 'A' and 'B')
        draw_paddle(p1_center_row, left_col, paddle_height, 'A');
        draw_paddle(p2_center_row, right_col, paddle_height, 'B');

        // Ball: use 'a' to render as red apple from Snake font
        if (ball_row >= 0 && ball_row < SCREEN_HEIGHT && ball_col >= 0 && ball_col < SCREEN_WIDTH) {
            screen[ball_row][ball_col] = 'a';
        }

        plot_emoji_pong((const char (*)[SCREEN_WIDTH])screen);
        wait_ticks((volatile unsigned long)frame_delay_ticks);
    }
}
