#include "video.h"
#include "utils.h"

// Usaremos uma matriz 16x16, igual à da bandeira que funcionava.
#define MATRIX_DIM 16

// Cores
#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF

// A matriz que será desenhada na tela.
uint32_t matrix[MATRIX_DIM][MATRIX_DIM];

/*
 * Console Baremetal - Raspberry Pi 2
 * 
 * Animação mais simples possível: um pixel se movendo
 * em uma matriz 16x16 para validar o framebuffer.
 */
void main() {
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);
    
    // Inicialização do display gráfico
    init_framebuffer();

    // Posição e direção do pixel
    int x = 0, y = 0;
    int dx = 1, dy = 1;

    // Loop principal
    while(1) {
        // 1. Limpa a matriz com a cor preta
        for (int r = 0; r < MATRIX_DIM; r++) {
            for (int c = 0; c < MATRIX_DIM; c++) {
                matrix[r][c] = BLACK;
            }
        }

        // 2. Desenha o pixel branco na posição atual
        matrix[y][x] = WHITE;

        // 3. Atualiza a posição para o próximo quadro
        x += dx;
        y += dy;

        // 4. Lógica de "quicar" nas bordas da matriz 16x16
        if (x <= 0 || x >= MATRIX_DIM - 1) {
            dx = -dx;
        }
        if (y <= 0 || y >= MATRIX_DIM - 1) {
            dy = -dy;
        }

        // 5. Renderiza a matriz no back buffer (usando a função que já funcionava)
        fill_screen_from_matrix((uint32_t*)matrix, MATRIX_DIM, MATRIX_DIM);
        
        // 6. Copia para o front buffer (exibe na tela)
        swap_buffers();
        
        // 7. Delay para a animação ser visível
        for (volatile int i = 0; i < 500000; i++);
    }
}

