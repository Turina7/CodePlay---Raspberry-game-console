#include "video.h"
#include "utils.h"
#include "teclado.h"

void main() {
    init_keyboard();
    init_framebuffer();
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);

    while (1) {
            // Modo interativo
            if (g_key_pressed) {
                handle_keyboard_input(g_last_key);
                g_key_pressed = 0;
            }
            
            // Pequeno delay para não sobrecarregar a CPU
            for (volatile int i = 0; i < 100000; i++);
        }
    return;
}
