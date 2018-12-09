#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cpu.h"
#include "io.h"


uchar font[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
	0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
	0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
	0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
	0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
	0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
	0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
	0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
	0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
	0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
	0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
	0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
	0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
	0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
	0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
	0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
};

void reset() {
	cpu.status = STATUS_RUNNING;
	cpu.PC = ROM_START_ADDR;
	srand(time(NULL));
	memset(cpu.memory, 0, sizeof(cpu.memory));
	memset(cpu.stack, 0, sizeof(cpu.stack));
	memset(cpu.V, 0, sizeof(cpu.V));
	memset(cpu.key, 0, sizeof(cpu.key));
	cpu.stackPointer = 0;
	cpu.I = 0;
	cpu.delayTimer = 0;
	cpu.soundTimer = 0;
	cpu.currentKey = -1;
}

uchar initChip8(char* rom) {
	reset();
	return load(rom);
}

void clearDisplay() {
	memset(cpu.screen, 0, 64*32);
	/* clearScreen(); */
}

void setPixel(uchar x, uchar y) {
	if(x > 64) {
		x -= 64;
		cpu.V[0xF] = 1;
	}
	if(y > 32) {
		y -= 32;
		cpu.V[0xF] = 1;
	}
	int i = x + (y * 64);
	cpu.screen[i] ^= 1;
}

void drawSprite(uchar height, uchar x, uchar y) {
	int sy;
	int sx;
	for(sy = 0; sy < height; sy++) {
		for(sx = 0; sx < 8; sx++) {
			if(cpu.memory[cpu.I+sx+sy] != 0) setPixel(x,y);
		}
	}
}

void drawScreen() {
	int p = 0;
	for(p = 0; p < 64*32; p++) {
		if((cpu.screen[p>>3]&(128>>(p&7))) != 0) {
			int x = p % 64;
 			int y = (p - x) / 32;
			drawPixel(x,y);
		}
	}
	flipScreen();
}

ushort getOpcode() {
	ushort opcode = cpu.memory[cpu.PC] << 8 | cpu.memory[cpu.PC + 1];
	cpu.PC += 2;
	return opcode;
} 

void dumpBytes(uchar* bytes, short filesize, char* filename) {
	int i;
	FILE* dumpFile;
	
	printf("Dumping to %s\n", filename);

	dumpFile = fopen(filename, "w");
	if(dumpFile == NULL) {
		printf("Error opening %s\n", filename);
		return;
	}

	for(i = 0; i < filesize; i++) {
		fprintf(dumpFile, "%02X ", bytes[i]);
		if((i + 1) % 0x10 == 0) fprintf(dumpFile, "\n");
	}
	fclose(dumpFile);
}

void printStatus() {
	int r;
	int s;

	printf("CPU V registers:\n");
	for(r = 0; r < 16; r++) {
		printf("\tV%X: %x\n", r, cpu.V[r]);
	}
	printf("Program counter: %d\n", cpu.PC);
	printf("CPU Address register (cpu.I): %d\n", cpu.I);
	printf("Delay timer: %d\nSound timer: %d\n", cpu.delayTimer, cpu.soundTimer);
	printf("cpu.drawFlag: %d\n", cpu.drawFlag);
	printf("Stack:\n");
	for(s = 0; s < 16; s++) {
		printf("\tstack[%d]: %x\n", s, cpu.stack[s]);
	}
	printf("Stack pointer: %d\n", cpu.stackPointer);
}

uchar runCycles(uchar printdebug) {
	uchar x = 0;	/* -X-- */
	uchar y = 0;	/* --Y- */
	ushort nnn = 0;	/* -nnn */
	uchar nn = 0;	/* --nn */
	uchar n = 0;	/* ---n */
	uchar event;
	cpu.currentKey = -1; /* -1 if no keys are pressed, otherwise */

	while(cpu.status != STATUS_STOPPED) {
		event = getEvent();
		if(event == EVENT_ESCAPE || event == EVENT_WINDOWCLOSE) {
			cpu.status = STATUS_STOPPED;
			return 0;
		}
		if(cpu.status == STATUS_PAUSED) {
			continue;
		}
		drawScreen();
		delay(60);


		if(cpu.PC > ROM_END_ADDR) {
			printf("Program counter (%04X) reached end of file\n", cpu.PC);
			cpu.status = STATUS_PAUSED;
			continue;
		}

		cpu.opcode = (cpu.memory[cpu.PC] << 8) | (cpu.memory[cpu.PC + 1]);
		cpu.PC += 2;

		x = (cpu.opcode & 0x0F00) >> 8;
		y = (cpu.opcode & 0x00F0) >> 4;

		nnn = cpu.opcode & 0x0FFF;
		nn = cpu.opcode & 0x00FF;
		n = cpu.opcode & 0x000F;

		cpu.currentKey = getKey();

		switch(cpu.opcode & 0xF000) {
			case 0x0000:
				if(cpu.opcode == 0x00E0) {
					if(printdebug) addrInfo("CLS");
					clearDisplay();
				} else if(cpu.opcode == 0x00EE) {
					if(printdebug) addrInfo("RET");
					cpu.stackPointer--;
					cpu.PC = cpu.stack[cpu.stackPointer] + ROM_START_ADDR;
				} else {
					if(printdebug) addrInfo("SYS %04X", nnn); /* not used for most "modern" ROMs */
				}
				break;
			case 0x1000:
				if(printdebug) addrInfo("JP %#04x", nnn);
				if(nnn == cpu.PC - 2) {
					if(printdebug) printf("Reached infinite loop in CHIP-8 ROM, pausing CPU\n");
					cpu.status = STATUS_PAUSED;
					break;
				}
				cpu.PC = nnn;
				break;
			case 0x2000:
				/* increment SP, put PC on top of stack, set to nnn */
				if(printdebug) addrInfo("CALL %#04X", nnn);
				cpu.stack[cpu.stackPointer] = cpu.PC;
				cpu.stackPointer++;
				cpu.PC = nnn;
				break;
			case 0x3000:
				if(printdebug) addrInfo("SE V%X, #%x", x, nn);
				if(cpu.V[x] == nn) {
					cpu.PC += 2;
				}
				break;
			case 0x4000:
				if(printdebug) addrInfo("SNE V%X, #%x", x, nn);
				if(cpu.V[x] != nn) {
					cpu.PC += 2;
				}
				break;
			case 0x5000:
			if(printdebug) addrInfo("SE V%X, V%X", x, y);
				if(cpu.V[x] != cpu.V[y]) cpu.PC += 2;
				break;
			case 0x6000:
				if(printdebug) addrInfo("LD V%X, #%x", x, nn);
				cpu.V[x] = nn;
				break;
			case 0x7000:
				if(printdebug) addrInfo("ADD V%X, #%x", x, nn);
				cpu.V[x] += nn;
				break;
			case 0x8000: {
				if(n == 0x0000) {
					if(printdebug) addrInfo("LD V%X, V%X", x, y);
					cpu.V[x] = cpu.V[y];
				} else if(n == 0x0001) {
					if(printdebug) addrInfo("OR V%X, V%X", x, y);
					cpu.V[x] = cpu.V[x] | cpu.V[y];
				} else if(n == 0x0002) {
					if(printdebug) addrInfo("AND V%X, V%X", x, y);
					cpu.V[x] = cpu.V[x] & cpu.V[y];
				} else if(n == 0x0003) {
					if(printdebug) addrInfo("XOR V%X, V%X", x, y);
					cpu.V[x] ^= cpu.V[y];
				} else if(n == 0x0004) {
					ushort sum = 0;
					if(printdebug) addrInfo("add V%X and V%X, if > 255, VF = 1. V%X = sum & 0xF", x, y, x);
					sum = cpu.V[x] + cpu.V[y];
					cpu.V[0xF] = sum > 255;
					cpu.V[x] = sum & 0xFF;
				} else if(n == 0x0005) {
					/* Set Vx = Vx - Vy, set VF = NOT borrow.
					 * If Vx > Vy, then VF is set to 1, otherwise 0.
					 * Then Vy is subtracted from Vx, and the results stored in Vx. */
					if (cpu.V[x] > cpu.V[y]) cpu.V[0xF] = 1;
					else cpu.V[0xF] = 0;
					cpu.V[x] -= cpu.V[y];
				} else if(n == 0x0006) {
					if(printdebug) addrInfo("Stores the least significant bit of V%X in VF and then shifts V%X to the right by 1", x, x);
					cpu.V[0xF] = (cpu.V[x] & 1);
					cpu.V[x] = cpu.V[x] >> 1;
				} else if(n == 0x0007) {
					if(printdebug) addrInfo("Sets V%X to V%X minus V%X. VF is set to 0 when there's a borrow, and 1 when there isn't", x, y, x);

					if(cpu.V[y] > cpu.V[x]) cpu.V[0xF] = 1;
					else cpu.V[0xF] = 0;
					cpu.V[x] = cpu.V[y] - cpu.V[x];
				} else if(n == 0x000E) {
					if(printdebug) addrInfo("Stores the most significant bit of V%X in VF and then shifts V%X to the left by 1", x, x);
					cpu.V[0xF] = (cpu.V[x] & 128);
					cpu.V[x] = cpu.V[x] << 1;
				} else {
					goto unrecognized_opcode;
				}
			}
			break;
			case 0x9000:
				if(cpu.V[x] != cpu.V[y])
					cpu.PC += 2;
				break;
			case 0xA000:
				if(printdebug) addrInfo("LD I, %#04x", nnn);
				cpu.I = nnn;
				break;
			case 0xB000:
				cpu.PC = cpu.V[0] + nnn;
				break;
			case 0xC000: {
				if(printdebug) addrInfo("RND V%X, #%x", x, nn);
				uchar rnd = (rand() % 255) + 1;
				cpu.V[x] = rnd & nn;
				break;
			}
			case 0xD000:
				if (printdebug) addrInfo("DRW V%X, V%X, #%x", x, y, n);
				uchar sx = 0;
				uchar sy = 0;
				
				for(sy = 0; sy < n; sy++) {
					uchar sprite = cpu.memory[cpu.I + sy];
					for(sx = 0; sx < 8; sx++) {
						if((sprite & 128) > 0) {
							if(x + sx > 64 || y + sy > 32) cpu.V[0xF] = 1;
							setPixel(sx, sy);
						}
					}
				}
				/* drawSprite(n, x, y); */

				break;
			case 0xE000:
				if(nn == 0x009E) {
					if(printdebug) addrInfo("Skip if key in V%X is pressed, not yet fully implemented", x);
					if(cpu.currentKey > -1 && cpu.V[x] == cpu.currentKey) {
						cpu.PC += 2;
					}
				} else if(nn == 0x00A1) {
					if(printdebug) addrInfo("Skip if key in V%X is not pressed, not yet fully implemented", x);
					if(cpu.currentKey > -1 && cpu.V[x] != cpu.currentKey) {
						cpu.PC += 2;
					}
				}
				break;
			case 0xF000: {
				if(nn == 0x0007) {
					cpu.V[x] = cpu.delayTimer;
				} else if(nn == 0x000A) {
					do {
						cpu.currentKey = getKey();
					} while(cpu.currentKey == -1);
					cpu.V[x] = cpu.currentKey;
					cpu.PC += 2;
				} else if(nn == 0x0015) {
					cpu.delayTimer = cpu.V[x];
				} else if(nn == 0x0018) {
					cpu.soundTimer = cpu.V[x];
				} else if(nn == 0x001E) {
					cpu.I += cpu.V[x];
				} else if(nn == 0x0029) {
					if(printdebug) addrInfo("Set I to the location of the sprite for the character in V%X, not yet implemented", x);
					goto unsupported_opcode;
				} else if(nn == 0x0033) {
					cpu.memory[cpu.I+0] = (cpu.V[x]/10) % 10;
					cpu.memory[cpu.I+1] = (cpu.V[x]/100) % 10;
					cpu.memory[cpu.I+2] = cpu.V[x] % 10;
				} else if(nn == 0x0055) {
					if(printdebug) addrInfo("write registers V0 - V%X to memory, with %#04x as the offset", x, cpu.I);
					ushort i;
					for(i = 0; i <= x; i++) {
						cpu.memory[cpu.I + i] = cpu.V[cpu.I + i];
					}
				} else if(nn == 0x0065) {
					ushort i;
					for(i = 0; i < x; i++) {
						cpu.V[cpu.I + i] = cpu.memory[cpu.I + i];
					}
				}
				break;
			default:
				goto unrecognized_opcode;
			}
		}
	}
	return 0;

	unrecognized_opcode:
		fprintf(stderr, "Unrecognized opcode: %04x at %04x\n", cpu.opcode, cpu.PC);
		dumpBytes(cpu.memory, 4096, "dumps/memorybytes_unrecognized.txt");
		printStatus();
		cpu.status = STATUS_STOPPED;
		return 1;

	unsupported_opcode:
		fprintf(stderr, "Unsupported opcode: %04x at %04x\n", cpu.opcode, cpu.PC);
		dumpBytes(cpu.memory, 4096, "dumps/memorybytes_unsupported.txt");
		printStatus();
		cpu.status = STATUS_STOPPED;
		return 1;
}

uchar load(char* file) {
	FILE *rom_file;
	uchar f = 0;
	uchar m = 0;

	rom_file = fopen(file, "rb");
	if(!rom_file) {
		fprintf(stderr, "Error: couldn\'t load %s\n", file);
		return 1;
	}
	fseek(rom_file, 0L, SEEK_END);
	cpu.romSize = ftell(rom_file);
	printf("Loading %s (%d bytes)\n", file, cpu.romSize);
	rewind(rom_file);

	cpu.romBytes = (uchar *)malloc(cpu.romSize+1);
	fread(cpu.romBytes, 1, cpu.romSize,rom_file);
	fclose(rom_file);

	for(f = 0; f < 80; f++) {
		cpu.memory[FONT_START_ADDR + f] = font[f];
	}
	for(m = 0; m < cpu.romSize; m++) {
		cpu.memory[ROM_START_ADDR + m] = cpu.romBytes[m];
	}
	return 0;
}
