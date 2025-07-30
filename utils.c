//Funções de utilidades não implementas pelo gcc
#include <stdint.h>
#include <stdlib.h>

// Endereços base para Raspberry Pi 2
#define PERIPHERAL_BASE 0x3F000000
#define MAILBOX_BASE (PERIPHERAL_BASE + 0xB880)

// GPIO para debug LED (GPIO 47 - LED de atividade)
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)
#define GPFSEL4 (*(volatile uint32_t*)(GPIO_BASE + 0x10))
#define GPSET1  (*(volatile uint32_t*)(GPIO_BASE + 0x20))
#define GPCLR1  (*(volatile uint32_t*)(GPIO_BASE + 0x2C))

// Função para configurar GPIO para debug
void init_debug_led() {
    // Configurar GPIO 47 como saída (LED de atividade)
    uint32_t sel = GPFSEL4;
    sel &= ~(7 << 21);  // Limpar bits 21-23
    sel |= (1 << 21);   // Configurar como saída
    GPFSEL4 = sel;
}

// Piscar LED para debug
void debug_blink(int count) {
    for (int i = 0; i < count; i++) {
        GPSET1 = (1 << 15); // GPIO 47 = bit 15 em GPSET1 (47-32)
        for (volatile int j = 0; j < 200000; j++); // Delay
        GPCLR1 = (1 << 15); // Apagar LED
        for (volatile int j = 0; j < 200000; j++); // Delay
    }
}

// Implementação da função memcpy
void* memcpy(void* dest, const void* src, unsigned long n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (unsigned long i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}
