#include <stdio.h>
#include <SDL.h>

#include "io.h"
#ifdef DEBUG_KEYS
#include "chip8.h"
#endif

uchar FULLSCREEN = 0;
SDL_Renderer *renderer;
SDL_Window *screen;
SDL_Texture *texture;
SDL_Surface *surface;

uchar initScreen(void) {
	SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_EVENTS);
	if(FULLSCREEN) {
		screen = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
	} else {
		screen = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	}
	renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
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

uchar getEvent(void) {
	SDL_Event e;
	SDL_PumpEvents();
	SDL_PollEvent(&e);
	switch(e.type) {
	case SDL_KEYDOWN:
		if(e.key.keysym.sym == SDLK_ESCAPE) {
			return EVENT_ESCAPE;
		}
	#ifdef DEBUG_KEYS
		if(e.key.keysym.sym == SDLK_F1) {
			togglePause();
		} else if(e.key.keysym.sym == SDLK_F2) {
			printf("Resetting\n");
			uchar preResetStatus = chip8.status;
			initChip8();
			initScreen();
			chip8.status = preResetStatus;
		} else if(e.key.keysym.sym == SDLK_F3) {
			stepIn();
		} else if(e.key.keysym.sym == SDLK_F4) {
			stepOut();
		}  else if(e.key.keysym.sym == SDLK_F5) {
			/* print current address and instruction */
			printf("0x%04X: 0x%04x - %s\n", chip8.PC, chip8.opcode, currentOpcode);
		} else if(e.key.keysym.sym == SDLK_F6) {
			printStatus();
		}
	#endif
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

uchar getKey(void) {
	const uchar* state = SDL_GetKeyboardState(NULL);
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
	return 0xFF;
}

schar isPressed(uchar key) {
	const uchar* state = SDL_GetKeyboardState(NULL);
	/* printf("Key : %d\n", key); */
	switch(key) {
		case 0x1:
			return state[SDL_SCANCODE_1];
		case 0x2:
			return state[SDL_SCANCODE_2];
		case 0x3:
			return state[SDL_SCANCODE_3];
		case 0xc:
			return state[SDL_SCANCODE_4];
		case 0x4:
			return state[SDL_SCANCODE_Q];
		case 0x5:
			return state[SDL_SCANCODE_W];
		case 0x6:
			return state[SDL_SCANCODE_E];
		case 0xd:
			return state[SDL_SCANCODE_R];
		case 0x7:
			return state[SDL_SCANCODE_A];
		case 0x8:
			return state[SDL_SCANCODE_S];
		case 0x9:
			return state[SDL_SCANCODE_D];
		case 0xe:
			return state[SDL_SCANCODE_F];
		case 0xa:
			return state[SDL_SCANCODE_Z];
		case 0x0:
			return state[SDL_SCANCODE_X];
		case 0xb:
			return state[SDL_SCANCODE_C];
		case 0xf:
			return state[SDL_SCANCODE_V];
		default:
			return 0;
	}
}

uchar initAudio(void) {
	SDL_AudioSpec audioSpec;
	audioSpec.freq = BEEP_FREQUENCY;
	audioSpec.samples = 2048;
	audioSpec.channels = 1;
	audioSpec.format = AUDIO_S16SYS;

	return 0;
}

void playSound(void) {

}

void playAudio(void* data, void* stream, int length) {
	
}

void drawPixel(uchar x, uchar y) {
	/* printf("Drawing pixel at %d,%d\n", x, y); */
	SDL_RenderDrawPoint(renderer, x, y);
}

void flipScreen(void) {
	SDL_RenderPresent(renderer);
}

void clearScreen(void) {
	SDL_FillRect(surface, NULL, 0x000000);
}

void cleanup(void) {
	if(surface != NULL && surface->refcount > 0)
		SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
	SDL_Quit();
}
