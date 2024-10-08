#include <conio.h>
#include <cbm.h>
#include <tgi.h>

#include "io.h"
#include "chip8.h"

unsigned char oldRepeat;

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

	oldRepeat = kbrepeat(KBREPEAT_ALL);
	return 0;
}

schar isPressed(uchar key) {
	int pressed = 0xFF;

	if(!kbhit()) return pressed;
	pressed = cgetc();
	return pressed;
}

void delay(ushort milliseconds) {
	
}

uchar initAudio(void) {
	return 0;
}

void _OC8_FASTCALL drawPixel(uchar x, uchar y) {
	tgi_bar(x*PIXEL_SCALE, y*PIXEL_SCALE, x*PIXEL_SCALE+PIXEL_SCALE, y*PIXEL_SCALE+PIXEL_SCALE);
}

void _OC8_FASTCALL clearScreen(void) {
	tgi_clear();
}

void _OC8_FASTCALL flipScreen(void) {
}

void cleanup(void) {
	tgi_unload();
	tgi_uninstall();
	kbrepeat(oldRepeat);
}

uchar clockCheck() {
	return 1;
}