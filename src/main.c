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

static void runCycles() {
	uchar event = EVENT_NULL;
	while(chip8.status != STATUS_STOPPED) {
		event = getEvent();
		if(event == EVENT_ESCAPE || event == EVENT_WINDOWCLOSE) {
			chip8.status = STATUS_STOPPED;
			return;
		}

		if(chip8.status == STATUS_PAUSED) {
			drawScreen(&chip8);
			continue;
		}

		doCycle(&chip8);
		if(chip8.drawFlag == 1) {
			drawScreen(&chip8);
		}
	}
}

int main(int argc, char *argv[]) {
#ifndef __EMBED_ROM__
	if(argc == 2) {
		chip8.romPath = argv[1];
	} else {
		printf("usage: %s [--print-opcodes] path/to/rom\n", argv[0]);
		return 1;
	}
#endif
	if (initChip8(&chip8) > 0) {
		printf("ERROR: Something went wrong while loading %s\n", chip8.romPath);
		goto finish;
	}

	if(initScreen() > 0) {
		printf("ERROR: Unable to init screen.\n");
		goto finish;
	}
	
	if(initAudio() > 0) {
		printf("ERROR: Unable to init audio.\n");
		goto finish;
	}

#ifdef EMSCRIPTEN_IO
	emscripten_set_main_loop_arg(runCycles, chip8, 60, 0);
#else
	runCycles();
#endif

finish:
	cleanup();
	return chip8.status;
}
