#include<stdio.h>
#include <stdlib.h>
#include"video.h"
// Função principal
void main() {
    
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);
    
    //Inicialização do display grafico--------------------------------------

    // Inicializar framebuffer
    init_framebuffer();

    //Loop principal do jogo
    //uint32_t red_color      = 0xFFFF0000; // Alpha=255, Red=255, Green=0,   Blue=0
    //uint32_t green_color    = 0xFF00FF00; // Alpha=255, Red=0,   Green=255, Blue=0
    //uint32_t blue_color     = 0xFF0000FF; // Alpha=255, Red=0,   Green=0,   Blue=255
    //uint32_t yellow_color   = 0xFFFFFF00; // Alpha=255, Red=255, Green=255, Blue=0
    //uint32_t cyan_color     = 0xFF00FFFF; // Alpha=255, Red=0,   Green=255, Blue=255
    //uint32_t magenta_color  = 0xFFFF00FF; // Alpha=255, Red=255, Green=0,   Blue=255
    //uint32_t white_color    = 0xFFFFFFFF; // Alpha=255, Red=255, Green=255, Blue=255
    //uint32_t black_color    = 0xFF000000; // Alpha=255, Red=0,   Green=0,   Blue=0
    //uint32_t orange_color   = 0xFFFFA500; // Alpha=255, Red=255, Green=165, Blue=0
    //uint32_t gray_color     = 0xFF808080; // Alpha=255, Red=128, Green=128, Blue=128
    //uint32_t purple_color   = 0xFF800080; // Alpha=255, Red=128, Green=0,   Blue=128
    
    uint32_t test_matrix[2][2] = {
    {0xFFFF0000, 0xFF00FF00},
    {0xFF0000FF, 0xFFFFFFFF}
    };

    while(1){
        //paint_blue_screen();
        for (volatile int i = 0; i < 10000000; i++); // Delay para manter a tela azul
        //paint_orange_screen();
        for (volatile int i = 0; i < 10000000; i++); // Delay para manter a tela laranja
        fill_screen_from_matrix((uint32_t*)test_matrix, 2, 2);
        for (volatile int i = 0; i < 10000000; i++);
    }
}