#include <tgi.h>

#include "io.h"
#include "chip8.h"


uchar getEvent(void) {
	return 0;
}

uchar getKey(void) {
	return 0xFF;
}

uchar initScreen(void) {
	tgi_install(tgi_static_stddrv);
	tgi_init();
	tgi_clear();
	return 0;
}

schar isPressed(uchar key) {
	/* int key = 0xFF;

	key = kbhit();
	if(!key) return 0xFF;
	key = cgetc(); */
	return 0xFF;
}

void delay(ushort milliseconds) {
	
}

uchar initAudio(void) {
	return 0;
}

void drawPixel(uchar x, uchar y) {
	/* tgi_setpixel (x, y); */
	tgi_bar(x*PIXEL_SCALE, y*PIXEL_SCALE, x*PIXEL_SCALE+PIXEL_SCALE, y*PIXEL_SCALE+PIXEL_SCALE);
}

void clearScreen(void) {
	tgi_clear();
}

void flipScreen(void) {

}

void cleanup(void) {
	tgi_unload();
	tgi_uninstall();
}

void addrInfo(struct Chip8* chip8, char* format, ...) {

}