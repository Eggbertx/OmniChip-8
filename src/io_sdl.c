#include <stdio.h>
#include <SDL.h>

#include "io.h"
#include "cpu.h"


uchar FULLSCREEN = 0;
SDL_Renderer *renderer;
SDL_Window *screen;
SDL_Texture *texture;
SDL_Surface *surface;
	
uchar initScreen() {
	SDL_Init(SDL_INIT_EVERYTHING);
	if(FULLSCREEN) {
		screen = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
	} else {
		screen = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	}
	renderer = SDL_CreateRenderer(screen, -1, 0);
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderSetScale(renderer, PIXEL_SCALE, PIXEL_SCALE);
	surface = SDL_GetWindowSurface(screen);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

	flipScreen();
	return 0;
}

void delay(ushort milliseconds) {
	SDL_Delay(milliseconds);
}

uchar getEvent() {
	SDL_Event e;
	SDL_PollEvent(&e);
	switch(e.type) {
		case SDL_KEYDOWN:
			if(e.key.keysym.sym == SDLK_ESCAPE) {
				return EVENT_ESCAPE;
			}
		break;

		case SDL_WINDOWEVENT:
			if(e.window.event == SDL_WINDOWEVENT_CLOSE) {
				return EVENT_WINDOWCLOSE;
			}
		break;

		default:
			return 0;
		break;
	}
	return 0;
}

schar getKey() {
	int numKeys = 0;
	const uchar* state = SDL_GetKeyboardState(&numKeys);
	if(numKeys == 0) return -1;
	if(state[SDL_SCANCODE_1]) return 0x01;
	if(state[SDL_SCANCODE_2]) return 0x02;
	if(state[SDL_SCANCODE_3]) return 0x03;
	if(state[SDL_SCANCODE_4]) return 0x0c;
	if(state[SDL_SCANCODE_Q]) return 0x04;
	if(state[SDL_SCANCODE_W]) return 0x05;
	if(state[SDL_SCANCODE_E]) return 0x06;
	if(state[SDL_SCANCODE_R]) return 0x0d;
	if(state[SDL_SCANCODE_A]) return 0x07;
	if(state[SDL_SCANCODE_S]) return 0x08;
	if(state[SDL_SCANCODE_D]) return 0x09;
	if(state[SDL_SCANCODE_F]) return 0x0e;
	if(state[SDL_SCANCODE_Z]) return 0x0a;
	if(state[SDL_SCANCODE_X]) return 0x00;
	if(state[SDL_SCANCODE_C]) return 0x0b;
	if(state[SDL_SCANCODE_V]) return 0x0f;
	return -1;
}

uchar initAudio() {
	SDL_AudioSpec audioSpec;
	audioSpec.freq = BEEP_FREQUENCY;
	audioSpec.samples = 2048;
	audioSpec.channels = 1;
	audioSpec.format = AUDIO_S16SYS;

	return 0;
}

void playAudio(void* data, void* stream, int length) {
	
}

void drawPixel(uchar x, uchar y) {
	SDL_RenderDrawPoint(renderer, x, y);
}

void flipScreen() {
	SDL_RenderPresent(renderer);
}

void clearScreen() {
	SDL_FillRect(surface, NULL, 0x000000);
}

void cleanup() {
	SDL_FreeSurface(surface);
	SDL_Quit();
}

void addrInfo(char* format, ...) {
	va_list args;

	printf("0x%04x: %04x, ", cpu.PC-2, cpu.opcode);
	va_start(args, format);
	vprintf(format, args);
	va_end(args);

	printf("\n");
}