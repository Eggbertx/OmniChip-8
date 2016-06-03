#include "io.h"
#include "chip8.h"
#include <stdio.h>

unsigned char FULLSCREEN = 0;
	
unsigned char initScreen() {
	SDL_Init(SDL_INIT_EVERYTHING);
	if(FULLSCREEN) {
		screen = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
	} else {
		screen = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	}
	renderer = SDL_CreateRenderer(screen, -1, 0);
	SDL_SetRenderDrawColor(renderer, 255,255,255,0);
	SDL_RenderSetScale(renderer, PIXEL_SCALE, PIXEL_SCALE);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
	int y = 1;
	int x = 0;
	for(int i=0;i<16;i++) {
		if(x*5+5 >= 64) {
			y += 6;
			x = 0;
		} 
		drawChar(i,1+x*5,y);
		flipScreen();
		SDL_Delay(100);
		x++;
	}
	
	SDL_Event e;
	while(SDL_WaitEvent(&e)) {
		if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return 1;
		else if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) return 1;
	}
	return 1;
}

unsigned char initAudio() {
	return 0;
}

void drawPixel(unsigned char x, unsigned char y) {
	SDL_RenderDrawPoint(renderer, x, y);
}

void flipScreen() {
	SDL_RenderPresent(renderer);
	//clearScreen();
}

void clearScreen() {
	SDL_RenderClear(renderer);
}
