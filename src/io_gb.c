#include <gb/gb.h>
#include <gb/drawing.h>

#include "io.h"
#include "chip8.h"


uchar getEvent(void) {
	return 0;
}

uchar getKey(void) {
	return 0xFF;
}

uchar initScreen(void) {

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
	plot(x,y, WHITE, SOLID);
}

void clearScreen(void) {

}

void flipScreen(void) {
	wait_vbl_done();
}

void cleanup(void) {

}

void addrInfo(struct Chip8* chip8, char* format, ...) {

}
