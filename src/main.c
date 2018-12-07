#include <stdio.h>
#include "cpu.h"
#include "io.h"


int main(int argc, char *argv[]) {
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

	if(runCycles(1) > 0) {
		return 0;
	}

	cleanup();
	return 0;
}
