#include <stdio.h>
#include "cpu.h"
#include "io.h"


int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("usage: %s rompath\n", argv[0]);
		return 1;
	}
	if(initScreen() > 0) {
		printf("ERROR: Unable to init screen.\n");
		return 2;
	}

	if(initAudio() > 0) {
		printf("ERROR: Unable to init audio.\n");
		return 2;
	}
		
	initChip8(argv[1]);

	if(runCycles() > 0) {
		printf("ERROR: unknown opcode!");
		return 1;
	}
	return 0;
}
