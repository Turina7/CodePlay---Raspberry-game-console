/*
 * Sistema de Vídeo - Console Baremetal Raspberry Pi 2
 * 
 * Implementa double buffering para renderização fluída:
 * - Back buffer: renderização off-screen (RAM)
 * - Front buffer: framebuffer visível (GPU)
 * - Swap instantâneo via memcpy otimizado
 * 
 * Funcionalidades:
 * - Inicialização via mailbox property interface
 * - Renderização de cores sólidas
 * - Scaling de matrizes pixel-perfect
 * - Sistema de debug via LED
 */

#include <stdint.h>
#include <stdlib.h>
#include "utils.h"

// Endereços base para Raspberry Pi 2
#define PERIPHERAL_BASE 0x3F000000
#define MAILBOX_BASE (PERIPHERAL_BASE + 0xB880)

// Registradores do mailbox
#define MAILBOX_READ   (*(volatile uint32_t*)(MAILBOX_BASE + 0x00))
#define MAILBOX_STATUS (*(volatile uint32_t*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE  (*(volatile uint32_t*)(MAILBOX_BASE + 0x20))

// Flags do mailbox
#define MAILBOX_EMPTY (1 << 30)
#define MAILBOX_FULL  (1 << 31)

// Canais do mailbox
#define MAILBOX_CHANNEL_FRAMEBUFFER 1

// Estrutura de mensagem do framebuffer para mailbox property interface
struct __attribute__((aligned(16))) mbox_fb_msg {
    uint32_t size;           // Tamanho total da mensagem
    uint32_t code;           // Código de requisição/resposta
    
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
    
    .tag_phys_size = 0x48003,      // Set physical display size
    .tag_phys_size_len = 8,
    .tag_phys_size_code = 0,
    .phys_width = 1024,
    .phys_height = 768,
    
    .tag_virt_size = 0x48004,      // Set virtual display size
    .tag_virt_size_len = 8,
    .tag_virt_size_code = 0,
    .virt_width = 1024,
    .virt_height = 768,
    
    .tag_depth = 0x48005,          // Set depth
    .tag_depth_len = 4,
    .tag_depth_code = 0,
    .depth = 32,
    
    .tag_allocate = 0x40001,       // Allocate framebuffer
    .tag_allocate_len = 8,
    .tag_allocate_code = 0,
    .fb_addr = 0,
    .fb_size = 0,
    
    .tag_pitch = 0x40008,          // Get pitch
    .tag_pitch_len = 4,
    .tag_pitch_code = 0,
    .pitch = 0,
    
    .end_tag = 0
};

// Back buffer para double buffering (alocação estática)
// 1024x768x4 bytes = 3,145,728 bytes (~3MB)
static uint32_t back_buffer[1024 * 768] __attribute__((aligned(16)));

// Função para escrever no mailbox com timeout
int mailbox_write(uint32_t channel, uint32_t data) {
    uint32_t timeout = 1000000;
    
    // Esperar até o mailbox não estar cheio
    while ((MAILBOX_STATUS & MAILBOX_FULL) && timeout--) {
        if (timeout == 0) return 0; // Timeout
    }
    
    // Escrever dados + canal
    MAILBOX_WRITE = (data & 0xFFFFFFF0) | (channel & 0xF);
    return 1;
}

// Função para ler do mailbox com timeout
uint32_t mailbox_read(uint32_t channel) {
    uint32_t data;
    uint32_t timeout = 1000000;
    
    do {
        // Esperar até ter dados
        while ((MAILBOX_STATUS & MAILBOX_EMPTY) && timeout--) {
            if (timeout == 0) return 0; // Timeout
        }
        
        // Ler dados
        data = MAILBOX_READ;
    } while ((data & 0xF) != channel);
    
    return data & 0xFFFFFFF0;
}

// Inicializar framebuffer usando property mailbox
int init_framebuffer() {
    // Converter endereço para bus address (adicionar 0x40000000 para coherent access)
    uint32_t fb_addr = (uint32_t)&fb_msg + 0x40000000;
    
    // Debug: 1 piscada = iniciando framebuffer
    debug_blink(1);
    
    // Enviar requisição via mailbox (canal 8 = property channel)
    if (!mailbox_write(8, fb_addr)) {
        debug_blink(10); // 10 piscadas = erro no write
        return 0;
    }
    
    // Aguardar resposta
    uint32_t result = mailbox_read(8);
    if (result == 0) {
        debug_blink(9); // 9 piscadas = erro no read
        return 0;
    }
    
    // Verificar se foi bem-sucedido
    if (fb_msg.code != 0x80000000) {
        debug_blink(8); // 8 piscadas = código de resposta inválido
        return 0;
    }
    
    // Verificar se o framebuffer foi alocado
    if (fb_msg.fb_addr == 0 || fb_msg.fb_size == 0) {
        debug_blink(7); // 7 piscadas = framebuffer não alocado
        return 0;
    }
    
    // Back buffer já está alocado estaticamente
    // Verificar se as dimensões coincidem
    uint32_t expected_pixels = (fb_msg.pitch / 4) * fb_msg.virt_height;
    if (expected_pixels > (1024 * 768)) {
        debug_blink(11); // 11 piscadas = back buffer muito pequeno
        return 0;
    }
    
    // Debug: 2 piscadas = framebuffer inicializado com sucesso
    debug_blink(2);
    
    return 1;
}

//Funções de pintura de tela - Para testes de hdmi
// Pintar tela de azul no back buffer
void paint_blue_screen() {
    
    // Debug: 3 piscadas = começando a pintar
    debug_blink(3);
    
    // Cor azul em formato ARGB (32 bits)
    uint32_t blue_color = 0xFF0000FF; // Alpha=255, Red=0, Green=0, Blue=255
    
    // Calcular número total de pixels baseado no pitch real
    uint32_t pixels_per_line = fb_msg.pitch / 4;
    uint32_t total_pixels = pixels_per_line * fb_msg.virt_height;
    
    // Pintar todos os pixels de azul no back buffer
    for (uint32_t i = 0; i < total_pixels; i++) {
        back_buffer[i] = blue_color;
    }
    
    // Debug: 4 piscadas = tela pintada com sucesso
    debug_blink(4);
}

void paint_orange_screen() {
    
    // Debug: 3 piscadas = começando a pintar
    debug_blink(3);
    
    // Cor laranja em formato ARGB (32 bits)
    uint32_t orange_color = 0xFFFFA500; // Alpha=255, Red=255, Green=165, Blue=0
    
    // Calcular número total de pixels baseado no pitch real
    uint32_t pixels_per_line = fb_msg.pitch / 4;
    uint32_t total_pixels = pixels_per_line * fb_msg.virt_height;
    
    // Pintar todos os pixels de laranja no back buffer
    for (uint32_t i = 0; i < total_pixels; i++) {
        back_buffer[i] = orange_color;
    }
    
    // Debug: 4 piscadas = tela pintada com sucesso
    debug_blink(4);
}

// Função para copiar back buffer para front buffer (swap buffers)
void swap_buffers() {
    if (fb_msg.fb_addr == 0) {
        debug_blink(6);
        return;
    }
    
    // Converter endereço do framebuffer
    uint32_t* framebuffer = (uint32_t*)(fb_msg.fb_addr & 0x3FFFFFFF);
    
    // Copiar todo o back buffer para o front buffer
    uint32_t total_pixels = (fb_msg.pitch / 4) * fb_msg.virt_height;
    
    // Copy ultra-otimizado: 8 pixels (32 bytes) por iteração
    uint32_t* src = back_buffer;
    uint32_t* dst = framebuffer;
    uint32_t remaining = total_pixels;
    
    // Loop desenrolado para máxima performance
    while (remaining >= 8) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        src += 8;
        dst += 8;
        remaining -= 8;
    }
    
    // Copiar pixels restantes
    while (remaining > 0) {
        *dst++ = *src++;
        remaining--;
    }
}

//Função de preenchimento de tela baseada em matriz qualquer
void fill_screen_from_matrix(uint32_t* matrix, int original_width, int original_height) {

    debug_blink(1); // Debug: função iniciada

    int screen_width = fb_msg.pitch / 4;
    int screen_height = fb_msg.virt_height;

    if (screen_height % original_height != 0 || screen_width % original_width != 0) {
        debug_blink(5);
        return;
    }

    int fator_linha = screen_height / original_height;
    int fator_coluna = screen_width / original_width;

    for (int screen_y = 0; screen_y < screen_height; screen_y++) {
        for (int screen_x = 0; screen_x < screen_width; screen_x++) {
            int original_y = screen_y / fator_linha;
            int original_x = screen_x / fator_coluna;
            
            // Acesso como array 1D: matrix[y * width + x] - renderizar no back buffer
            back_buffer[screen_y * screen_width + screen_x] = 
                matrix[original_y * original_width + original_x];
        }
    }

    debug_blink(4);
}