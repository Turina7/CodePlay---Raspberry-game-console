//Funções de utilidades não implementas pelo gcc
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

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

// Implementação otimizada da função memcpy para transfers de 32-bit
void* memcpy(void* dest, const void* src, unsigned long n) {
    // Para transfers alinhados de 32-bit, usar cópia word por word
    if (n >= 16 && ((uintptr_t)dest % 4 == 0) && ((uintptr_t)src % 4 == 0) && (n % 4 == 0)) {
        uint32_t* d32 = (uint32_t*)dest;
        const uint32_t* s32 = (const uint32_t*)src;
        unsigned long words = n / 4;
        
        // Copiar 4 words por vez quando possível
        while (words >= 4) {
            d32[0] = s32[0];
            d32[1] = s32[1];
            d32[2] = s32[2];
            d32[3] = s32[3];
            d32 += 4;
            s32 += 4;
            words -= 4;
        }
        
        // Copiar words restantes
        while (words > 0) {
            *d32++ = *s32++;
            words--;
        }
        return dest;
    }
    
    // Fallback para cópia byte por byte
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (unsigned long i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}
