#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <curses.h>

#include "io.h"
#include "cpu.h"

#define PIXEL_CHAR "#"
struct winsize max;

uchar initScreen() {
	ioctl(0, TIOCGWINSZ, &max); // get console size to make sure the console is big enough
	if(max.ws_col < SCREEN_WIDTH || max.ws_row < SCREEN_HEIGHT) {
		printf("Error: console too small. Must be at least %d rows and %d columns.\n", SCREEN_HEIGHT, SCREEN_WIDTH);
		printf("Currently: %d rows and %d columns.\n", max.ws_row, max.ws_col);
		exit(2);
	}

	initscr();
	timeout(-1); // timeout() sets blocking or non blocking
	//keypad(stdscr, 1);
	nodelay(stdscr, 1);
	noecho();
	curs_set(0); // hide cursor
	int y = 1;
	int x = 0;
	// testing drawChar function
	for(int i=0;i<16;i++) {
		if(x*5+5 >= 113) {
			y += 6;
			x = 0;
		} 
		drawChar(i,1+x*5,y);
		flipScreen();

		x++;
	}
	return 0;
}

void flipScreen() {
	refresh();
}

void clearScreen() {

}

uchar initAudio() {
	return 0;
}

uchar getEvent() {
	int key = getch();
	switch(key) {
		case KEY_Q:
			quit(0);
		break;
			
	}
	return 0;
}

 void drawPixel(uchar x, uchar y) {
	 mvprintw(y, x, PIXEL_CHAR);
 }

void quit(uchar exit_code) {
	endwin();
	exit(exit_code);
}