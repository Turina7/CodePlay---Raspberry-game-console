#include "video.h"
#include "utils.h"
#include "timer.h"

// USPi
#include <uspienv.h>
#include <uspi.h>
#include <uspios.h>
#include <uspienv/util.h>

static volatile int g_key_pressed = 0;

// Função chamada quando uma tecla for pressionada
static void KeyPressedHandler (const char *pString)
{
	ScreenDeviceWrite (USPiEnvGetScreen (), pString, my_strlen (pString));
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

static const char FromSample[] = "sample";

void main() {
	if (!USPiEnvInitialize ())
	{
		return;
	}
	
	if (!USPiInitialize ())
	{
		LogWrite (FromSample, LOG_ERROR, "Cannot initialize USPi");

		USPiEnvClose ();

		return;
	}
	
	if (!USPiKeyboardAvailable ())
	{
		LogWrite (FromSample, LOG_ERROR, "Keyboard not found");

		USPiEnvClose ();

		return;
	}

	USPiKeyboardRegisterKeyPressedHandler (KeyPressedHandler);

	LogWrite (FromSample, LOG_NOTICE, "Just type something!");

	// just wait and turn the rotor
	for (unsigned nCount = 0; 1; nCount++)
	{
		USPiKeyboardUpdateLEDs ();

		ScreenDeviceRotor (USPiEnvGetScreen (), 0, nCount);
	}

	return;
    // Inicializar framebuffer
    /*init_framebuffer();
    for (volatile int i = 0; i < 10000000; i++);

    paint_orange_screen();
    for (volatile int i = 0; i < 10000000; i++);
    for (volatile int i = 0; i < 10000000; i++);

    timer_init();

    delay_ms(100);

    __asm__ volatile("cpsie i");

    delay_ms(100);

    // Inicializar ambiente USPi
    debug_blink(1);
    if (!USPiEnvInitialize()) {
        debug_blink(2);
        return;
    }

    debug_blink(3);
    if (!USPiInitialize()) {
        LogWrite ("teste", LOG_ERROR, "Cannot initialize USPi");
        USPiEnvClose();
        debug_blink(4);
        return;
    }

    debug_blink(5);
    if (!USPiKeyboardAvailable()) {
        LogWrite ("teste", LOG_ERROR, "Keyboard not found");
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
    }*/
}
