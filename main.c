#include "video.h"
#include "utils.h"

// Cores
#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF

/*
 * Console Baremetal - Raspberry Pi 2
 * 
 * Animação de um retângulo usando desenho direto no back buffer.
 * Esta abordagem evita matrizes globais no main, prevenindo
 * problemas de carregamento do kernel.
 */
void main() {
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);
    
    // Inicialização do display gráfico
    init_framebuffer();

    // Posição e direção do retângulo
    int x = 100, y = 100;
    int dx = 2, dy = 2;

    // Loop principal
    while(1) {
        // 1. Limpa o back buffer com a cor preta
        clear_back_buffer(BLACK);

        // 2. Desenha um retângulo branco na posição atual
        draw_rect_on_back_buffer(x, y, 50, 50, WHITE);

        // 3. Atualiza a posição para o próximo quadro
        x += dx;
        y += dy;

        // 4. Lógica de "quicar" nas bordas (valores fixos para segurança)
        if (x < 10 || x > 800) {
            dx = -dx;
        }
        if (y < 10 || y > 600) {
            dy = -dy;
        }
        
        // 5. Copia o back buffer para o front buffer (exibe na tela)
        swap_buffers();
        
        // 6. Delay mínimo para a animação ser visível
        for (volatile int i = 0; i < 50000; i++);
    }
}