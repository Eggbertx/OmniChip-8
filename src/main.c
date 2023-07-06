#include <stdio.h>
/* TODO: replace printf/fprintf with conio.h functions for cc65 */
#include <stdlib.h>
#include <string.h>
#ifdef EMSCRIPTEN_IO
#include <emscripten.h>
#endif
#include "chip8.h"
#include "io.h"
#include "util.h"

struct Chip8 chip8;
uchar printOpcodes;

int main(int argc, char *argv[]) {
#ifndef __EMBED_ROM__
	printOpcodes = 0;
	if(argc == 3 && argv[1][0] == '-' && argv[1][1] == '-') {
		chip8.romPath = argv[2];
		if(strcmp(argv[1], "--print-opcodes") == 0) {
			printOpcodes = 1;
		}
	} else if(argc == 2 && argv[1][0] != '-' && argv[1][0] != '-') {
		chip8.romPath = argv[1];
	} else {
		oc8log("usage: %s [--print-opcodes] path/to/rom\n", argv[0]);
		return 1;
	}
#endif

	if (initChip8(&chip8) > 0) {
		oc8log("ERROR: Something went wrong while loading %s\n", chip8.romPath);
		goto finish;
	}

	if(initScreen() > 0) {
		oc8log("ERROR: Unable to init screen.\n");
		goto finish;
	}
	
	if(initAudio() > 0) {
		oc8log("ERROR: Unable to init audio.\n");
		goto finish;
	}


#ifdef EMSCRIPTEN_IO
	emscripten_set_main_loop_arg(runCycles, chip8, 60, 0);
#else
	runCycles(&chip8, printOpcodes);
#endif

finish:
	cleanup();
	return chip8.status;
}
