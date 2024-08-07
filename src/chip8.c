#include <stdarg.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "chip8.h"
#include "opcode_funcs.h"
#include "io.h"
#include "util.h"


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

struct Chip8 chip8;

void resetChip8() {
	chip8.status = STATUS_RUNNING;
	chip8.PC = ROM_START_ADDR;
#if !defined(GB_IO) && !defined(TI83_IO) /* because z88dk doesn't appear to have time() or clock() for gb or ti83 */
	srand(time(NULL));
#endif
	memset(chip8.memory, 0, sizeof(chip8.memory));
	memset(chip8.stack, 0, sizeof(chip8.stack));
	memset(chip8.V, 0, sizeof(chip8.V));
	memset(chip8.key, 0, sizeof(chip8.key));
	chip8.stackPointer = 0;
	chip8.I = 0;
	chip8.delayTimer = 0;
	chip8.soundTimer = 0;
	chip8.currentKey = -1;
}

uchar initChip8() {
	resetChip8();
	return loadROM(chip8.romPath);
}

void _OC8_FASTCALL drawScreen() {
	register uchar x = 0;
	register uchar y = 0;
	for(y = 0; y < 32; y++) {
		for(x = 0; x < 64; x++) {
			if(chip8.screen[x+y*64] != 0) {
				drawPixel(x, y);
			}
		}
	}

	flipScreen();
}

void dumpBytes(uchar* bytes, short filesize, char* filename) {
	ushort i;
#if defined(SDL_IO) || defined(CURSES_IO)
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
#endif
}

void printStatus() {
#ifdef PRINT_DEBUG
	uchar r = 0;
	uchar s = 0;
	printf("V registers:\n");
	for(r = 0; r < 16; r++) {
		printf("\tV%X: %x\n", r, chip8.V[r]);
	}
	printf("Program counter: %d\n", chip8.PC);
	printf("Address register (I): %d\n", chip8.I);
	printf("Delay timer: %d\nSound timer: %d\n", chip8.delayTimer, chip8.soundTimer);
	printf("chip8.drawFlag: %d\n", chip8.drawFlag);
	printf("Stack:\n");
	for(s = 0; s < 16; s++) {
		printf("\tstack[%d]: %x\n", s, chip8.stack[s]);
	}
	printf("Stack pointer: %d\n", chip8.stackPointer);
#endif
}


void _OC8_FASTCALL doCycle() {
	uchar x = 0;	/* -X-- */
	uchar y = 0;	/* --Y- */
	ushort nnn = 0;	/* -nnn */
	uchar nn = 0;	/* --nn */
	uchar n = 0;	/* ---n */
	uchar event;
	chip8.currentKey = -1; /* -1 if no keys are pressed, otherwise */

	delay(1);
	if(chip8.delayTimer > 0) chip8.delayTimer--;
	if(chip8.soundTimer > 0) {
	#ifdef PRINT_DEBUG
		printf("beep");
	#endif
		chip8.soundTimer--;
	}


	if(chip8.PC > ROM_END_ADDR) {
	#ifdef SDL_IO
		printf("Program counter (%04x) reached end of file\n", chip8.PC);
	#endif
		chip8.status = STATUS_PAUSED;
		return;
	}

	chip8.opcode = OPCODE();
	chip8.PC += 2;

	x = OPCODE_X();
	y = OPCODE_Y();
	nnn = OPCODE_NNN();
	nn = OPCODE_NN();
	n = OPCODE_N();
	chip8.drawFlag = 0;
	chip8.currentKey = getKey();

	switch(chip8.opcode & 0xF000) {
		case 0x0000:
			if(chip8.opcode == 0x0000) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "SYS %04x", nnn); /* not used for most "modern" ROMs */
				#endif
			} else if(chip8.opcode == 0x00E0) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "CLS");
				#endif
				clearDisplay();
				chip8.drawFlag = 1;
			} else if(chip8.opcode == 0x00EE) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "RET");
				#endif
				chip8.PC = chip8.stack[chip8.stackPointer] + ROM_START_ADDR + 2;
				chip8.stackPointer--;
			} else {
				goto unrecognized_opcode;
			}
			break;
		case 0x1000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "JP %#04x", nnn);
			#endif
			if(nnn == chip8.PC - 2) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "Reached infinite loop in CHIP-8 ROM, pausing exeution.\n");
				#endif
				chip8.status = STATUS_PAUSED;
				break;
			}
			chip8.PC = nnn;
			break;
		case 0x2000:
			/* increment SP, put PC on top of stack, set to nnn */
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "CALL %#04x", nnn);
			#endif
			chip8.stack[chip8.stackPointer] = chip8.PC;
			chip8.stackPointer++;
			chip8.PC = nnn;
			break;
		case 0x3000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "SE V%X, #%x", x, nn);
			#endif
			if(chip8.V[x] == nn) {
				chip8.PC += 2;
			}
			break;
		case 0x4000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "SNE V%X, #%x", x, nn);
			#endif
			if(chip8.V[x] != nn) {
				chip8.PC += 2;
			}
			break;
		case 0x5000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "SE V%X, V%X", x, y);
			#endif
			if(chip8.V[x] == chip8.V[y]) {
				chip8.PC += 2;
			}
			break;
		case 0x6000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "LD V%X, #%x", x, nn);
			#endif
			chip8.V[x] = nn;
			break;
		case 0x7000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "ADD V%X, #%x", x, nn);
			#endif
			chip8.V[x] += nn;
			break;
		case 0x8000: {
			if(n == 0x0000) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "LD V%X, V%X", x, y);
				#endif
				chip8.V[x] = chip8.V[y];
			} else if(n == 0x0001) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "OR V%X, V%X", x, y);
				#endif
				chip8.V[x] = chip8.V[x] | chip8.V[y];
			} else if(n == 0x0002) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "AND V%X, V%X", x, y);
				#endif
				chip8.V[x] = chip8.V[x] & chip8.V[y];
			} else if(n == 0x0003) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "XOR V%X, V%X", x, y);
				#endif
				chip8.V[x] ^= chip8.V[y];
			} else if(n == 0x0004) {
				ushort sum = 0;
				/*
				 * Add V[x] and V[y] (lowest 8 bits)
				 * If sum > 255, VF = 1. V%X = sum & 0xF
				 */
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "ADD V%X, V%X", x, y);
				#endif
				sum = chip8.V[x] + chip8.V[y];
				chip8.V[0xF] = sum > 255;
				chip8.V[x] = sum & 0xFF;
			} else if(n == 0x0005) {
				/*
				 * Set Vx = Vx - Vy, set VF = NOT borrow.
				 * If Vx > Vy, then VF is set to 1, otherwise 0.
				 * Then Vy is subtracted from Vx, and the results stored in Vx.
				 */
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "SUB V%X, V%X", chip8.V[x], chip8.V[y]);
				#endif
				chip8.V[0xF] = chip8.V[x] > chip8.V[y];
				chip8.V[x] -= chip8.V[y];
			} else if(n == 0x0006) {
				/*
				 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
				 * Then Vx is divided by 2.
				 */
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "SHR V%X {, V%X}", chip8.V[x], chip8.V[y]);
				#endif
				chip8.V[0xF] = (chip8.V[x] & 1);
				chip8.V[x] /= 2;
				/* chip8.V[x] = chip8.V[x] >> 1; */
			} else if(n == 0x0007) {
				/*
				 * If Vy > Vx, then VF is set to 1, otherwise 0.
				 * Then Vx is subtracted from Vy, and the results stored in Vx.
				 * Sets V%X to V%X minus V%X. VF is set to 0 when there's a borrow,
				 * and 1 when there isn't
				 */
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "SUBN V%X, V%X", chip8.V[x], chip8.V[y]);
				#endif
				chip8.V[0xF] = chip8.V[y] > chip8.V[x];
				chip8.V[x] = chip8.V[y] - chip8.V[x];
			} else if(n == 0x000E) {
				/*
				 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
				 * Then Vx is multiplied by 2.
				 */
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "SHL V%X {, V%X}", chip8.V[x], chip8.V[y]);
					/* addrInfo(chip8, "Stores the most significant bit of V%X in VF and then shifts V%X to the left by 1", x, x); */
				#endif
				chip8.V[0xF] = (chip8.V[x] & 128);
				chip8.V[x] = chip8.V[x] << 1;
			} else {
				goto unrecognized_opcode;
			}
		}
		break;
		case 0x9000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "SNE V%X, V%X", x, y);
			#endif
			if(chip8.V[x] != chip8.V[y])
				chip8.PC += 2;
			break;
		case 0xA000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "LD I, %#04x", nnn);
			#endif
			chip8.I = nnn;
			break;
		case 0xB000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "JP V0, %#04x", nnn);
			#endif
			chip8.PC = nnn + chip8.V[0];
			break;
		case 0xC000: {
			uchar rnd;
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "RND V%X, #%x", x, nn);
			#endif
			rnd = (rand() % 255) + 1;
			chip8.V[x] = rnd & nn;
			break;
		}
		case 0xD000:
			#ifdef PRINT_OPCODES
				addrInfo(chip8, "DRW %X, %X, %X", x, y, n);
			#endif
			drawSprite(x, y, n);
			break;
		case 0xE000:
			if(nn == 0x009E) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "SKP V%X", x);
				#endif
				if(isPressed(chip8.V[x])) chip8.PC += 2;
			} else if(nn == 0x00A1) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "SKNP V%X", x);
				#endif
				if(!isPressed(chip8.V[x])) chip8.PC += 2;
			}
			break;
		case 0xF000: {
			if(nn == 0x0007) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "LD V%X, DT", x);
				#endif
				chip8.V[x] = chip8.delayTimer;
			} else if(nn == 0x000A) {
				int key = 0xFF;
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "LD V%X, K", x);
				#endif
				do {
					key = getKey();
				} while(key == 0xFF);
				chip8.V[x] = key;
			} else if(nn == 0x0015) {
				chip8.delayTimer = chip8.V[x];
			} else if(nn == 0x0018) {
				chip8.soundTimer = chip8.V[x];
			} else if(nn == 0x001E) {
				chip8.I += chip8.V[x];
			} else if(nn == 0x0029) {
				#ifdef PRINT_OPCODES
					addrInfo(chip8, "LD F, V%X", x);
				#endif
				chip8.I = chip8.V[x] * 5;
			} else if(nn == 0x0033) {
				chip8.memory[chip8.I+0] = chip8.V[x] / 100;
				chip8.memory[chip8.I+1] = (chip8.V[x] / 10) % 10;
				chip8.memory[chip8.I+2] = chip8.V[x] % 10;
			} else if(nn == 0x0055) {
				ushort i;
				#ifdef PRINT_OPCODES
				addrInfo(chip8, "LD [I], V%X", x);
				#endif
				for(i = 0; i <= x; i++) {
					chip8.memory[chip8.I + i] = chip8.V[i];
				}
			} else if(nn == 0x0065) {
				ushort i;
				for(i = 0; i <= x; i++) {
					chip8.V[i] = chip8.memory[chip8.I + i];
				}
			}
		}
		break;
		default:
			goto unrecognized_opcode;
	}
	
	return;

	unrecognized_opcode:
		dumpBytes(chip8.memory, 4096, "dumps/memorybytes_unrecognized.txt");
	#ifdef PRINT_DEBUG
		printf("Unrecognized opcode: %04x at %04x\n", chip8.opcode, chip8.PC);
		printStatus(chip8);
	#endif
		chip8.status = STATUS_ERROR;
}
