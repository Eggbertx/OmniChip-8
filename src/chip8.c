#include <stdarg.h>
#include <stdio.h> 
/* TODO: replace printf/fprintf with conio.h functions for cc65 */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chip8.h"
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

void reset(struct Chip8* chip8) {
	chip8->status = STATUS_RUNNING;
	chip8->PC = ROM_START_ADDR;
	srand(time(NULL));
	memset(chip8->memory, 0, sizeof(chip8->memory));
	memset(chip8->stack, 0, sizeof(chip8->stack));
	memset(chip8->V, 0, sizeof(chip8->V));
	memset(chip8->key, 0, sizeof(chip8->key));
	chip8->stackPointer = 0;
	chip8->I = 0;
	chip8->delayTimer = 0;
	chip8->soundTimer = 0;
	chip8->currentKey = -1;
}

uchar initChip8(struct Chip8* chip8, char* rom) {
	reset(chip8);
	return load(chip8, rom);
}

void clearDisplay(struct Chip8* chip8) {
	memset(chip8->screen, 0, 64*32);
	/* clearScreen(); */
}

void drawScreen(struct Chip8* chip8) {
	int x = 0;
	int y = 0;
	for(y = 0; y < 32; y++) {
		for(x = 0; x < 64; x++) {
			if(chip8->screen[x+y*64] != 0) {
				drawPixel(x, y);
			}
		}
	}

	flipScreen();
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

void printStatus(struct Chip8* chip8) {
	int r = 0;
	int s = 0;

	printf("V registers:\n");
	for(r = 0; r < 16; r++) {
		printf("\tV%X: %x\n", r, chip8->V[r]);
	}
	printf("Program counter: %d\n", chip8->PC);
	printf("Address register (I): %d\n", chip8->I);
	printf("Delay timer: %d\nSound timer: %d\n", chip8->delayTimer, chip8->soundTimer);
	printf("chip8->drawFlag: %d\n", chip8->drawFlag);
	printf("Stack:\n");
	for(s = 0; s < 16; s++) {
		printf("\tstack[%d]: %x\n", s, chip8->stack[s]);
	}
	printf("Stack pointer: %d\n", chip8->stackPointer);
}

void runCycles(struct Chip8* chip8) {
	uchar x = 0;	/* -X-- */
	uchar y = 0;	/* --Y- */
	ushort nnn = 0;	/* -nnn */
	uchar nn = 0;	/* --nn */
	uchar n = 0;	/* ---n */
	uchar event;
	chip8->currentKey = -1; /* -1 if no keys are pressed, otherwise */

	while(chip8->status != STATUS_STOPPED) {
		event = getEvent();
		if(event == EVENT_ESCAPE || event == EVENT_WINDOWCLOSE) {
			chip8->status = STATUS_STOPPED;
			return;
		}
		if(chip8->status == STATUS_PAUSED) {
			drawScreen(chip8);
			continue;
		}
		delay(1);
		if(chip8->delayTimer > 0) chip8->delayTimer--;
		if(chip8->soundTimer > 0) {
			printf("beep");
			chip8->soundTimer--;
		}


		if(chip8->PC > ROM_END_ADDR) {
			printf("Program counter (%04x) reached end of file\n", chip8->PC);
			chip8->status = STATUS_PAUSED;
			continue;
		}

		chip8->opcode = (chip8->memory[chip8->PC] << 8) | (chip8->memory[chip8->PC + 1]);
		chip8->PC += 2;

		x = (chip8->opcode & 0x0F00) >> 8;
		y = (chip8->opcode & 0x00F0) >> 4;

		nnn = chip8->opcode & 0x0FFF;
		nn = chip8->opcode & 0x00FF;
		n = chip8->opcode & 0x000F;
		chip8->drawFlag = 0;
		chip8->currentKey = getKey();

		switch(chip8->opcode & 0xF000) {
			case 0x0000:
				if(chip8->opcode == 0x00E0) {
					if(chip8->printDebug) addrInfo(chip8, "CLS");
					clearDisplay(chip8);
					chip8->drawFlag = 1;
				} else if(chip8->opcode == 0x00EE) {
					if(chip8->printDebug) addrInfo(chip8, "RET");
					chip8->PC = chip8->stack[chip8->stackPointer] + ROM_START_ADDR;
					chip8->stackPointer--;
				} else {
					if(chip8->printDebug) addrInfo(chip8, "SYS %04x", nnn); /* not used for most "modern" ROMs */
				}
				break;
			case 0x1000:
				if((chip8->printDebug)) addrInfo(chip8, "JP %#04x", nnn);
				if(nnn == chip8->PC - 2) {
					if(chip8->printDebug) addrInfo(chip8, "Reached infinite loop in CHIP-8 ROM, pausing exeution.\n");
					chip8->status = STATUS_PAUSED;
					break;
				}
				chip8->PC = nnn;
				break;
			case 0x2000:
				/* increment SP, put PC on top of stack, set to nnn */
				if((chip8->printDebug)) addrInfo(chip8, "CALL %#04x", nnn);
				chip8->stack[chip8->stackPointer] = chip8->PC;
				chip8->stackPointer++;
				chip8->PC = nnn;
				break;
			case 0x3000:
				if((chip8->printDebug)) addrInfo(chip8, "SE V%X, #%x", x, nn);
				if(chip8->V[x] == nn) {
					chip8->PC += 2;
				}
				break;
			case 0x4000:
				if((chip8->printDebug)) addrInfo(chip8, "SNE V%X, #%x", x, nn);
				if(chip8->V[x] != nn) {
					chip8->PC += 2;
				}
				break;
			case 0x5000:
			if((chip8->printDebug)) addrInfo(chip8, "SE V%X, V%X", x, y);
				if(chip8->V[x] != chip8->V[y]) chip8->PC += 2;
				break;
			case 0x6000:
				if((chip8->printDebug)) addrInfo(chip8, "LD V%X, #%x", x, nn);
				chip8->V[x] = nn;
				break;
			case 0x7000:
				if((chip8->printDebug)) addrInfo(chip8, "ADD V%X, #%x", x, nn);
				chip8->V[x] += nn;
				break;
			case 0x8000: {
				if(n == 0x0000) {
					if((chip8->printDebug)) addrInfo(chip8, "LD V%X, V%X", x, y);
					chip8->V[x] = chip8->V[y];
				} else if(n == 0x0001) {
					if((chip8->printDebug)) addrInfo(chip8, "OR V%X, V%X", x, y);
					chip8->V[x] = chip8->V[x] | chip8->V[y];
				} else if(n == 0x0002) {
					if((chip8->printDebug)) addrInfo(chip8, "AND V%X, V%X", x, y);
					chip8->V[x] = chip8->V[x] & chip8->V[y];
				} else if(n == 0x0003) {
					if((chip8->printDebug)) addrInfo(chip8, "XOR V%X, V%X", x, y);
					chip8->V[x] ^= chip8->V[y];
				} else if(n == 0x0004) {
					ushort sum = 0;
					/* Add V[x] and V[y] (lowest 8 bits)
					 * If sum > 255, VF = 1. V%X = sum & 0xF */
					if(chip8->printDebug) addrInfo(chip8, "ADD V%X, V%X", chip8->V[x], chip8->V[y]);
					sum = chip8->V[x] + chip8->V[y];
					chip8->V[0xF] = sum > 255;
					chip8->V[x] = sum & 0xFF;
				} else if(n == 0x0005) {
					/* Set Vx = Vx - Vy, set VF = NOT borrow.
					 * If Vx > Vy, then VF is set to 1, otherwise 0.
					 * Then Vy is subtracted from Vx, and the results stored in Vx. */
					if((chip8->printDebug)) addrInfo(chip8, "SUB V%X, V%X", chip8->V[x], chip8->V[y]);
					chip8->V[0xF] = chip8->V[x] > chip8->V[y];
					chip8->V[x] -= chip8->V[y];
				} else if(n == 0x0006) {
					/* If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
					 * Then Vx is divided by 2. */
					if(chip8->printDebug) addrInfo(chip8, "SHR V%X {, V%X}", chip8->V[x], chip8->V[y]);
					chip8->V[0xF] = (chip8->V[x] & 1);
					chip8->V[x] /= 2;
					/* chip8->V[x] = chip8->V[x] >> 1; */
				} else if(n == 0x0007) {
					/* If Vy > Vx, then VF is set to 1, otherwise 0.
					 * Then Vx is subtracted from Vy, and the results stored in Vx. */
					/* Sets V%X to V%X minus V%X. VF is set to 0 when there's a borrow, and 1 when there isn't */
					if(chip8->printDebug) addrInfo(chip8, "SUBN V%X, V%X", chip8->V[x], chip8->V[y]);
					chip8->V[0xF] = chip8->V[y] > chip8->V[x];
					chip8->V[x] = chip8->V[y] - chip8->V[x];
				} else if(n == 0x000E) {
					/* If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
					 * Then Vx is multiplied by 2.
					 */
					if(chip8->printDebug) addrInfo(chip8, "SHL V%X {, V%X}", chip8->V[x], chip8->V[y]);
					if((chip8->printDebug)) addrInfo(chip8, "Stores the most significant bit of V%X in VF and then shifts V%X to the left by 1", x, x);
					chip8->V[0xF] = (chip8->V[x] & 128);
					chip8->V[x] = chip8->V[x] << 1;
				} else {
					goto unrecognized_opcode;
				}
			}
			break;
			case 0x9000:
				if((chip8->printDebug)) addrInfo(chip8, "SNE V%X, V%X", x, y);
				if(chip8->V[x] != chip8->V[y])
					chip8->PC += 2;
				break;
			case 0xA000:
				if((chip8->printDebug)) addrInfo(chip8, "LD I, %#04x", nnn);
				chip8->I = nnn;
				break;
			case 0xB000:
				if((chip8->printDebug)) addrInfo(chip8, "JP V0, %#04x", nnn);
				chip8->PC = nnn + chip8->V[0];
				break;
			case 0xC000: {
				uchar rnd;
				if((chip8->printDebug)) addrInfo(chip8, "RND V%X, #%x", x, nn);
				rnd = (rand() % 255) + 1;
				chip8->V[x] = rnd & nn;
				break;
			}
			case 0xD000: {
				ushort sx = 0;
				ushort sy = 0;
				uchar pixel;
				ushort offset;
				if ((chip8->printDebug)) addrInfo(chip8, "DRW V%X, V%X, #%x", x, y, n);
				chip8->V[0xF] = 0;

				for(sy = 0; sy < n && (sy+y) < 32; sy++) {
					pixel = chip8->memory[chip8->I+sy];
					for(sx = 0; sx < 8; sx++) {
						if((pixel & (0x80 >> sx)) != 0) {
							offset = (chip8->V[x] + sx + ((chip8->V[y] + sy) * 64));
							if(chip8->screen[offset] == 1) {
								chip8->V[0xF] = 1;
							}
							chip8->screen[offset] ^= 1;
						}
					}
				}
				chip8->drawFlag = 1;
			}
			break;
			case 0xE000:
				if(nn == 0x009E) {
					if((chip8->printDebug)) addrInfo(chip8, "SKP V%X", x);
					if(isPressed(chip8->V[x])) chip8->PC += 2;
				} else if(nn == 0x00A1) {
					if((chip8->printDebug)) addrInfo(chip8, "SKNP V%X", x);
					if(!isPressed(chip8->V[x])) chip8->PC += 2;
				}
				break;
			case 0xF000: {
				if(nn == 0x0007) {
					if((chip8->printDebug)) addrInfo(chip8, "LD V%X, DT", x);
					chip8->V[x] = chip8->delayTimer;
				} else if(nn == 0x000A) {
					int key = 0xFF;
					if((chip8->printDebug)) addrInfo(chip8, "LD V%X, K", x);
					do {
						key = getKey();
						printf("Key: %X\n", key);
					} while(key == 0xFF);
					chip8->V[x] = key;
				} else if(nn == 0x0015) {
					chip8->delayTimer = chip8->V[x];
				} else if(nn == 0x0018) {
					chip8->soundTimer = chip8->V[x];
				} else if(nn == 0x001E) {
					chip8->I += chip8->V[x];
				} else if(nn == 0x0029) {
					if((chip8->printDebug)) addrInfo(chip8, "LD F, V%X", x);
					chip8->I = chip8->V[x] * 5;
				} else if(nn == 0x0033) {
					chip8->memory[chip8->I+0] = (chip8->V[x]/10) % 10;
					chip8->memory[chip8->I+1] = (chip8->V[x]/100) % 10;
					chip8->memory[chip8->I+2] = chip8->V[x] % 10;
				} else if(nn == 0x0055) {
					ushort i;
					if((chip8->printDebug)) addrInfo(chip8, "LD [I], V%X", x);
					for(i = 0; i <= x; i++) {
						chip8->memory[chip8->I + i] = chip8->V[chip8->I + i];
					}
				} else if(nn == 0x0065) {
					ushort i;
					for(i = 0; i < x; i++) {
						chip8->V[chip8->I + i] = chip8->memory[chip8->I + i];
					}
				}
			}
			break;
			default:
				error(chip8, "Unrecognized opcode: %04x at %04x\n", chip8->opcode, chip8->PC);
				goto unrecognized_opcode;
		}
		if(chip8->drawFlag == 1) {
			drawScreen(chip8);
		}
	}
	return;

	unrecognized_opcode:
		fprintf(stderr, "Unrecognized opcode: %04x at %04x\n", chip8->opcode, chip8->PC);
		dumpBytes(chip8->memory, 4096, "dumps/memorybytes_unrecognized.txt");
		printStatus(chip8);
		chip8->status = STATUS_ERROR;
}

void error(struct Chip8* chip8, char* format, ...) {
	va_list args;

	va_start(args, format);
	vprintf(format, args);
	va_end(args);

	dumpBytes(chip8->memory, 4096, "dumps/memorybytes.txt");
	printStatus(chip8);
	chip8->status = STATUS_STOPPED;
}

uchar load(struct Chip8* chip8, char* file) {
	FILE *rom_file;
	ushort i = 0;

	rom_file = fopen(file, "rb");
	if(!rom_file) {
		fprintf(stderr, "Error: couldn\'t load %s\n", file);
		return 1;
	}
	fseek(rom_file, 0L, SEEK_END);
	chip8->romSize = ftell(rom_file);
	if(chip8->romSize == 0) {
		return 1;
	}
	printf("Loading %s (%d bytes)\n", file, chip8->romSize);
	rewind(rom_file);

	chip8->romBytes = (uchar *)malloc(chip8->romSize+1);
	fread(chip8->romBytes, 1, chip8->romSize,rom_file);
	fclose(rom_file);

	for(i = 0; i < 80; i++) {
		chip8->memory[FONT_START_ADDR + i] = font[i];
	}
	for(i = 0; i < chip8->romSize; i++) {
		chip8->memory[ROM_START_ADDR + i] = chip8->romBytes[i];
	}
	return 0;
}
