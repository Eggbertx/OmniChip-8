#include "io.h"

extern SDL_Window* screen;
extern SDL_Texture* texture;
SDL_Renderer* renderer;
	
bool initScreen() {
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
	SDL_Delay(2000);
}

bool initAudio() {
	
}

void drawPixel(unsigned char x, unsigned char y) {
	SDL_RenderDrawPoint(renderer, x, y);
}

void flipScreen() {
	SDL_RenderPresent(renderer);
	clearScreen();
}

void clearScreen() {
	SDL_RenderClear(renderer);
}
