#include <stdio.h>
/* TODO: replace printf/fprintf with conio.h functions for cc65 */
#include <stdlib.h>
#ifdef EMSCRIPTEN_IO
#include <emscripten.h>
#endif
#include "chip8.h"
#include "io.h"

int main(int argc, char *argv[]) {
	int exitStatus;
	struct Chip8* chip8 = NULL;
	chip8 = (struct Chip8*)malloc(sizeof(struct Chip8));
	if(argc != 2) {
		printf("usage: %s rompath\n", argv[0]);
		return 0;
	}

	if(initScreen() > 0) {
		printf("ERROR: Unable to init screen.\n");
		return 0;
	}

	if(initAudio() > 0) {
		printf("ERROR: Unable to init audio.\n");
		return 0;
	}

	if (initChip8(chip8, argv[1]) > 0) {
		printf("ERROR: Something went wrong while loading %s\n", argv[1]);
		return 0;
	}

#if defined(SDL_IO) /* || defined(CURSES_IO) */
	chip8->printDebug = 1;
#else
	chip8->printDebug = 0;
#endif

#ifdef EMSCRIPTEN_IO
	emscripten_set_main_loop_arg(runCycles, chip8, 60, 0);
#else
	runCycles(chip8);
#endif
	cleanup();
	exitStatus = chip8->status == STATUS_ERROR;
	free(chip8);
	return exitStatus;
}
