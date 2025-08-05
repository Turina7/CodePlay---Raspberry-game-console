// ==================== video.h ====================
#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

// Definições para texto na tela
#define SCREEN_HEIGHT 7                                         // Quantidade de linhas de texto
#define SCREEN_WIDTH 10                                         // Quantidade de caracteres por linha
#define MATRIX_WIDTH (SCREEN_WIDTH * 5) + (SCREEN_WIDTH - 1)    // n Caracteres * 5 pixels + n-1 espaços
#define MATRIX_HEIGHT (SCREEN_HEIGHT * 5) + (SCREEN_HEIGHT - 1) // m linhas * 5 pixels + m-1 espaços
#define CHAR_WIDTH 6                                            // 5 pixels + 1 de espaço
#define CHAR_HEIGHT 6                                           // 5 pixels + 1 de espaço

// Declarações das funções
int init_framebuffer();
void fill_screen_from_matrix(uint32_t *matrix, int original_width, int original_height);
void write_on_screen(char text_matrix[SCREEN_HEIGHT][SCREEN_WIDTH]);

#endif