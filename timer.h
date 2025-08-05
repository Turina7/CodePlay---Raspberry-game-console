#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

void timer_init();
uint32_t timer_get_ticks(void);
void delay_us(uint32_t microseconds);
void delay_ms(uint32_t milliseconds);

#endif