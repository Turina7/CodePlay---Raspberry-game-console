// ==================== video.c ====================
#include <stdint.h>
#include <stddef.h>
#include "video.h"
#include "utils.h"

// =============================================================================
// CONSTANTES DO HARDWARE
// =============================================================================
/*
 *Esse arquivo é parte do projeto VideoPlayer
 *Aqui queremos mapear a saida de vídeo, que é basicamente um framebuffer,
 * em uma matriz de pixels, facil para o usuário manipular.
 */

// Endereços base para Raspberry Pi 2
#define PERIPHERAL_BASE 0x3F000000
#define MAILBOX_BASE (PERIPHERAL_BASE + 0xB880)

// Registradores do mailbox
#define MAILBOX_READ (*(volatile uint32_t *)(MAILBOX_BASE + 0x00))
#define MAILBOX_STATUS (*(volatile uint32_t *)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE (*(volatile uint32_t *)(MAILBOX_BASE + 0x20))

// Flags do mailbox
#define MAILBOX_EMPTY (1 << 30)
#define MAILBOX_FULL (1 << 31)

// Canais do mailbox
#define MAILBOX_CHANNEL_FRAMEBUFFER 1

// =============================================================================
// ESTRUTURA DO FRAMEBUFFER
// =============================================================================

// Estrutura de mensagem do framebuffer para mailbox property interface
struct __attribute__((aligned(16))) mbox_fb_msg
{
    uint32_t size; // Tamanho total da mensagem
    uint32_t code; // Código de requisição/resposta

    // Tag para configurar resolução física
    uint32_t tag_phys_size;
    uint32_t tag_phys_size_len;
    uint32_t tag_phys_size_code;
    uint32_t phys_width;
    uint32_t phys_height;

    // Tag para configurar resolução virtual
    uint32_t tag_virt_size;
    uint32_t tag_virt_size_len;
    uint32_t tag_virt_size_code;
    uint32_t virt_width;
    uint32_t virt_height;

    // Tag para configurar profundidade de cor
    uint32_t tag_depth;
    uint32_t tag_depth_len;
    uint32_t tag_depth_code;
    uint32_t depth;

    // Tag para alocar framebuffer
    uint32_t tag_allocate;
    uint32_t tag_allocate_len;
    uint32_t tag_allocate_code;
    uint32_t fb_addr;
    uint32_t fb_size;

    // Tag para obter pitch
    uint32_t tag_pitch;
    uint32_t tag_pitch_len;
    uint32_t tag_pitch_code;
    uint32_t pitch;

    uint32_t end_tag;
};

static struct mbox_fb_msg fb_msg __attribute__((aligned(16))) = {
    .size = sizeof(struct mbox_fb_msg),
    .code = 0,

    .tag_phys_size = 0x48003, // Set physical display size
    .tag_phys_size_len = 8,
    .tag_phys_size_code = 0,
    .phys_width = 1024,
    .phys_height = 768,

    .tag_virt_size = 0x48004, // Set virtual display size
    .tag_virt_size_len = 8,
    .tag_virt_size_code = 0,
    .virt_width = 1024,
    .virt_height = 768,

    .tag_depth = 0x48005, // Set depth
    .tag_depth_len = 4,
    .tag_depth_code = 0,
    .depth = 32,

    .tag_allocate = 0x40001, // Allocate framebuffer
    .tag_allocate_len = 8,
    .tag_allocate_code = 0,
    .fb_addr = 0,
    .fb_size = 0,

    .tag_pitch = 0x40008, // Get pitch
    .tag_pitch_len = 4,
    .tag_pitch_code = 0,
    .pitch = 0,

    .end_tag = 0};

// =============================================================================
// CORES
// =============================================================================

static const uint32_t white_color = 0xFFFFFFFF;
static const uint32_t black_color = 0xFF000000;

// =============================================================================
// SISTEMA DE FONTES 5x5
// =============================================================================

static uint32_t font_A[5][5] = {
    {black_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color}};

static uint32_t font_B[5][5] = {
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, black_color}};

static uint32_t font_C[5][5] = {
    {black_color, white_color, white_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {black_color, white_color, white_color, white_color, white_color}};

static uint32_t font_D[5][5] = {
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, black_color}};

static uint32_t font_E[5][5] = {
    {white_color, white_color, white_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, white_color}};

static uint32_t font_F[5][5] = {
    {white_color, white_color, white_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, black_color, black_color, black_color, black_color}};

static uint32_t font_G[5][5] = {
    {black_color, white_color, white_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, black_color, white_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, white_color}};

static uint32_t font_H[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color}};

static uint32_t font_I[5][5] = {
    {white_color, white_color, white_color, white_color, white_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, white_color}};

static uint32_t font_J[5][5] = {
    {black_color, black_color, black_color, black_color, white_color},
    {black_color, black_color, black_color, black_color, white_color},
    {black_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, black_color}};

static uint32_t font_K[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, white_color, black_color},
    {white_color, white_color, white_color, black_color, black_color},
    {white_color, black_color, black_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color}};

static uint32_t font_L[5][5] = {
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, white_color}};

static uint32_t font_M[5][5] = {
    {white_color, white_color, black_color, white_color, white_color},
    {white_color, white_color, black_color, white_color, white_color},
    {white_color, black_color, white_color, black_color, white_color},
    {white_color, black_color, white_color, black_color, white_color},
    {white_color, black_color, white_color, black_color, white_color}};

static uint32_t font_N[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, black_color, black_color, white_color},
    {white_color, black_color, white_color, black_color, white_color},
    {white_color, black_color, black_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, white_color}};

static uint32_t font_O[5][5] = {
    {black_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, black_color}};

static uint32_t font_P[5][5] = {
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, black_color, black_color, black_color, black_color}};

static uint32_t font_Q[5][5] = {
    {black_color, white_color, white_color, black_color, black_color},
    {white_color, black_color, black_color, white_color, black_color},
    {white_color, black_color, black_color, white_color, black_color},
    {white_color, black_color, black_color, white_color, black_color},
    {black_color, white_color, white_color, black_color, white_color}};

static uint32_t font_R[5][5] = {
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color}};

static uint32_t font_S[5][5] = {
    {black_color, white_color, white_color, white_color, white_color},
    {white_color, black_color, black_color, black_color, black_color},
    {black_color, white_color, white_color, white_color, white_color},
    {black_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, black_color}};

static uint32_t font_T[5][5] = {
    {white_color, white_color, white_color, white_color, white_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color}};

static uint32_t font_U[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, black_color}};

static uint32_t font_V[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, black_color, white_color, black_color},
    {black_color, black_color, white_color, black_color, black_color}};

static uint32_t font_W[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, white_color, black_color, white_color},
    {white_color, black_color, white_color, black_color, white_color},
    {black_color, white_color, black_color, white_color, black_color}};

static uint32_t font_X[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, black_color, white_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, white_color, black_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color}};

static uint32_t font_Y[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, black_color, white_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color}};

static uint32_t font_Z[5][5] = {
    {white_color, white_color, white_color, white_color, white_color},
    {black_color, black_color, black_color, white_color, white_color},
    {black_color, black_color, white_color, black_color, black_color},
    {white_color, white_color, black_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, white_color}};

static uint32_t font_0[5][5] = {
    {black_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, black_color}};

static uint32_t font_1[5][5] = {
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, white_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, white_color, white_color, white_color, black_color}};

static uint32_t font_2[5][5] = {
    {black_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, black_color, white_color, white_color, black_color},
    {black_color, white_color, black_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, white_color}};

static uint32_t font_3[5][5] = {
    {white_color, white_color, white_color, white_color, black_color},
    {black_color, black_color, black_color, black_color, white_color},
    {black_color, black_color, white_color, white_color, black_color},
    {black_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, black_color}};

static uint32_t font_4[5][5] = {
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, white_color},
    {black_color, black_color, black_color, black_color, white_color},
    {black_color, black_color, black_color, black_color, white_color}};

static uint32_t font_5[5][5] = {
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, black_color},
    {black_color, black_color, black_color, black_color, white_color},
    {white_color, white_color, white_color, white_color, black_color}};

static uint32_t font_6[5][5] = {
    {black_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, black_color},
    {white_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, black_color}};

static uint32_t font_7[5][5] = {
    {white_color, white_color, white_color, white_color, white_color},
    {black_color, black_color, black_color, black_color, white_color},
    {black_color, black_color, black_color, white_color, black_color},
    {black_color, black_color, white_color, black_color, black_color},
    {black_color, black_color, white_color, black_color, black_color}};

static uint32_t font_8[5][5] = {
    {black_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, black_color}};

static uint32_t font_9[5][5] = {
    {black_color, white_color, white_color, white_color, black_color},
    {white_color, black_color, black_color, black_color, white_color},
    {black_color, white_color, white_color, white_color, white_color},
    {black_color, black_color, black_color, black_color, white_color},
    {black_color, black_color, black_color, black_color, white_color}};

// =============================================================================
// FUNÇÕES DE COMUNICAÇÃO COM MAILBOX
// =============================================================================

static int mailbox_write(uint32_t channel, uint32_t data)
{
    uint32_t timeout = 1000000;

    // Esperar até o mailbox não estar cheio
    while ((MAILBOX_STATUS & MAILBOX_FULL) && timeout--)
    {
        if (timeout == 0)
            return 0; // Timeout
    }

    // Escrever dados + canal
    MAILBOX_WRITE = (data & 0xFFFFFFF0) | (channel & 0xF);
    return 1;
}

// Função para ler do mailbox com timeout
static uint32_t mailbox_read(uint32_t channel)
{
    uint32_t data;
    uint32_t timeout = 1000000;

    do
    {
        // Esperar até ter dados
        while ((MAILBOX_STATUS & MAILBOX_EMPTY) && timeout--)
        {
            if (timeout == 0)
                return 0; // Timeout
        }

        // Ler dados
        data = MAILBOX_READ;
    } while ((data & 0xF) != channel);

    return data & 0xFFFFFFF0;
}

static char tolower(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c + ('a' - 'A');
    else
        return c;
}

// Função para obter fonte de um caractere
static void get_font(uint32_t (**font)[5], char c)
{
    switch (tolower(c))
    {
    case 'a':
        *font = font_A;
        break;
    case 'b':
        *font = font_B;
        break;
    case 'c':
        *font = font_C;
        break;
    case 'd':
        *font = font_D;
        break;
    case 'e':
        *font = font_E;
        break;
    case 'f':
        *font = font_F;
        break;
    case 'g':
        *font = font_G;
        break;
    case 'h':
        *font = font_H;
        break;
    case 'i':
        *font = font_I;
        break;
    case 'j':
        *font = font_J;
        break;
    case 'k':
        *font = font_K;
        break;
    case 'l':
        *font = font_L;
        break;
    case 'm':
        *font = font_M;
        break;
    case 'n':
        *font = font_N;
        break;
    case 'o':
        *font = font_O;
        break;
    case 'p':
        *font = font_P;
        break;
    case 'q':
        *font = font_Q;
        break;
    case 'r':
        *font = font_R;
        break;
    case 's':
        *font = font_S;
        break;
    case 't':
        *font = font_T;
        break;
    case 'u':
        *font = font_U;
        break;
    case 'v':
        *font = font_V;
        break;
    case 'w':
        *font = font_W;
        break;
    case 'x':
        *font = font_X;
        break;
    case 'y':
        *font = font_Y;
        break;
    case 'z':
        *font = font_Z;
        break;
    case '0':
        *font = font_0;
        break;
    case '1':
        *font = font_1;
        break;
    case '2':
        *font = font_2;
        break;
    case '3':
        *font = font_3;
        break;
    case '4':
        *font = font_4;
        break;
    case '5':
        *font = font_5;
        break;
    case '6':
        *font = font_6;
        break;
    case '7':
        *font = font_7;
        break;
    case '8':
        *font = font_8;
        break;
    case '9':
        *font = font_9;
        break;
    default:
        *font = NULL;
        break;
    }
}

// =============================================================================
// INICIALIZAÇÃO DO FRAMEBUFFER
// =============================================================================

int init_framebuffer()
{
    // Converter endereço para bus address (adicionar 0x40000000 para coherent access)
    uint32_t fb_addr = (uint32_t)&fb_msg + 0x40000000;

    // Debug: 1 piscada = iniciando framebuffer
    // debug_blink>>(1);

    // Enviar requisição via mailbox (canal 8 = property channel)
    if (!mailbox_write(8, fb_addr))
    {
        // debug_blink(10); // 10 piscadas = erro no write
        return 0;
    }

    // Aguardar resposta
    uint32_t result = mailbox_read(8);
    if (result == 0)
    {
        // debug_blink(9); // 9 piscadas = erro no read
        return 0;
    }

    // Verificar se foi bem-sucedido
    if (fb_msg.code != 0x80000000)
    {
        // debug_blink(8); // 8 piscadas = código de resposta inválido
        return 0;
    }

    // Verificar se o framebuffer foi alocado
    if (fb_msg.fb_addr == 0 || fb_msg.fb_size == 0)
    {
        return 0;
    }

    return 1;
}

// =============================================================================
// FUNÇÕES DE DESENHO
// =============================================================================

void fill_screen_from_matrix(uint32_t *matrix, int original_width, int original_height)
{
    if (fb_msg.fb_addr == 0)
    {
        return;
    }

    int screen_width = fb_msg.pitch / 4;
    int screen_height = fb_msg.virt_height;

    // Removida verificação rígida de divisibilidade para permitir escalamento

    int fator_linha = screen_height / original_height;
    int fator_coluna = screen_width / original_width;
    uint32_t *framebuffer = (uint32_t *)(uintptr_t)(fb_msg.fb_addr & 0x3FFFFFFF);

    for (int screen_y = 0; screen_y < screen_height; screen_y++)
    {
        for (int screen_x = 0; screen_x < screen_width; screen_x++)
        {
            int original_y = screen_y / fator_linha;
            int original_x = screen_x / fator_coluna;
            // Acesso usando aritmética de ponteiros para simular array 2D
            framebuffer[screen_y * screen_width + screen_x] =
                matrix[original_y * original_width + original_x];
        }
    }
}

// =============================================================================
// RENDERIZAÇÃO DE TEXTO
// =============================================================================

void write_on_screen(char text_matrix[SCREEN_HEIGHT][SCREEN_WIDTH])
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
            get_font(&font, c);

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
