#ifndef TECLADO_H
#define TECLADO_H

extern int g_key_pressed;
extern char g_last_key;
void init_keyboard();
void handle_keyboard_input(char key);

#endif
