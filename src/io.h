#ifndef IO_H
#define IO_H

#include "chip8.h"

#ifdef SDL_IO
	#define PIXEL_SCALE 8
#elif defined(__C64__)
	#define PIXEL_SCALE 5
#else
	#define PIXEL_SCALE 1
#endif

#define PIXEL_CHAR "#"

#define BEEP_AMPLITUDE 28000
#define BEEP_FREQUENCY 44100

#define SCREEN_WIDTH 64 * PIXEL_SCALE
#define SCREEN_HEIGHT 32 * PIXEL_SCALE 
#define WINDOW_TITLE "OmniChip-8"

#define BG_COLOR 0xFF
#define FG_COLOR 0x00

#define EVENT_NULL 0
#define EVENT_ESCAPE 1
#define EVENT_WINDOWCLOSE 2

#ifdef CURSES_IO
	#define KEY_ESCAPE 27
#endif

uchar getEvent(void);

uchar getKey(void);

schar isPressed(uchar key);

uchar initScreen(void);

void delay(ushort milliseconds);

uchar initAudio(void);

void playSound(void);

void drawPixel(uchar x, uchar y);

void clearScreen(void);

void flipScreen(void);

void cleanup(void);

void addrInfo(struct Chip8* chip8, char* format, ...);

#endif
