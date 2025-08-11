#include "video.h"
// USPi
#include <uspienv.h>
#include <uspi.h>
#include <uspios.h>
#include <uspienv/util.h>

static const char FromSample[] = "sample";

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

int g_key_pressed = 0;
char g_last_key = 0;

// Função chamada quando uma tecla for pressionada
static void KeyPressedHandler(const char *pString)
{
    if (!pString || !pString[0]) {
        return;
    }
    
    g_last_key = pString[0];
    g_key_pressed = 1;
}

void handle_keyboard_input(char key) {
    // Normaliza maiúsculas para minúsculas
    if (key >= 'A' && key <= 'Z') {
        key = key + 32;
    }
    
    switch (key) {
        case '1':
            write_on_screen(texto);
            break;
        case '2':
            write_on_screen(texto2);
            break;
        case '3':
            write_on_screen(texto3);
            break;
        default:
            break;
    }
}

void init_keyboard() {
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
}
