#ifndef VIDEO_H
#define VIDEO_H
#include <stdint.h>
#include "utils.h"

int init_framebuffer();
void paint_blue_screen();
void paint_orange_screen();
void fill_screen_from_matrix(uint32_t* matrix, int width, int height);

#endif