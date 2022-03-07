#include <stdio.h>
/* TODO: replace printf/fprintf with conio.h functions for cc65 */
#include <stdlib.h>
#ifdef EMSCRIPTEN_IO
#include <emscripten.h>
#endif
#include "chip8.h"
#include "io.h"
#include "util.h"

struct Chip8 chip8;
int main(int argc, char *argv[]) {
	if(argc != 2) {
	#ifndef __EMBED_ROM__
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

	chip8.romPath = argv[1];
	if (initChip8(&chip8) > 0) {
		oc8log("ERROR: Swent wrong while loading %s\n", argv[1]);
		goto finish;
	}

#ifdef EMSCRIPTEN_IO
	emscripten_set_main_loop_arg(runCycles, chip8, 60, 0);
#else
	runCycles(&chip8);
#endif

finish:
	cleanup();
	return chip8.status;
}
