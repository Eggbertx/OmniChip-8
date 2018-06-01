#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cpu.h"
#include "io.h"


const uchar font[16][5] = { 
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

void dec2Bin(uchar in, uchar (*out)[5]) {
	printf(out[3]);
	for (int i = 1 << 31; i > 0; i = i / 2) {
		(in & i)? printf("1"): printf("0");
	}
	
}

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

uchar initChip8(char* rom) {
	cpu.PC = START_ADDR;
	load(rom);
	cpu.running = 1;
	return 0;
}

void clearDisplay() {
	memset(cpu.screen, 0, sizeof(cpu.screen[0][0]*64*32));
}

void drawChar(uchar c, uchar x, uchar y) {
	uchar nibble;
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

ushort getOpcode(ushort pc) {
	cpu.PC += 2;
	ushort opcode = cpu.romBytes[cpu.PC] << 8 | cpu.romBytes[cpu.PC + 1];
	printf("opcode: %#x\n", opcode);
	return opcode;
} 

void reset() {
	cpu.PC = 0;
	srand(time(NULL));
	memset(cpu.memory, 0, sizeof(uchar)*4096);

	memset(cpu.stack, 0, sizeof(uchar)*16);
	memset(cpu.V,0,sizeof(uchar)*16);
	cpu.stackPointer = 0;
	cpu.I = 0;
	cpu.delayTimer = 0;
	cpu.soundTimer = 0;
	
}

void printBytes(uchar* bytes, ushort filesize, uchar* filename) {
	uchar* dumpFileName;
	sprintf(dumpFileName, "dumps/%s.dmp.txt", filename);
	FILE* dumpFile = fopen(dumpFileName, "w");
	if(dumpFile == NULL) {
		printf("Error opening %s\n", dumpFileName);
		return;
	}

	for(int i = 0; i < filesize; i++) {
		fprintf(dumpFile, "%02X ", bytes[i]);
		if((i + 1) % 32 == 0) fprintf(dumpFile, "\n");
	}
}


uchar runCycles() {
	ushort nnn = 0; // address
	uchar x = 0;
	uchar y = 0;
	uchar kk = 0; // end byte

	uchar event;
	reset();
	drawPixel(3,4);
	drawPixel(4,4);
	while(cpu.running) {
		event = getEvent();
		if(event == 1) {
			cpu.running = 0;
			return 0;
		}
		cpu.opcode = getOpcode(cpu.PC);
		// Examples
		nnn = cpu.opcode & 0x0FFF; // addr
		x = (cpu.opcode & 0x0F00) >> 8;
		y = (cpu.opcode & 0x00F0) >> 4;
		kk = cpu.opcode & 0x00FF;

		switch(cpu.opcode & 0xF000) {
			case 0x0000:
				if(cpu.opcode == 0x00E0) {
					// CLS (clear screen)
					clearDisplay();
				} else if(cpu.opcode == 0x00EE) {
					// RET (return from subroutine)
					cpu.PC = cpu.stack[--cpu.stackPointer];
				} else {
					// JMP nnn (unused)
				}
				break;
			case 0x1000:
				// goto nnn (0x1nnn)
				cpu.PC = cpu.opcode & 0x0FFF;
				break;
			case 0x2000:
				// call subroutine at nnn
				// increment SP, put PC on top of stack, set to nnn
				cpu.stackPointer++;
				cpu.PC = cpu.opcode & 0x0FFF;
				break;
			case 0x3000:
				// ADD Vx, Vy
				if(cpu.V[x] == (cpu.opcode & 0x00FF)) {
					cpu.PC += 2;
				}
				break;
			case 0x4000:
				if(cpu.V[x] != kk) {
					cpu.PC += 2;
				}
				break;
			case 0x5000:
				cpu.V[x] = (cpu.opcode & 0x0F00) >> 8;
				cpu.V[y] = (cpu.opcode & 0x0F00) >> 4;
				if(cpu.V[x] != cpu.V[y]) cpu.PC += 2;
				break;
			case 0x6000:
				cpu.V[x] = cpu.opcode & 0x00FF;
				break;
			case 0x7000:
				cpu.V[x] += cpu.opcode & 0x0FF;
				break;
			case 0x8000:
				printf("cpu.opcode & 0x000F: %x\n", cpu.opcode & 0x000F);
				uchar n = cpu.opcode & 0x000F;
				if(n == 0x0) {
					// store V[y] in V[x]
					cpu.V[x] = cpu.V[y];
				} else if(n == 0x1) {
					// V[x] = V[x] or V[y]
					cpu.V[x] = cpu.V[x] | cpu.V[y];
				} else if(n == 0x2) {
					// V[x] = V[x] and V[y]
					cpu.V[x] = cpu.V[x] & cpu.V[y];
				} else if(n == 0x3) {
					// V[x] = V[x] xor V[y]
					cpu.V[x] = cpu.V[x] ^ cpu.V[y];
				} else if(n == 0x4) {
					// add V[x] and V[y], if > 255, V[0xF] = 1. V[x] = sum & 0xF
					int sum = cpu.V[x] + cpu.V[y];
					cpu.V[0xF] = sum > 0xFF;
					cpu.V[x] = sum & 0xFF;
				} else if(n == 0x5) {
					// Set Vx = Vx - Vy, set VF = NOT borrow.
					// If Vx > Vy, then VF is set to 1, otherwise 0.
					// Then Vy is subtracted from Vx, and the results stored in Vx.
					if(cpu.V[x] > cpu.V[y]) cpu.V[0xF] = 1;
					else cpu.V[0xF] = 0;
					cpu.V[x] -= cpu.V[y];
				} else if(n == 0x6) {

				} else if(n == 0x7) {

				} else if(n == 0xE) {

				} else {
					printf("Error: Unrecognized 0x8... opcode %#x.\n", cpu.opcode);
					cpu.running = 0;
					return 1;
				}
				break;
			case 0x9000:
				
				break;
			case 0xA000:
				cpu.I = nnn;
				break;
			case 0xB000:
				cpu.PC = nnn + cpu.V[0];
				break;
			case 0xC000: {
				// The interpreter generates a random number from 0 to 255,
				// which is then ANDed with the value kk. The results are stored in Vx.
				uchar rnd = (rand() % 255) + 1;
				cpu.V[x] = kk & rnd;
				break;
			}
			case 0xD000:
				
				break;
			case 0xE000:
				
				break;
			case 0xF000:
				if(kk == 0x0055) {
					// write registers V[0] - V[x] to memory, with cpu.I as the offset
					for(ushort i = 0; i <= x; i++) {
						if(cpu.I + i < 4096) cpu.memory[cpu.I + i] = cpu.V[i];
					}
				}
				break;
			default:
				printf("Error: Unrecognized opcode %#x.\n", cpu.opcode);
				printBytes(cpu.memory, 4096,"ibm.dmp.txt");
				cpu.running = 0;
				return 1;
		}
	}
	return 0;
}

uchar load(char* file) {
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
	cpu.romBytes=(uchar *)malloc(filesize+1);
	
	fread(cpu.romBytes,filesize,1,rom_file);
	fclose(rom_file);
	free(rom_file);
	return 0;
}

char* hex2Dec(int hex) {
	char *dec = malloc(6);
	sprintf(dec, "%d", hex);
	return dec;
}

char* dec2Hex(int dec) {
	char *hex = malloc(sizeof(uchar) * 4);
	sprintf(hex, "%0x", dec);
	return hex;
}
