#include "video.h"
#include "teclado.h"

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

static volatile int running = 0;
static void wait_ticks(volatile unsigned long ticks) { for (volatile unsigned long i=0;i<ticks;++i) __asm__ __volatile__("nop"); }

void game_input_test_run(void) {
    running = 1;
    int r1 = SCREEN_HEIGHT/2;
    int c1 = SCREEN_WIDTH/2 - 2;
    int r2 = SCREEN_HEIGHT/2;
    int c2 = SCREEN_WIDTH/2 + 2;

    while (running) {
        if (g_key_pressed_p1) {
            char k = g_last_key_p1;
            g_key_pressed_p1 = 0;
            if (k == 'w') { if (r1 > 0) r1--; }
            else if (k == 's') { if (r1 < SCREEN_HEIGHT-1) r1++; }
            else if (k == 'a') { if (c1 > 0) c1--; }
            else if (k == 'd') { if (c1 < SCREEN_WIDTH-1) c1++; }
            else if (k == 'm') { running = 0; break; }
        }
        if (g_key_pressed_p2) {
            char k = g_last_key_p2; // already mapped w/a/s/d from arrows
            g_key_pressed_p2 = 0;
            if (k == 'w') { if (r2 > 0) r2--; }
            else if (k == 's') { if (r2 < SCREEN_HEIGHT-1) r2++; }
            else if (k == 'a') { if (c2 > 0) c2--; }
            else if (k == 'd') { if (c2 < SCREEN_WIDTH-1) c2++; }
        }
        clear_screen();
        draw_string(0, 0, "INPUT TEST   M=MENU");
        draw_string(1, 0, "P1: WASD");
        draw_string(2, 0, "P2: ARROWS");
        screen[r1][c1] = '1';
        screen[r2][c2] = '2';
        write_on_screen((const char (*)[SCREEN_WIDTH])screen);
        wait_ticks(40000);
    }
}
