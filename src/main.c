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


#ifndef EMBED_ROM
#ifdef C64_IO
#define MAX_FILENAME_LENGTH 16
#else
#define MAX_FILENAME_LENGTH 256
#endif
#endif

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
#ifndef EMBED_ROM
	if(argc == 2) {
		chip8.romPath = argv[1];
	} else {
		char path[MAX_FILENAME_LENGTH];
		printf("Enter path to ROM: ");
		if(fgets(path, sizeof(path), stdin) != NULL) {
			size_t len = strlen(path);
			if(len > 0 && path[len - 1] == '\n') {
				path[len - 1] = '\0';
			}
			chip8.romPath = path;
		} else {
			printf("ERROR: Unable to read ROM path from stdin.\n");
			return 1;
		}
	}
#endif
	if (initChip8() > 0) {
	#if defined(C64_IO) || defined(GB_IO) || defined(TI83_IO)
		while(1){}
	#endif
		return 1;
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
	emscripten_set_main_loop(runCycles, 0, 1);
#else
	runCycles();
#endif

finish:
	cleanup();
	return chip8.status;
}
