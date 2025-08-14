#include "video.h"
#include "utils.h"
#include "teclado.h"
#include "games.h"

void main() {
    init_keyboard();
    init_framebuffer();
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);

    // Colorful intro animation using pixel matrix directly
    {
        static uint32_t intro[MATRIX_HEIGHT][MATRIX_WIDTH];
        // Clear
        for (int y = 0; y < MATRIX_HEIGHT; ++y) for (int x = 0; x < MATRIX_WIDTH; ++x) intro[y][x] = 0xFF000000;
        // Sweep stripes with changing colors
        for (int step = 0; step < MATRIX_WIDTH; step += 2) {
            uint32_t color = 0xFF0000FF;
            if ((step / 10) % 3 == 1) color = 0xFF00FF00;
            else if ((step / 10) % 3 == 2) color = 0xFFFFFF00;
            for (int y = 0; y < MATRIX_HEIGHT; ++y) {
                int x = step;
                if (x < MATRIX_WIDTH) intro[y][x] = color;
                if (x+1 < MATRIX_WIDTH) intro[y][x+1] = color;
            }
            fill_screen_from_matrix((uint32_t *)intro, MATRIX_WIDTH, MATRIX_HEIGHT);
            for (volatile int d = 0; d < 70000; ++d) __asm__ __volatile__("nop");
        }
        // Render CODEPLAY centered
        char logo[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
        for (int r = 0; r < SCREEN_HEIGHT; ++r) for (int c = 0; c < SCREEN_WIDTH; ++c) logo[r][c] = ' ';
        const char *title = " CODEPLAY ";
        int start_col = (SCREEN_WIDTH - 10) / 2;
        for (int i = 0; title[i] && (start_col + i) < SCREEN_WIDTH; ++i) logo[SCREEN_HEIGHT/2][start_col + i] = title[i];
        write_on_screen(logo);
        for (volatile int d = 0; d < 500000; ++d) __asm__ __volatile__("nop");
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
                // tecla de menu
                if (g_last_key >= '1' && g_last_key <= '9') {
                    int selection = (int)(g_last_key - '1');
                    if (selection >= 0 && selection < (int)g_games_count) {
                        if (g_games[selection].run) {
                            g_games[selection].run();
                            // voltar ao menu após encerrar o jogo
                            write_on_screen(choose_game);
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
