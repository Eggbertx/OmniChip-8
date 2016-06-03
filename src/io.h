#ifndef IO_H
#define IO_H

#include <SDL2/SDL.h>

#define PIXEL_SCALE 8
#define SCREEN_WIDTH 64 * PIXEL_SCALE
#define SCREEN_HEIGHT 32 * PIXEL_SCALE 
#define WINDOW_TITLE "Chip-8 Emulator"

#define BG_COLOR 0xFF
#define FG_COLOR 0x00


unsigned char *keystates;

#ifdef USE_SDL
	unsigned char FULLSCREEN;
	SDL_Window* screen;
	SDL_Texture* texture;
	unsigned char pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
	SDL_Renderer* renderer;
#endif

extern unsigned char getKey();
extern unsigned char initScreen();
extern unsigned char initAudio();
extern void drawPixel(unsigned char x, unsigned char y);
extern void clearScreen();
extern void flipScreen();

#endif