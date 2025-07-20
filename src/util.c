#include <stdarg.h>
#ifdef __CC65__
	#include <conio.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#ifdef __EMBED_ROM__
	#include "rom_embed.h"
#endif

/* to be used since CC65 doesn't have lseek, required by fseek and ftell, for most platforms */
#define BUFFER_SIZE 256

uchar loadROM(char* file) {
#ifdef __EMBED_ROM__
	memcpy(chip8.memory + ROM_START_ADDR, ROM, ROM_len);
	chip8.romSize = ROM_len;
	if(chip8.romSize == 0) {
		return 1;
	}
#else
	FILE *romFile;
	ushort bufferRead = 0;

	romFile = fopen(file, "rb");
	if(!romFile) {
		perror(file);
		return 1;
	}
	#ifdef __CC65__
		chip8.romSize = 0;
		do {
			bufferRead = fread(chip8.memory + bufferRead, 1, BUFFER_SIZE, romFile);
			chip8.romSize += bufferRead;
		} while (bufferRead > 0);
		if(chip8.romSize == 0) {
			printf("%s: ROM file appears to be empty", file);
			fclose(romFile);
			return 1;
		}
	#else
		fseek(romFile, 0L, SEEK_END);
		chip8.romSize = ftell(romFile);
		if(chip8.romSize == 0) {
			printf("%s: ROM file appears to be empty", file);
			fclose(romFile);
			return 1;
		}
		printf("Loading %s (%d bytes)\n", file, chip8.romSize);
		rewind(romFile);

		fread(chip8.memory + ROM_START_ADDR, 1, chip8.romSize, romFile);
	#endif
	fclose(romFile);
#endif
	memcpy(chip8.memory + FONT_START_ADDR, font, 80);
	return 0;
}
