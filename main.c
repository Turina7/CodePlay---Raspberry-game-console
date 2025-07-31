#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "utils.h"

// --- Configurações do Jogo da Cobrinha ---

// Dimensões do campo de jogo (matriz lógica)
#define GAME_WIDTH 32
#define GAME_HEIGHT 24
#define MAX_SNAKE_LENGTH 100

// Cores (formato ARGB)
#define BG_COLOR     0xFF000000 // Preto
#define SNAKE_COLOR  0xFF00FF00 // Verde
#define FOOD_COLOR   0xFFFF0000 // Vermelho
#define GAMEOVER_COLOR 0xFF808080 // Cinza

// Estrutura para um ponto (coordenadas x, y)
typedef struct {
    int x;
    int y;
} Point;

// --- Variáveis Globais do Jogo ---

// Matriz que representa o campo de jogo
uint32_t game_matrix[GAME_HEIGHT][GAME_WIDTH];

// A cobra
Point snake_body[MAX_SNAKE_LENGTH];
int snake_length;

// A comida
Point food;

// Direção da cobra (dx, dy)
int dx, dy;

// --- Funções do Jogo ---

// Gera uma nova posição para a comida de forma pseudo-aleatória
void generate_food() {
    // Usa uma fórmula simples para não depender de rand() completo
    food.x = (food.x * 7 + 13) % GAME_WIDTH;
    food.y = (food.y * 5 + 11) % GAME_HEIGHT;

    // Garante que a comida não apareça em cima da cobra
    for (int i = 0; i < snake_length; i++) {
        if (food.x == snake_body[i].x && food.y == snake_body[i].y) {
            generate_food(); // Tenta novamente
            return;
        }
    }
}

// Inicializa o estado do jogo
void init_game() {
    // Começa movendo para a direita
    dx = 1;
    dy = 0;

    // Posição inicial da cobra
    snake_length = 3;
    snake_body[0] = (Point){.x = GAME_WIDTH / 2, .y = GAME_HEIGHT / 2};
    snake_body[1] = (Point){.x = GAME_WIDTH / 2 - 1, .y = GAME_HEIGHT / 2};
    snake_body[2] = (Point){.x = GAME_WIDTH / 2 - 2, .y = GAME_HEIGHT / 2};

    // Posição inicial da comida
    food = (Point){.x = GAME_WIDTH / 4, .y = GAME_HEIGHT / 4};
    generate_food();
}

// Atualiza o estado do jogo a cada quadro
void update_game() {
    // Calcula a nova posição da cabeça
    Point new_head = {
        .x = snake_body[0].x + dx,
        .y = snake_body[0].y + dy
    };

    // --- Lógica de IA Simples (Auto-Play) ---
    // Se bater na parede, vira para uma direção válida
    if (new_head.x < 0 || new_head.x >= GAME_WIDTH || new_head.y < 0 || new_head.y >= GAME_HEIGHT) {
        if (dx != 0) { // Estava se movendo na horizontal
            dx = 0;
            dy = (snake_body[0].y < GAME_HEIGHT / 2) ? 1 : -1; // Move para baixo ou para cima
        } else { // Estava se movendo na vertical
            dy = 0;
            dx = (snake_body[0].x < GAME_WIDTH / 2) ? 1 : -1; // Move para direita ou para esquerda
        }
        // Recalcula a cabeça com a nova direção
        new_head = (Point){.x = snake_body[0].x + dx, .y = snake_body[0].y + dy};
    }

    // Verifica colisão com o próprio corpo
    for (int i = 1; i < snake_length; i++) {
        if (new_head.x == snake_body[i].x && new_head.y == snake_body[i].y) {
            // Fim de jogo: Pinta a cobra de cinza e para
            for(int k=0; k < snake_length; k++){
                 game_matrix[snake_body[k].y][snake_body[k].x] = GAMEOVER_COLOR;
            }
            while(1); // Trava o jogo
        }
    }

    // Move o corpo da cobra
    for (int i = snake_length - 1; i > 0; i--) {
        snake_body[i] = snake_body[i - 1];
    }
    snake_body[0] = new_head;

    // Verifica se comeu a comida
    if (new_head.x == food.x && new_head.y == food.y) {
        if (snake_length < MAX_SNAKE_LENGTH) {
            snake_length++;
        }
        generate_food();
    }
}

// Desenha o estado atual do jogo na matriz
void draw_game() {
    // 1. Limpa a tela (pinta o fundo)
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            game_matrix[y][x] = BG_COLOR;
        }
    }

    // 2. Desenha a comida
    game_matrix[food.y][food.x] = FOOD_COLOR;

    // 3. Desenha a cobra
    for (int i = 0; i < snake_length; i++) {
        game_matrix[snake_body[i].y][snake_body[i].x] = SNAKE_COLOR;
    }
}


/*
 * Console Baremetal - Raspberry Pi 2
 * 
 * Simulação do jogo da cobrinha para testar
 * o sistema de framebuffer com atualizações recorrentes.
 */
void main() {
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);
    
    // Inicialização do display gráfico
    init_framebuffer();

    // Inicializa as variáveis do jogo
    init_game();

    // Loop principal do jogo
    while(1) {
        // 1. Atualiza a lógica do jogo (movimento, colisões)
        update_game();

        // 2. Desenha o estado atual na matriz do jogo
        draw_game();

        // 3. Renderiza a matriz do jogo no back buffer (com scaling)
        fill_screen_from_matrix((uint32_t*)game_matrix, GAME_WIDTH, GAME_HEIGHT);
        
        // 4. Copia o back buffer para o front buffer (tela)
        swap_buffers();
        
        // 5. Delay para controlar a velocidade do jogo
        for (volatile int i = 0; i < 500000; i++);
    }
}
