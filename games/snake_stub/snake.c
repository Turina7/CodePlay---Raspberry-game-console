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

static volatile int running = 0;

static void wait_ms(volatile unsigned long ticks) {
    for (volatile unsigned long i = 0; i < ticks; ++i) {
        __asm__ __volatile__("nop");
    }
}

void game_snake_run(void) {
    running = 1;
    int head_r = SCREEN_HEIGHT/2;
    int head_c = SCREEN_WIDTH/2;
    int dr = 0, dc = 1; // direita

    while (running) {
        if (g_key_pressed) {
            char k = g_last_key;
            g_key_pressed = 0;
            if (k == 'w') { dr = -1; dc = 0; }
            else if (k == 's') { dr = 1; dc = 0; }
            else if (k == 'a') { dr = 0; dc = -1; }
            else if (k == 'd') { dr = 0; dc = 1; }
            else if (k == 'm') { running = 0; break; }
        }

        head_r += dr; head_c += dc;
        if (head_r < 0) head_r = SCREEN_HEIGHT-1;
        if (head_r >= SCREEN_HEIGHT) head_r = 0;
        if (head_c < 0) head_c = SCREEN_WIDTH-1;
        if (head_c >= SCREEN_WIDTH) head_c = 0;

        clear_screen();
        screen[head_r][head_c] = 'o';
        write_on_screen(screen);
        wait_ms(50000);
    }
}
