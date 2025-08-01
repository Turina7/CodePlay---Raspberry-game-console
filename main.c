#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "video.h"
#include "utils.h"

// USPi
#include <uspienv.h>
#include <uspi.h>
#include <uspios.h>

static volatile int g_key_pressed = 0;

// Função chamada quando uma tecla for pressionada
static void KeyPressedHandler(const char *pString) {
    (void)pString;
    g_key_pressed = 1;
}


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
    
    uint32_t brasil_matrix[16][16] = {
    {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00},
    {0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00},
    {0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00},
    {0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00},
    {0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00},
    {0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00},
    {0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF0000FF, 0xFF0000FF, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
    };

void main() {
    // Inicializar framebuffer
    init_framebuffer();
    for (volatile int i = 0; i < 10000000; i++);

    paint_orange_screen();
    for (volatile int i = 0; i < 10000000; i++);
    for (volatile int i = 0; i < 10000000; i++);

    // Inicializar ambiente USPi
    debug_blink(1);
    if (!USPiEnvInitialize()) {
        debug_blink(2);
        return;
    }

    debug_blink(3);
    if (!USPiInitialize()) {
        USPiEnvClose();
        debug_blink(4);
        return;
    }

    debug_blink(5);
    if (!USPiKeyboardAvailable()) {
        USPiEnvClose();
        debug_blink(6);
        return;
    }

    // Registrar callback de teclado
    USPiKeyboardRegisterKeyPressedHandler(KeyPressedHandler);

    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);

    while (1) {
        fill_screen_from_matrix((uint32_t *)brasil_matrix, 16, 16);
        for (volatile int i = 0; i < 10000000; i++);

        paint_blue_screen();
        for (volatile int i = 0; i < 10000000; i++);

        paint_orange_screen();
        for (volatile int i = 0; i < 10000000; i++);

        // Verificar se houve tecla pressionada
        if (g_key_pressed) {
            debug_blink(7);
            g_key_pressed = 0; // Zera a flag
        }
    }
}
