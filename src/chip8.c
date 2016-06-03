#include <stdio.h>
#include <string.h>
#include "chip8.h"
#include "io.h"


// font comes from http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
 unsigned char font[16][5] = { 
  {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
  {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
  {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
  {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
  {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
  {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
  {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
  {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
  {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
  {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
  {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
  {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
  {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
  {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
  {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
  {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
};

// I realize this is probably a bad way to do things, I'll fix it later
char hex2bin[16][4] = {
	{0,0,0,0},
	{0,0,0,1},
	{0,0,1,0},
	{0,0,1,1},
	{0,1,0,0},
	{0,1,0,1},
	{0,1,1,0},
	{0,1,1,1},
	{1,0,0,0},
	{1,0,0,1},
	{1,0,1,0},
	{1,0,1,1},
	{1,1,0,0},
	{1,1,0,1},
	{1,1,1,0},
	{1,1,1,1}
};

unsigned char initChip8(char* rom) {
	cpu.PC = 0x200;
	load(rom);
	runCycles();
	return 0;
}

void drawChar(unsigned char c, unsigned char x, unsigned char y) {
	char nibble;
	char charline[5];
	for(int i=0;i<5;i++)
		charline[i] = font[c][i]; 
	
	for(int ay=0;ay<5;ay++) {
		nibble = (font[c][ay] >> 4) ;
		for(int ax=0;ax<4;ax++) {
			if(hex2bin[nibble][ax] == 1)
				drawPixel(ax+x,ay+y);
		}
	}
}

unsigned char runCycles() {
	while(cpu.running) {
		//opcode = getOpcode(cpu, cpu->PC);
		switch(opcode) {
			case 0x01:
				// 
			default:
				printf("Error: Unrecognized opcode.\n");

		}
	}
	return 0;
}

unsigned char load(char* file) {
	FILE *rom_file;
	long filesize;
	rom_file = fopen(file, "rb");
	if(!rom_file) {
		printf("Error: couldn\'t load file \"%s\"\n", file);
		exit(1);
		return 1;
	}
	fseek(rom_file, 0L, SEEK_END);
	filesize = ftell(rom_file);
	rewind(rom_file);
	cpu.romBytes=(char *)malloc(filesize+1);
	
	fread(cpu.romBytes,filesize,1,rom_file);
	

	return 0;
}

