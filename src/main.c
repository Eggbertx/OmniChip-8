#include <stdio.h>
#include "cpu.h"
#include "io.h"


int main(int argc, char *argv[]) {
	int debug = 0;
	if(argc != 2) {
		fprintf(stderr, "usage: %s rompath\n", argv[0]);
		return 0;
	}

	if(initScreen() > 0) {
		fprintf(stderr, "ERROR: Unable to init screen.\n");
		return 0;
	}

	if(initAudio() > 0) {
		fprintf(stderr, "ERROR: Unable to init audio.\n");
		return 0;
	}

	if (initChip8(argv[1]) > 0) {
		fprintf(stderr, "ERROR: Something went wrong while loading %s\n", argv[1]);
		return 0;
	};

#ifdef SDL_IO
	// TODO: figure out a better way to handle this
	if(runCycles(1) > 0) {
		return 0;
	}
#else
	if(runCycles(0) > 0) {
		return 0;
	}
#endif
	cleanup();
	return 0;
}
