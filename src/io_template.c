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

}

void clearScreen(void) {

}

void flipScreen(void) {

}

void cleanup(void) {

}

void addrInfo(struct Chip8* chip8, char* format, ...) {
	va_list args;

	printf("0x%04x: %04x, ", chip8->PC-2, chip8->opcode);
	va_start(args, format);
	vprintf(format, args);
	va_end(args);

	printf("\n");
}
