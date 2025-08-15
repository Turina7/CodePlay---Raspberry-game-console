#include "video.h"
// USPi
#include <uspienv.h>
#include <uspi.h>
#include <uspios.h>
#include <uspienv/util.h>

static const char FromSample[] = "sample";

int g_key_pressed = 0;
char g_last_key = 0;
int g_key_pressed_p1 = 0;
char g_last_key_p1 = 0;
int g_key_pressed_p2 = 0;
char g_last_key_p2 = 0;

// Função chamada quando uma tecla for pressionada
static void KeyPressedHandler(const char *pString)
{
    if (!pString || !pString[0]) {
        return;
    }

    // Separar P1 (WASD) e P2 (arrows)
    if (pString[0] == '\x1b' && pString[1] == '[') {
        char code = pString[2];
        if (code == 'A' || code == 'B' || code == 'C' || code == 'D') {
            // Player 2 usa setas
            g_last_key_p2 = (code == 'A') ? 'w' : (code == 'B') ? 's' : (code == 'C') ? 'd' : 'a';
            g_key_pressed_p2 = 1;
        }
    } else {
        // Player 1 usa WASD (normaliza)
        char c = pString[0];
        if (c >= 'A' && c <= 'Z') c = (char)(c + ('a' - 'A'));
        if (c == 'w' || c == 'a' || c == 's' || c == 'd' || c == 'm' || (c >= '1' && c <= '9')) {
            g_last_key_p1 = c;
            g_key_pressed_p1 = 1;
        }
        // Compat: manter flags antigas para o menu existente
        g_last_key = c;
        g_key_pressed = 1;
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
