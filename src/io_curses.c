#include <stdarg.h>
#include <stdio.h>

#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <curses.h>
#include <termios.h>
#include <unistd.h>

#include "io.h"
#include "chip8.h"

struct winsize terminalSize;
WINDOW* debugWindow;
struct termios old_tio;
struct termios new_tio;
int debug_line = 0;
int valid_size = 0;
int nextTick = 0;

uchar initScreen(void) {
	ioctl(0, TIOCGWINSZ, &terminalSize); /* get console size to make sure the console is big enough */
	valid_size = (terminalSize.ws_col >= SCREEN_WIDTH && terminalSize.ws_row >= SCREEN_HEIGHT);
	if(!valid_size) {
		printf("Error: console too small. Must be at least %d rows and %d columns.\n", SCREEN_HEIGHT, SCREEN_WIDTH);
		printf("Currently: %d rows and %d columns.\n", terminalSize.ws_row, terminalSize.ws_col);
		return 1;
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

	if(terminalSize.ws_row > SCREEN_HEIGHT+10) {
		/* area for drawing debug info if the screen is large enough
		debugWindow = newwin(SCREEN_HEIGHT-2, 28, 1, SCREEN_WIDTH+1); */
		debugWindow = newwin(10, SCREEN_WIDTH, 33, 0);
	}

	return 0;
}

void flipScreen(void) {
	refresh();
	wrefresh(debugWindow);
}

void clearScreen(void) {
	clear();
}

uchar initAudio(void) {
	return 0;
}

void playSound(void) {
	/* this probably won't be implemented with curses mode */
}

uchar getEvent(void) {
	int ch = getch();
	uchar event = 0;
	switch(ch) {
		case 27:
			event = EVENT_ESCAPE;
			break;
		default:
			/* ungetch(ch); */
			break;
	}
	return event;
}

schar isPressed(uchar key) {
	return 0;
}

uchar getKey(void) {
	int ch = 0;
	int key = 0xFF;

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
	return key;
}

void delay(ushort milliseconds) {
	napms(milliseconds);
}

void drawPixel(uchar x, uchar y) {
	mvprintw(y, x, PIXEL_CHAR);
}

void cleanup(void) {
	if(!valid_size)
		/* the terminal width appears to be 0 columns, safe to assume no cleanup is needed */
		return; 
	tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
	endwin();
}

uchar clockCheck() {
	int now;
	struct timeval time;
	gettimeofday(&time, NULL);
	now = time.tv_usec / 1000;
	if(now >= 1000) {
		now -= 1000;
	}

	if(now >= nextTick) {
		nextTick = now + CLOCK_DELAY;
		if(nextTick >= 1000) {
			nextTick -= 1000;
		}
		return 1;
	}
	return 0;
}