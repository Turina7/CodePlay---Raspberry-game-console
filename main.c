#include "video.h"
#include "utils.h"
#include "teclado.h"
#include "games.h"

void main() {
    init_keyboard();
    init_framebuffer();
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);

    char start_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    for (int r = 0; r < SCREEN_HEIGHT; ++r) {
        for (int c = 0; c < SCREEN_WIDTH; ++c) start_screen[r][c] = ' ';
    }
    const char *banner = " CODEPLAY ";
    for (int i = 0; i < SCREEN_WIDTH && banner[i]; ++i) start_screen[SCREEN_HEIGHT/2][i] = banner[i];
    write_on_screen(start_screen);
    for (volatile int i = 0; i < 100000000; i++);

    for(int i = 0; i < SCREEN_HEIGHT; i++){
        for(int j = 0; j < SCREEN_WIDTH; j++){
            start_screen[i][j] = 'X';
            write_on_screen(start_screen);
            for (volatile int i = 0; i < 1000000; i++);
        }
    }

    // Montar tela dinamicamente com até 6 primeiras entradas
    char choose_game[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    // título centralizado
    const char *title = "pick game";
    for (int i = 0; i < SCREEN_WIDTH && title[i]; ++i) choose_game[0][i] = title[i];
    for (int line = 1; line < SCREEN_HEIGHT; ++line) {
        for (int col = 0; col < SCREEN_WIDTH; ++col) choose_game[line][col] = ' ';
    }
    // Lista até 6 jogos
    int max_list = SCREEN_HEIGHT - 1;
    for (int idx = 0; idx < (int)g_games_count && idx < max_list; ++idx) {
        // formato: " n  NAME" (10 colunas). n de 1..6
        int line = idx + 1;
        for (int col = 0; col < SCREEN_WIDTH; ++col) choose_game[line][col] = ' ';
        char number_char = (char)('1' + idx);
        choose_game[line][0] = ' ';
        choose_game[line][1] = number_char;
        choose_game[line][2] = ' ';
        choose_game[line][3] = ' ';
        // Copia nome truncado
        for (int j = 0; j < SCREEN_WIDTH - 4; ++j) {
            char c = g_games[idx].name[j];
            if (!c) break;
            choose_game[line][4 + j] = c;
        }
    }
    write_on_screen(choose_game);
    for (volatile int i = 0; i < 100000; i++);

    while (1) {
            // Modo interativo
            if (g_key_pressed) {
                if (g_last_key >= '1' && g_last_key <= '9') {
                    int selection = (int)(g_last_key - '1');
                    if (selection >= 0 && selection < (int)g_games_count) {
                        if (g_games[selection].run) {
                            g_games[selection].run();
                        } else {
                            write_on_screen(g_games[selection].screen);
                        }
                    }
                }
                g_key_pressed = 0;
            }
            
            // Pequeno delay para não sobrecarregar a CPU
            for (volatile int i = 0; i < 100000; i++);
        }
    return;
}
