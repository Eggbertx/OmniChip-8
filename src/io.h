#include <SDL2/SDL.h>

#define PIXEL_SCALE 8
#define SCREEN_WIDTH 64 * PIXEL_SCALE
#define SCREEN_HEIGHT 32 * PIXEL_SCALE 
#define WINDOW_TITLE "Chip-8 Emulator"

#define BG_COLOR 0xFFFFFF
#define FG_COLOR 0x000000

typedef unsigned char bool;
typedef unsigned char byte;

bool FULLSCREEN = 0;
unsigned char *keystates;

#ifdef USE_SDL
	SDL_Window* screen = NULL;
	SDL_Texture* texture;
	byte pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
	SDL_Renderer* renderer;
#endif

byte getKey();
bool initScreen();
bool initAudio();
void drawPixel(byte x, byte y);
void clearScreen();
void flipScreen();