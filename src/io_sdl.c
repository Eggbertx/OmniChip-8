#include <stdio.h>
#include <SDL2/SDL.h>

#include "io.h"
#include "cpu.h"


uchar FULLSCREEN = 0;
SDL_Renderer *renderer;
SDL_Window *screen;
SDL_Texture *texture;
	
uchar initScreen() {
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

	/*
	// testing drawChar function
	for(int i=0;i<16;i++) {
		if(x*5+5 >= 64) {
			y += 6;
			x = 0;
		} 
		drawChar(i,1+x*5,y);
		flipScreen();
		SDL_Delay(1);
		x++;
	}
	*/
	flipScreen();
	return 0;
}

uchar getEvent() {
	SDL_Event e;
	SDL_PollEvent(&e);
	switch(e.type) {
		case SDL_KEYDOWN:
			if(e.key.keysym.sym == SDLK_ESCAPE) {
				printf("Received Escape key, exiting normally.\n");
				printf("cpu.memory bytes:\n");
				printBytes(cpu.memory, 4096, "cpu-memory");
				return 1;
			}
		break;

		case SDL_WINDOWEVENT:
			if(e.window.event == SDL_WINDOWEVENT_CLOSE)
				return 1;
		break;

		default:
			return 0;
		break;
	}
	return 0;
}

uchar initAudio() {
	return 0;
}

void drawPixel(uchar x, uchar y) {
	SDL_RenderDrawPoint(renderer, x, y);
	flipScreen();
}

void flipScreen() {
	SDL_RenderPresent(renderer);
	//clearScreen();
}

void clearScreen() {
	SDL_RenderClear(renderer);
}
