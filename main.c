#include "video.h"
#include "utils.h"
#include "teclado.h"

void main() {
    init_keyboard();
    init_framebuffer();
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);

    char start_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {
        "          ",
        "          ",
        "          ",
        " CODEPLAY ",
        "          ",
        "          ",
        "          "
    };
    write_on_screen(start_screen);
    for (volatile int i = 0; i < 100000000; i++);

    for(int i = 0; i < SCREEN_HEIGHT; i++){
        for(int j = 0; j < SCREEN_WIDTH; j++){
            start_screen[i][j] = 'X';
            write_on_screen(start_screen);
            for (volatile int i = 0; i < 1000000; i++);
        }
    }

    char choose_game[SCREEN_HEIGHT][SCREEN_WIDTH] = {
        "pick game",
        " 1  Snake",
        " 2 Tetris",
        " 3  Pong ",
        " 4  Empty",
        " 5  Empty",
        " 6  Empty"
    };
    write_on_screen(choose_game);
    for (volatile int i = 0; i < 100000; i++);

    while (1) {
            // Modo interativo
            if (g_key_pressed) {
                // handle_keyboard_input(g_last_key);

                if (g_last_key == '1'){
                    char snake_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {
                        "SNAKE GAME",
                        "          ",
                        "          ",
                        "          ",
                        "          ",
                        "          ",
                        "          "
                    };
                    write_on_screen(snake_screen);
                } else if (g_last_key == '2') {
                    char tetris_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {
                        "TETRISGAME",
                        "          ",
                        "          ",
                        "          ",
                        "          ",
                        "          ",
                        "          "
                    };
                    write_on_screen(tetris_screen);
                } else if (g_last_key == '3') {
                    char pong_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {
                        " PONG GAME",
                        "          ",
                        "          ",
                        "          ",
                        "          ",
                        "          ",
                        "          "
                    };
                    write_on_screen(pong_screen);
                }
                g_key_pressed = 0;
            }
            
            // Pequeno delay para não sobrecarregar a CPU
            for (volatile int i = 0; i < 100000; i++);
        }
    return;
}
