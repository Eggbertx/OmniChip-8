#include <stdio.h>
#include "cpu.h"
#include "io.h"


int main(int argc, char *argv[]) {
#if defined(SDL_IO) || defined(CURSES_IO)
	int debug = 1;
#else
	int debug = 0;
#endif
	if(argc != 2) {
		printf("usage: %s rompath\n", argv[0]);
		return 0;
	}

	if(initScreen() > 0) {
		printf( "ERROR: Unable to init screen.\n");
		return 0;
	}

	if(initAudio() > 0) {
		printf( "ERROR: Unable to init audio.\n");
		return 0;
	}

	if (initChip8(argv[1]) > 0) {
		printf( "ERROR: Something went wrong while loading %s\n", argv[1]);
		return 0;
	};

	if(runCycles(debug) > 0) {
		return 0;
	}

	cleanup();
	return 0;
}
