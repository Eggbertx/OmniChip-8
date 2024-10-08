#include <stdarg.h>
#include <stdio.h>

#include "io.h"
#include "chip8.h"


uchar getEvent(void) {
	return 0;
}

uchar getKey(void) {
	return 0xFF;
}

uchar initScreen(void) {
	printf("initScreen()\n");
	return 0;
}

schar isPressed(uchar key) {
	printf("isPressed(%x)\n", key);
	return 0xFF;
}

void delay(ushort milliseconds) {
	printf("delay(%d)", milliseconds);
}

uchar initAudio(void) {
	printf("initAudio()\n");
	return 0;
}

void drawPixel(uchar x, uchar y) {
	printf("drawPixel(%d, %d)\n", x, y);
}

void clearScreen(void) {
	printf("clearScreen()\n");
}

void flipScreen(void) {
	printf("flipScreen()\n");
}

void cleanup(void) {

}

uchar clockCheck() {
	return 1;
}