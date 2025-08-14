#ifndef GAMES_H
#define GAMES_H

#include "video.h"
#include <stddef.h>
#include <stdint.h>

// Each game provides a 14x20 screen buffer (ASCII) to render
typedef struct GameEntry {
    const char *name; // null-terminated, max 32 suggested
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH]; // optional initial screen
    void (*run)(void); // optional game loop entry (blocking until exit)
} GameEntry;

extern const GameEntry g_games[];
extern const size_t g_games_count;

// Weak declarations so manifest can reference symbols from games/* without explicit headers
__attribute__((weak)) void game_snake_run(void);
__attribute__((weak)) void game_input_test_run(void);

#endif
