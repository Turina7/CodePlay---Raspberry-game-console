#ifndef VIDEO_H
#define VIDEO_H
#include <stdint.h>
#include "utils.h"

/*
 * Sistema de Vídeo - Console Baremetal
 * 
 * API principal para renderização com double buffering.
 * Todas as funções de desenho operam no back buffer.
 * Use swap_buffers() para atualizar a tela.
 */

// Inicialização do sistema de vídeo
int init_framebuffer();

// Funções de renderização (desenham no back buffer)
void paint_blue_screen();
void paint_orange_screen(); 
void fill_screen_from_matrix(uint32_t* matrix, int width, int height);

// Sistema de double buffering
void swap_buffers(); // Copia back buffer -> front buffer (atualiza tela)

#endif