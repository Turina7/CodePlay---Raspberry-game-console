#include "video.h"
#include "utils.h"

// --- Configurações da Animação ---

// Dimensões da matriz lógica que representa a tela
#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 24

// Cores (formato ARGB)
#define BG_COLOR    0xFF000000 // Preto
#define BLOCK_COLOR 0xFF00FFFF // Ciano

// Matriz lógica para o desenho
uint32_t screen_matrix[SCREEN_HEIGHT][SCREEN_WIDTH];

/*
 * Console Baremetal - Raspberry Pi 2
 * 
 * Animação simples de um bloco para testar o framebuffer
 * com atualizações recorrentes e de forma mais segura.
 */
void main() {
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);
    
    // Inicialização do display gráfico
    init_framebuffer();

    // Posição inicial do bloco
    int x = 0;
    int y = 0;

    // Direção inicial do movimento
    int dx = 1;
    int dy = 1;

    // Loop principal
    while(1) {
        // 1. Limpar a matriz com a cor de fundo
        for (int r = 0; r < SCREEN_HEIGHT; r++) {
            for (int c = 0; c < SCREEN_WIDTH; c++) {
                screen_matrix[r][c] = BG_COLOR;
            }
        }

        // 2. Desenhar o bloco na sua posição atual
        screen_matrix[y][x] = BLOCK_COLOR;

        // 3. Atualizar a posição do bloco para o próximo quadro
        x += dx;
        y += dy;

        // 4. Fazer o bloco quicar nas bordas da tela
        if (x <= 0 || x >= SCREEN_WIDTH - 1) {
            dx = -dx; // Inverte a direção horizontal
        }
        if (y <= 0 || y >= SCREEN_HEIGHT - 1) {
            dy = -dy; // Inverte a direção vertical
        }

        // 5. Renderizar a matriz no back buffer (com scaling)
        fill_screen_from_matrix((uint32_t*)screen_matrix, SCREEN_WIDTH, SCREEN_HEIGHT);
        
        // 6. Copiar o back buffer para o front buffer (exibir na tela)
        swap_buffers();
        
        // 7. Delay para controlar a velocidade da animação
        for (volatile int i = 0; i < 500000; i++);
    }
}
