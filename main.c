// ==================== main.c ====================
#include "video.h"

// Função principal
int main()
{
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);

    // Inicialização do display gráfico
    init_framebuffer();

    char texto[SCREEN_HEIGHT][SCREEN_WIDTH] = {
        "          ",
        " BEM VINDO",
        "  AO LAB  ",
        " DE BRUNO ",
        "   PUSSY  ",
        "          ",
        "          "
    };

    char texto2[SCREEN_HEIGHT][SCREEN_WIDTH] = {
        "          ",
        "          ",
        "          ",
        " CODEPLAY ",
        "          ",
        "          ",
        "          "
    };

    while (1)
    {
        write_on_screen(texto);
        for (volatile int i = 0; i < 10000000; i++);

        write_on_screen(texto2);
        for (volatile int i = 0; i < 10000000; i++);
    }
    return 0;
}
