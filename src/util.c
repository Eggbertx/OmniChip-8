#include <stdarg.h>
#ifdef __CC65__
	#include <conio.h>
#else
	#include <stdio.h>
	#include <stdlib.h>
#endif

#include "util.h"
#include "rom_embed.h"

uchar loadROM(char* file) {
	ushort i = 0;
#ifdef __EMBED_ROM__
	chip8.romBytes = ROM;
	chip8.romSize = ROM_len;
#else
	FILE *rom_file;

	rom_file = fopen(file, "rb");
	if(!rom_file) {
		fprintf(stderr, "Error: couldn\'t load %s\n", file);
		return 1;
	}
	fseek(rom_file, 0L, SEEK_END);
	chip8.romSize = ftell(rom_file);
	if(chip8.romSize == 0) {
		return 1;
	}
	printf("Loading %s (%d bytes)\n", file, chip8.romSize);
	rewind(rom_file);

	chip8.romBytes = (uchar *)malloc(chip8.romSize+1);
	fread(chip8.romBytes, 1, chip8.romSize,rom_file);
	fclose(rom_file);
#endif
	for(i = 0; i < 80; i++) {
		chip8.memory[FONT_START_ADDR + i] = font[i];
	}
	for(i = 0; i < chip8.romSize; i++) {
		chip8.memory[ROM_START_ADDR + i] = chip8.romBytes[i];
	}
	return 0;
}
