// Auto-generated. Edit games/* instead.
#include "games.h"
#include <stddef.h>
typedef struct GameEntry GameEntry;
const GameEntry g_games[] = {
  { .name = "Pong", .screen = {
    "        PONG        ",
    "                    ",
    "                    ",
    "  P1: W/S move      ",
    "  P2: Arrows move   ",
    "  Press M for menu  ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
  }, .run = game_pong_run },
  { .name = "Snake", .screen = {
    "      SNAKE         ",
    "                    ",
    "                    ",
    " Use WASD to move   ",
    " Press M for menu   ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
    "                    ",
  }, .run = game_snake_run },
};
const size_t g_games_count = sizeof(g_games)/sizeof(g_games[0]);
