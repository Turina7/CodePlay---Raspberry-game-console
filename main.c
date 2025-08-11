#include "video.h"
#include "utils.h"

// USPi
#include <uspienv.h>
#include <uspi.h>
#include <uspios.h>
#include <uspienv/util.h>

static const char FromSample[] = "sample";
static int g_key_pressed = 0;
static char g_last_key = 0;
static int g_program_mode = 0;

char texto[SCREEN_HEIGHT][SCREEN_WIDTH] = {
    "          ",
    " BEM VINDO",
    "  AO LAB  ",
    " DE BRUNO ",
    "  Basseto ",
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

char texto3[SCREEN_HEIGHT][SCREEN_WIDTH] = {
    "          ",
    "          ",
    "   BRUNO  ",
    "   PUSSY  ",
    "          ",
    "          ",
    "          "
};

// Função chamada quando uma tecla for pressionada
static void KeyPressedHandler(const char *pString)
{
    // Verificação básica se a string é válida
    if (!pString || !pString[0]) {
        LogWrite(FromSample, LOG_ERROR, "Invalid key string received");
        return;
    }
    
    g_last_key = pString[0];
    g_key_pressed = 1;
    
    // ESC ou 'q' para alternar entre modos
    if (g_last_key == 27 || g_last_key == 'q' || g_last_key == 'Q') {
        g_program_mode = !g_program_mode;
        if (g_program_mode == 0) {
            LogWrite(FromSample, LOG_NOTICE, "Switched to ANIMATION mode");
        } else {
            LogWrite(FromSample, LOG_NOTICE, "Switched to INTERACTIVE mode");
        }
        g_key_pressed = 0; // Consome a tecla aqui para não processar duas vezes
    }
}

void handle_keyboard_input(char key) {
    // Normaliza maiúsculas para minúsculas
    if (key >= 'A' && key <= 'Z') {
        key = key + 32;
    }
    
    switch (key) {
        case '1':
            write_on_screen(texto);
            LogWrite(FromSample, LOG_NOTICE, "Blue screen selected");
            break;
        case '2':
            write_on_screen(texto2);
            LogWrite(FromSample, LOG_NOTICE, "Orange screen selected");
            break;
        case 'r':
            // Volta ao modo animação
            g_program_mode = 0;
            LogWrite(FromSample, LOG_NOTICE, "Returning to animation mode");
            break;
        case 'h':
            LogWrite(FromSample, LOG_NOTICE, "Commands: 1=blue, 2=orange, 3=brazil, r=animate, q=toggle");
            break;
        case 13: // Enter
            LogWrite(FromSample, LOG_NOTICE, "Enter key pressed");
            break;
        case 32: // Space
            LogWrite(FromSample, LOG_NOTICE, "Space key pressed");
            break;
        case 8:  // Backspace
            LogWrite(FromSample, LOG_NOTICE, "Backspace key pressed");
            break;
        default:
            LogWrite(FromSample, LOG_NOTICE, "erro");
            break;
    }
}

void main() {
    if (!USPiEnvInitialize()) {
        return;
    }
    
    if (!USPiInitialize()) {
        LogWrite(FromSample, LOG_ERROR, "Cannot initialize USPi");
        USPiEnvClose();
        return;
    }
    
    if (!USPiKeyboardAvailable()) {
        LogWrite(FromSample, LOG_ERROR, "Keyboard not found");
        USPiEnvClose();
        return;
    }

    USPiKeyboardRegisterKeyPressedHandler(KeyPressedHandler);
    LogWrite(FromSample, LOG_NOTICE, "=== RPI2 BARE METAL KEYBOARD TEST ===");
    LogWrite(FromSample, LOG_NOTICE, "Commands:");
    LogWrite(FromSample, LOG_NOTICE, "  q/ESC = Toggle between animation/interactive");
    LogWrite(FromSample, LOG_NOTICE, "  1 = Blue screen");
    LogWrite(FromSample, LOG_NOTICE, "  2 = Orange screen");
    LogWrite(FromSample, LOG_NOTICE, "  3 = Brazil flag");
    LogWrite(FromSample, LOG_NOTICE, "  r = Return to animation");
    LogWrite(FromSample, LOG_NOTICE, "  h = Show help");
    LogWrite(FromSample, LOG_NOTICE, "Starting in ANIMATION mode...");

    init_framebuffer();
    // Delay para estabilizar o sistema
    for (volatile int i = 0; i < 1000000; i++);

    while (1) {
        if (g_program_mode == 0) {
            // Modo animação
            write_on_screen(texto);
            
            // Verifica input durante a animação (delay menor para resposta mais rápida)
            for (volatile int i = 0; i < 5000000; i++) {
                if (g_key_pressed && (g_last_key == 27 || g_last_key == 'q' || g_last_key == 'Q')) {
                    break; // Sai do delay se tecla de alternância foi pressionada
                }
            }
            
            if (g_program_mode == 0) { // Ainda no modo animação
                write_on_screen(texto2);
                for (volatile int i = 0; i < 5000000; i++) {
                    if (g_key_pressed && (g_last_key == 27 || g_last_key == 'q' || g_last_key == 'Q')) {
                        break;
                    }
                }
            }
            
            if (g_program_mode == 0) { // Ainda no modo animação
                write_on_screen(texto3);
                for (volatile int i = 0; i < 5000000; i++) {
                    if (g_key_pressed && (g_last_key == 27 || g_last_key == 'q' || g_last_key == 'Q')) {
                        break;
                    }
                }
            }
        } else {
            // Modo interativo
            if (g_key_pressed) {
                // Só processa se não foi uma tecla de alternância de modo
                if (g_last_key != 27 && g_last_key != 'q' && g_last_key != 'Q') {
                    handle_keyboard_input(g_last_key);
                }
                g_key_pressed = 0;
            }
            
            // Pequeno delay para não sobrecarregar a CPU
            for (volatile int i = 0; i < 100000; i++);
        }
    }
    return;
}
