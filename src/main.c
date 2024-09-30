#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef EMSCRIPTEN_IO
#include <emscripten.h>
#endif
#include "chip8.h"
#include "io.h"
#include "util.h"

static void runCycles() {
	uchar event = EVENT_NULL;
	#ifndef EMSCRIPTEN_IO
	while(chip8.status != STATUS_STOPPED && chip8.status != STATUS_ERROR) {
	#endif
		event = getEvent();
		if(event == EVENT_ESCAPE || event == EVENT_WINDOWCLOSE) {
			chip8.status = STATUS_STOPPED;
			return;
		}

		if(chip8.status == STATUS_PAUSED) {
			drawScreen();
			#ifndef EMSCRIPTEN_IO
			continue;
			#else
			return;
			#endif
		}

		doCycle();
		if(chip8.drawFlag == 1) {
			drawScreen();
		}
	#ifndef EMSCRIPTEN_IO
	}
	#endif
}

int main(int argc, char *argv[]) {
#ifndef __EMBED_ROM__
	if(argc == 2) {
		chip8.romPath = argv[1];
	} else {
		printf("usage: %s path/to/rom\n", argv[0]);
		return 1;
	}
#endif
	if (initChip8() > 0) {
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
	printf("Hello, emscripten!\n");
	emscripten_set_main_loop(runCycles, 0, 1);
#else
	runCycles();
#endif

finish:
	cleanup();
	return chip8.status;
}
