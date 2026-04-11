#include <conio.h>
#include <cbm.h>
#include <tgi.h>

#include "io.h"
#include "chip8.h"

unsigned char oldRepeat;
#define OFFSET_X 32
#define OFFSET_Y 32

uchar getEvent(void) {
	return 0;
}

uchar getKey(void) {
	return 0xFF;
}

uchar initScreen(void) {
	uchar maxY;
	tgi_install(tgi_static_stddrv);
	tgi_init();
	tgi_clear();

	oldRepeat = kbrepeat(KBREPEAT_ALL);
	maxY = tgi_getmaxy();
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
	ushort mX = (x << 2) + OFFSET_X;
	ushort mY = (y << 2) + OFFSET_Y;
	tgi_bar(mX, mY, mX+PIXEL_SCALE, mY+PIXEL_SCALE);
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