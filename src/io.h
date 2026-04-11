#ifndef IO_H
#define IO_H

#include "chip8.h"

#ifdef SDL_IO
	#define PIXEL_SCALE 8
#elif defined(__C64__)
	#define PIXEL_SCALE 5
#elif defined(GB_IO)
	#define PIXEL_SCALE 2
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


/* if set, causes the screen to flash before and after execution to see how long execution takes compared to drawing */
#ifdef DEBUG_DRAWS
	#if defined(C64_IO)
		#define DEBUG_DRAW1() *(uchar*)0xD020 = 2
		#define DEBUG_DRAW2() *(uchar*)0xD020 = 6
	#elif defined(GB_IO)
		#include <gb/gb.h>
		#define DEBUG_DRAW1() BGP_REG = 0xE4
		#define DEBUG_DRAW2() BGP_REG = 0x1B
	#endif
#else
	/* dummy/no-op */
	#define DEBUG_DRAW1()
	#define DEBUG_DRAW2()
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

uchar clockCheck();

#endif
