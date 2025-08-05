#include <stdint.h>

#define TIMER_BASE   0x3F003000  // Pi 3

#define TIMER_CS        (TIMER_BASE + 0x00)
#define TIMER_CLO       (TIMER_BASE + 0x04)
#define TIMER_CHI       (TIMER_BASE + 0x08)
#define TIMER_C0        (TIMER_BASE + 0x0C)
#define TIMER_C1        (TIMER_BASE + 0x10)
#define TIMER_C2        (TIMER_BASE + 0x14)
#define TIMER_C3        (TIMER_BASE + 0x18)

// Memory-mapped I/O access
#define mmio_write(reg, data) (*(volatile uint32_t*)(reg) = (data))
#define mmio_read(reg) (*(volatile uint32_t*)(reg))

void timer_init(void) {
    // Clear timer compare status bits
    mmio_write(TIMER_CS, 0x0F);
    
    // The system timer is usually already running after boot
    // No additional initialization typically needed for basic timing
}

// Utility functions you might need
uint32_t timer_get_ticks(void) {
    return mmio_read(TIMER_CLO);
}

void delay_us(uint32_t microseconds) {
    uint32_t start = timer_get_ticks();
    while ((timer_get_ticks() - start) < microseconds) {
        // Wait
    }
}

void delay_ms(uint32_t milliseconds) {
    delay_us(milliseconds * 1000);
}