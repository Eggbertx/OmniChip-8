#include <stdarg.h>
#include <stdio.h>

#include <stdlib.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <termios.h>
#include <unistd.h>

#include "io.h"
#include "cpu.h"

#define PIXEL_CHAR "#"
struct winsize max;
WINDOW* debugWindow;
struct termios old_tio;
struct termios new_tio;
int debug_line = 0;

uchar initScreen(void) {
	ioctl(0, TIOCGWINSZ, &max); /* get console size to make sure the console is big enough */
	if(max.ws_col < SCREEN_WIDTH || max.ws_row < SCREEN_HEIGHT) {
		printf("Error: console too small. Must be at least %d rows and %d columns.\n", SCREEN_HEIGHT, SCREEN_WIDTH);
		printf("Currently: %d rows and %d columns.\n", max.ws_row, max.ws_col);
		exit(2);
	}

	initscr();
	/* keypad(stdscr, 1); */
	nodelay(stdscr, 1);
	
	tcgetattr(STDIN_FILENO,&old_tio);
	new_tio=old_tio;
	new_tio.c_lflag &=(~ICANON & ~ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);



	noecho();
	/* setvbuf(stdin, NULL, _IONBF, 0);  */
	curs_set(0); /* hide cursor */

	if(max.ws_col > SCREEN_WIDTH+30) {
		/* area for drawing debug info if the screen is large enough
		debugWindow = newwin(SCREEN_HEIGHT-2, 28, 1, SCREEN_WIDTH+1); */
		debugWindow = newwin(64, 64, 0, 65);
	}

	return 0;
}

void flipScreen(void) {
	refresh();
}

void clearScreen(void) {
	clear();
}

uchar initAudio(void) {
	return 0;
}

uchar getEvent(void) {
	int ch = getch();
	uchar event = 0;
	switch(ch) {
		case 27:
			event = EVENT_ESCAPE;
			cleanup();
			exit(0);
			break;
		default:
			/* ungetch(ch); */
			break;
	}
	return event;
}

uchar getKey(void) {
	int ch = -1;
	int key = -1;

	ch = getch();
	switch(ch) {
		case '1':
			key = 0x01;
			break;
		case '2':
			key = 0x02;
			break;
		case '3':
			key = 0x03;
			break;
		case '4':
			key = 0x0C;
			break;
		case 'q':
			key = 0x04;
			break;
		case 'w':
			key = 0x05;
			break;
		case 'e':
			key = 0x06;
			break;
		case 'r':
			key = 0x0D;
			break;
		case 'a':
			key = 0x07;
			break;
		case 's':
			key = 0x08;
			break;
		case 'd':
			key = 0x09;
			break;
		case 'f':
			key = 0x0E;
			break;
		case 'z':
			key = 0x0A;
			break;
		case 'x':
			key = 0x00;
			break;
		case 'c':
			key = 0x0B;
			break;
		case 'v':
			key = 0x0F;
			break;
		default:
			ungetch(ch);
			break;
	}
	addrInfo("Key: %d\n", key);
	return key;
}

void delay(ushort milliseconds) {
	usleep(milliseconds * 1000);
}

void drawPixel(uchar x, uchar y) {
	mvprintw(y, x, PIXEL_CHAR);
}

void addrInfo(char* format, ...) {
	va_list args;

	va_start(args, format);
	mvwprintw(debugWindow, debug_line, 0, format);
	debug_line++;

	va_end(args);
	wrefresh(debugWindow);
}

void cleanup(void) {
	tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
	endwin();
}
