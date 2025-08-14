#include "video.h"
#include "teclado.h"
#include <stddef.h>

#define MAX_SNAKE_LEN1 256

static char screen[SCREEN_HEIGHT][SCREEN_WIDTH];
static int snake_r1[MAX_SNAKE_LEN1];
static int snake_c1[MAX_SNAKE_LEN1];
static int snake_r2[MAX_SNAKE_LEN1];
static int snake_c2[MAX_SNAKE_LEN1];
static int snake_len1;
static int snake_len2;
static int apple_r, apple_c;

static unsigned int g_seed = 1;


static const uint32_t green = 0xFF00FF00;
static const uint32_t black = 0xFF000000;
static const uint32_t redxx = 0xFFFF0000;
static const uint32_t blue = 0xFF0000FF;

void writeVictory1(){
    char win_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    for (int r = 0; r < SCREEN_HEIGHT; ++r) {
        for (int c = 0; c < SCREEN_WIDTH; ++c) win_screen[r][c] = ' ';
    }
    const char *banner = " Green Player Wins ";
    for (int i = 0; i < SCREEN_WIDTH && banner[i]; ++i) win_screen[SCREEN_HEIGHT/2][i] = banner[i];
    write_on_screen(win_screen);
}

void writeVictory2(){
    char win_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    for (int r = 0; r < SCREEN_HEIGHT; ++r) {
        for (int c = 0; c < SCREEN_WIDTH; ++c) win_screen[r][c] = ' ';
    }
    const char *banner = " Blue Player Wins ";
    for (int i = 0; i < SCREEN_WIDTH && banner[i]; ++i) win_screen[SCREEN_HEIGHT/2][i] = banner[i];
    write_on_screen(win_screen);
}

void writeTie(){
    char win_screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    for (int r = 0; r < SCREEN_HEIGHT; ++r) {
        for (int c = 0; c < SCREEN_WIDTH; ++c) win_screen[r][c] = ' ';
    }
    const char *banner = " Its a fucking tie ";
    for (int i = 0; i < SCREEN_WIDTH && banner[i]; ++i) win_screen[SCREEN_HEIGHT/2][i] = banner[i];
    write_on_screen(win_screen);
}


static uint32_t font_snake_head[5][5] = {
    {green, green, green, green, green},
    {green, black, green, black, green},
    {green, green, black, green, green},
    {green, green, green, green, green},
    {green, green, green, green, green}
};


static uint32_t font_snake_body[5][5] = {
    {black, green, green, green, black},
    {green, green, green, green, green},
    {green, green, green, green, green},
    {green, green, green, green, green},
    {black, green, green, green, black}
};

// Snake 2 head (blue)
static uint32_t font_snake2_head[5][5] = {
    {blue, blue, blue, blue, blue},
    {blue, black, blue, black, blue},
    {blue, blue, black, blue, blue},
    {blue, blue, blue, blue, blue},
    {blue, blue, blue, blue, blue}
};

// Snake 2 body (blue)
static uint32_t font_snake2_body[5][5] = {
    {black, blue, blue, blue, black},
    {blue, blue, blue, blue, blue},
    {blue, blue, blue, blue, blue},
    {blue, blue, blue, blue, blue},
    {black, blue, blue, blue, black}
};

static uint32_t font_apple[5][5] = {
    {black, black, redxx, black, black},
    {black, redxx, redxx, redxx, black},
    {redxx, redxx, redxx, redxx, redxx},
    {black, redxx, redxx, redxx, black},
    {black, black, redxx, black, black}
};

static void clear_screen(void) {
    for (int r = 0; r < SCREEN_HEIGHT; ++r) {
        for (int c = 0; c < SCREEN_WIDTH; ++c) {
            screen[r][c] = ' ';
        }
    }
}

static int prng_int(int max) {
    g_seed = g_seed * 1103515245 + 12345;
    return (unsigned int)(g_seed / 65536) % max;
}

static void place_apple(void) {
    int valid_position;
    do {
        valid_position = 1;
        apple_r = prng_int(SCREEN_HEIGHT);
        apple_c = prng_int(SCREEN_WIDTH);

        // Verifica se a nova posição da maçã não está no corpo da cobra
        for (int i = 0; i < snake_len1; ++i) {
            if ((snake_r1[i] == apple_r && snake_c1[i] == apple_c) || (snake_r2[i] == apple_r && snake_c2[i] == apple_c)){
                valid_position = 0; // Posição inválida, tenta de novo
                break;
            }
        }
    } while (!valid_position);
}

static volatile int running = 0;

static void wait_ms(volatile unsigned long ticks) {
    for (volatile unsigned long i = 0; i < ticks; ++i) {
        __asm__ __volatile__("nop");
    }
}

static void get_snake_font(uint32_t (**font)[5], char c)
{
    switch (c)
    {
    case 'a':
        *font = font_apple;
        break;
    case 'b':
        *font = font_snake_body;
        break;
    case 'h':
        *font = font_snake_head;
        break;
    case 'B': // Snake 2 body
        *font = font_snake2_body;
        break;
    case 'H': // Snake 2 head
        *font = font_snake2_head;
        break;
    default:
        *font = NULL;
        break;
    }
}

void plot_emoji(const char text_matrix[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    static uint32_t text_pixel_matrix[MATRIX_HEIGHT][MATRIX_WIDTH];

    // Limpa fundo (preto)
    for (int i = 0; i < MATRIX_HEIGHT; i++)
    {
        for (int j = 0; j < MATRIX_WIDTH; j++)
        {
            text_pixel_matrix[i][j] = 0xFF000000;
        }
    }

    // Desenhar cada caractere
    for (int linha = 0; linha < SCREEN_HEIGHT; linha++)
    {
        for (int coluna = 0; coluna < SCREEN_WIDTH; coluna++)
        {
            char c = text_matrix[linha][coluna];

            if (c == '\0')
                continue;

            uint32_t (*font)[5]; // Ponteiro para array de 5 elementos
            get_snake_font(&font, c);

            if (font != NULL)
            {
                int pixel_x = coluna * CHAR_WIDTH;
                int pixel_y = linha * CHAR_HEIGHT;

                if (pixel_x + 5 <= MATRIX_WIDTH && pixel_y + 5 <= MATRIX_HEIGHT)
                {
                    // Desenhar o caractere usando acesso 2D correto
                    for (int i = 0; i < 5; i++)
                    {
                        for (int j = 0; j < 5; j++)
                        {
                            uint32_t color = font[i][j]; // Acesso 2D correto
                            text_pixel_matrix[pixel_y + i][pixel_x + j] = color;
                        }
                    }
                }
            }
        }
    }

    fill_screen_from_matrix((uint32_t *)text_pixel_matrix, MATRIX_WIDTH, MATRIX_HEIGHT);
}

void game_snake_run(void) {
    running = 1;
    snake_len1 = 3;
    int updateMov = 0;
    int head_r1 = SCREEN_HEIGHT/2;
    int head_c1 = SCREEN_WIDTH/4;
    int dr1 = 0, dc1 = 1; // direita

    snake_len2 = 3;
    int head_r2 = SCREEN_HEIGHT/2;
    int head_c2 = 3*SCREEN_WIDTH/4;
    int dr2 = 0, dc2 = 1; // direita

    snake_r1[0] = head_r1;
    snake_c1[0] = head_c1;

    snake_r2[0] = head_r2;
    snake_c2[0] = head_c2;


    while (running) {
        if (g_key_pressed_p1) {
            char k1 = g_last_key_p1;
            g_key_pressed_p1 = 0;
            if (k1 == 'w' && dr1 == 0) { dr1 = -1; dc1 = 0; }
            else if (k1 == 's' && dr1 == 0) { dr1 = 1; dc1 = 0; }
            else if (k1 == 'a' && dc1 == 0) { dr1 = 0; dc1 = -1; }
            else if (k1 == 'd' && dc1 == 0) { dr1 = 0; dc1 = 1; }
        }
        if (g_key_pressed_p2) {
            char k2 = g_last_key_p2;
            g_key_pressed_p2 = 0;
            if (k2 == 'w' && dr2 == 0) { dr2 = -1; dc2 = 0; }
            else if (k2 == 's' && dr2 == 0) { dr2 = 1; dc2 = 0; }
            else if (k2 == 'a' && dc2 == 0) { dr2 = 0; dc2 = -1; }
            else if (k2 == 'd' && dc2 == 0) { dr2 = 0; dc2 = 1; }
        }

        if (g_last_key == 'm') { running = 0; break; }

        updateMov += 1;
        if (updateMov > 8){ 
            head_r1 += dr1; head_c1 += dc1; updateMov = 0;
            if (head_r1 < 0) head_r1 = SCREEN_HEIGHT-1;
            if (head_r1 >= SCREEN_HEIGHT) head_r1 = 0;
            if (head_c1 < 0) head_c1 = SCREEN_WIDTH-1;
            if (head_c1 >= SCREEN_WIDTH) head_c1 = 0;

            head_r2 += dr2; head_c2 += dc2; updateMov = 0;
            if (head_r2 < 0) head_r2 = SCREEN_HEIGHT-1;
            if (head_r2 >= SCREEN_HEIGHT) head_r2 = 0;
            if (head_c2 < 0) head_c2 = SCREEN_WIDTH-1;
            if (head_c2 >= SCREEN_WIDTH) head_c2 = 0;


            // Calcula a próxima posição da cabeça
            int next_head_r1 = snake_r1[0] + dr1;
            int next_head_c1 = snake_c1[0] + dc1;

            int next_head_r2 = snake_r2[0] + dr2;
            int next_head_c2 = snake_c2[0] + dc2;

            // "Atravessar" as paredes
            if (next_head_r1 < 0) next_head_r1 = SCREEN_HEIGHT - 1;
            if (next_head_r1 >= SCREEN_HEIGHT) next_head_r1 = 0;
            if (next_head_c1 < 0) next_head_c1 = SCREEN_WIDTH - 1;
            if (next_head_c1 >= SCREEN_WIDTH) next_head_c1 = 0;

            if (next_head_r2 < 0) next_head_r2 = SCREEN_HEIGHT - 1;
            if (next_head_r2 >= SCREEN_HEIGHT) next_head_r2 = 0;
            if (next_head_c2 < 0) next_head_c2 = SCREEN_WIDTH - 1;
            if (next_head_c2 >= SCREEN_WIDTH) next_head_c2 = 0;

            // Verifica colisão
            for (int i = 1; i < snake_len1; ++i) {
                if (next_head_r1 == snake_r1[i] && next_head_c1 == snake_c1[i]) {
                    running = 0; // Game Over
                    writeVictory2();
                    while( g_last_key != 'm' && g_last_key != 'M' ){wait_ms(60000);}
                    break;
                }
                if (next_head_r2 == snake_r1[i] && next_head_c2 == snake_c1[i]) {
                    running = 0; // Game Over
                    writeVictory1();
                    while( g_last_key != 'm' && g_last_key != 'M' ){wait_ms(60000);}
                    break;
                }
            }

            if (next_head_r2 == next_head_r1 && next_head_c2 == next_head_c1) {
                running = 0; // Game Over
                writeTie();
                while( g_last_key != 'm' && g_last_key != 'M' ){wait_ms(60000);}
                break;
            }


            for (int i = 1; i < snake_len2; ++i) {
                if (next_head_r1 == snake_r2[i] && next_head_c1 == snake_c2[i]) {
                    running = 0; // Game Over
                    writeVictory2();
                    while( g_last_key != 'm' && g_last_key != 'M' ){wait_ms(60000);}
                    break;
                }
                if (next_head_r2 == snake_r2[i] && next_head_c2 == snake_c2[i]) {
                    running = 0; // Game Over
                    writeVictory1();
                    while( g_last_key != 'm' && g_last_key != 'M' ){wait_ms(60000);}
                    break;
                }
            }

            int ate_apple1 = (next_head_r1 == apple_r && next_head_c1 == apple_c);
            int ate_apple2 = (next_head_r2 == apple_r && next_head_c2 == apple_c);
            if (ate_apple1) {
                if (snake_len1 < MAX_SNAKE_LEN1) {
                    snake_len1++; // Aumenta o tamanho
                }
                place_apple(); // Coloca uma nova maçã
            } else if (ate_apple2)
            {
                if (snake_len2 < MAX_SNAKE_LEN1) {
                    snake_len2++; // Aumenta o tamanho
                }
                place_apple(); // Coloca uma nova maçã            
            }
            
            // Move o corpo da cobra
            for (int i = snake_len1 - 1; i > 0; --i) {
                snake_r1[i] = snake_r1[i - 1];
                snake_c1[i] = snake_c1[i - 1];
            }
            for (int i = snake_len2 - 1; i > 0; --i) {
                snake_r2[i] = snake_r2[i - 1];
                snake_c2[i] = snake_c2[i - 1];
            }

            // Move a cabeça
            snake_r1[0] = next_head_r1;
            snake_c1[0] = next_head_c1;

            snake_r2[0] = next_head_r2;
            snake_c2[0] = next_head_c2;
        }
        clear_screen();


        screen[apple_r][apple_c] = 'a';
        for (int i = 1; i < snake_len1; ++i) {
            screen[snake_r1[i]][snake_c1[i]] = 'b';
        }
        screen[snake_r1[0]][snake_c1[0]] = 'h';

        for (int i = 1; i < snake_len2; ++i) {
            screen[snake_r2[i]][snake_c2[i]] = 'B';
        }
        screen[snake_r2[0]][snake_c2[0]] = 'H';


        plot_emoji(screen);
        wait_ms(50000);
    }
}

