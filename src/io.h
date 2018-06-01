#ifndef IO_H
#define IO_H

#include "cpu.h"

#ifdef USE_SDL
	#define PIXEL_SCALE 8
#else
	#define PIXEL_SCALE 1
#endif

#define SCREEN_WIDTH 64 * PIXEL_SCALE
#define SCREEN_HEIGHT 32 * PIXEL_SCALE 
#define WINDOW_TITLE "Chip-8 Emulator"

#define BG_COLOR 0xFF
#define FG_COLOR 0x00

#define KEY_1 0x01
#define KEY_2 0x02
#define KEY_3 0x03
#define KEY_4 0x0c
#define KEY_Q 0x04
#define KEY_W 0x05
#define KEY_E 0x06
#define KEY_R 0x0d
#define KEY_A 0x07
#define KEY_S 0x08
#define KEY_D 0x09
#define KEY_FF 0x0e
#define KEY_Z 0x0a
#define KEY_X 0x00
#define KEY_C 0x0b
#define KEY_V 0x0f

uchar *keystates;

#ifdef USE_SDL
	#include <SDL2/SDL.h>
	
	uchar FULLSCREEN;
	SDL_Window* screen;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	SDL_Event e;
#elif USE_CURSES
	#define KEY_ESCAPE 27
#endif


uchar pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

extern uchar getEvent();
extern uchar getKey();
extern uchar initScreen();
extern uchar initAudio();
extern void drawPixel(uchar x, uchar y);
extern void clearScreen();
extern void flipScreen();
extern void quit(uchar exit_code);

#endif