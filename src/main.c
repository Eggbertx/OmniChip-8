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
	struct Chip8* chip8 = NULL;
	chip8 = (struct Chip8*)malloc(sizeof(struct Chip8));
	if(argc != 2) {
		#if !defined (__CC65__) && !defined(EMSCRIPTEN_IO)
			oc8log("usage: %s rompath\n", argv[0]);
			free(chip8);
			return 1;
		#endif
	}

	if(initScreen() > 0) {
		oc8log("ERROR: Unable to init screen.\n");
		free(chip8);
		return 1;
	}

	if(initAudio() > 0) {
		oc8log("ERROR: Unable to init audio.\n");
		cleanup();
		free(chip8);
		return 1;
	}

	if (initChip8(chip8, argv[1]) > 0) {
		oc8log("ERROR: Something went wrong while loading %s\n", argv[1]);
		cleanup();
		free(chip8);
		return 1;
	}

#ifdef EMSCRIPTEN_IO
	emscripten_set_main_loop_arg(runCycles, chip8, 60, 0);
#else
	runCycles(chip8);
#endif

	cleanup();
	free(chip8);
	return chip8->status == STATUS_ERROR;
}
