#include <stdio.h>
#include "chip8.h"
#include "io.h"


void usage(char* this) {
	printf("oc8: usage: oc8 [rompath]\n");
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		usage(argv[0]);
		return 1;
	}
	initScreen();
	initChip8(argv[1]);
	return 0;
}
