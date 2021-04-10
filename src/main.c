#include <stdio.h>
/* TODO: replace printf/fprintf with conio.h functions for cc65 */
#include <stdlib.h>
#ifdef EMSCRIPTEN_IO
#include <emscripten.h>
#endif
#include "chip8.h"
#include "io.h"
#include "util.h"

int main(int argc, char *argv[]) {
#ifdef GB_IO
	/* Kinda hacky but z88dk doesn't seem to like malloc'ing structs */
	struct Chip8 chip8;
#else
	struct Chip8* chip8 = NULL;
	chip8 = (struct Chip8*)malloc(sizeof(struct Chip8));
#endif

	if(argc != 2) {
	#if !defined (__CC65__) && !defined(EMSCRIPTEN_IO) && !defined(GB_IO)
		oc8log("usage: %s rompath\n", argv[0]);
		goto finish;
	#endif
	}

	if(initScreen() > 0) {
		oc8log("ERROR: Unable to init screen.\n");
		goto finish;
	}
	
	if(initAudio() > 0) {
		oc8log("ERROR: Unable to init audio.\n");
		goto finish;
	}

	if (initChip8(chip8, argv[1]) > 0) {
		oc8log("ERROR: Something went wrong while loading %s\n", argv[1]);
		goto finish;
	}

#ifdef EMSCRIPTEN_IO
	emscripten_set_main_loop_arg(runCycles, chip8, 60, 0);
#else
	runCycles(chip8);
#endif

finish:
	cleanup();
#ifdef GB_IO
	return chip8.status;
#else
	return chip8->status;
#endif
}
