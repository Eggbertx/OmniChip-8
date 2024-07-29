#include <graphics.h>

#include "io.h"
#include "chip8.h"

#define __EMBED_ROM__ 1

uchar getEvent(void) {
	return 0;
}

uchar getKey(void) {
	return 0xFF;
}

uchar initScreen(void) {
	clg();
	return 0;
}

schar isPressed(uchar key) {

	return 0xFF;
}

void delay(ushort milliseconds) {
	
}

uchar initAudio(void) {
	return 0;
}

void drawPixel(uchar x, uchar y) {
	fillb(x, y, 1, 1);
}

void clearScreen(void) {
	clg();
}

void flipScreen(void) {

}

void cleanup(void) {
	
}
