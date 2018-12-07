#ifndef IO_H
#define IO_H

#include "cpu.h"

#ifdef SDL_IO
	#define PIXEL_SCALE 8
#else
	#define PIXEL_SCALE 1
#endif

#define BEEP_AMPLITUDE 28000
#define BEEP_FREQUENCY 44100

#define SCREEN_WIDTH 64 * PIXEL_SCALE
#define SCREEN_HEIGHT 32 * PIXEL_SCALE 
#define WINDOW_TITLE "Chip-8 Emulator"

#define BG_COLOR 0xFF
#define FG_COLOR 0x00

#define EVENT_NULL 0
#define EVENT_ESCAPE 1
#define EVENT_WINDOWCLOSE 2

uchar *keystates;

#ifdef CURSES_IO
	#define KEY_ESCAPE 27
#endif


uchar pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

uchar getEvent();

schar getKey();

uchar initScreen();

void delay(ushort milliseconds);

uchar initAudio();

void drawPixel(uchar x, uchar y);

void clearScreen();

void flipScreen();

void cleanup();

void addrInfo(char* format, ...);

#endif