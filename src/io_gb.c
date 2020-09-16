#include <gb/gb.h>
#include <gb/drawing.h>

#include "io.h"
#include "chip8.h"


#define OFFSET_X GRAPHICS_WIDTH/2 - SCREEN_WIDTH/2 - 1
#define OFFSET_Y GRAPHICS_HEIGHT/2 - SCREEN_HEIGHT/2 - 1


uchar getEvent(void) {
	return 0;
}

uchar getKey(void) {
	return 0xFF;
}

uchar initScreen(void) {
	color(BLACK, WHITE, M_FILL);
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
	int cY;
	for(cY = 0; cY < PIXEL_SCALE; cY++) {
		line(
			x*PIXEL_SCALE + OFFSET_X,
			y*PIXEL_SCALE+cY + OFFSET_Y,
			x*PIXEL_SCALE+PIXEL_SCALE + OFFSET_X,
			y*PIXEL_SCALE+cY + OFFSET_Y
		);
	}
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
