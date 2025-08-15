#ifndef TECLADO_H
#define TECLADO_H

extern int g_key_pressed;
extern char g_last_key;
// Multiplayer minimal: player 1 (WASD) and player 2 (arrows)
extern int g_key_pressed_p1;
extern char g_last_key_p1;
extern int g_key_pressed_p2;
extern char g_last_key_p2;
void init_keyboard();

#endif
