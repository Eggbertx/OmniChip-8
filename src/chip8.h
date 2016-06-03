#ifndef CHIP8_H
#define CHIP8_H
/*
 * Based on documentation from the CHIP8 emulator by David Winter (HPMANIAC).
 * Several comments are ripped straight from CHIP8.DOC
 * so I donn't have to go back and forth
 */

 struct Chip8 {
	unsigned char type; // chip8, superchip8, megachip8
	unsigned char running;
	unsigned char *romBytes;
	unsigned short PC; // program counter
	unsigned short I; // address register 
	unsigned char delayTimer; // generally used to make delay loops
	unsigned char soundTimer; //speaker will beep while non zero
	// both timers down-count about 60 times per second when non-zero.

	unsigned char stack[16]; // 16 levels, allowing 16 successive subroutine calls
	unsigned char memory[4096];  // 4 KB, font located at 0x8110
	unsigned char key[16]; 
	unsigned char display[64][32]; 
	
	// All drawings are done in XOR mode. 
	// When one or more pixels are erased while a sprite is drawn, the VF register is set to 01, otherwise 00.
};

// registers: VF is used as carry (when using arithmetic instructions) 
// and collision detector (when drawing sprites).
unsigned char V[16];

unsigned char font[16][5];


typedef struct Chip8 Chip8CPU;

Chip8CPU cpu;
unsigned short opcode;

extern unsigned char initChip8(char* rom);

extern unsigned char runCycles();

extern void draw(unsigned char x, unsigned char y);

extern void drawChar(unsigned char c, unsigned char x, unsigned char y);

extern unsigned short getOpcode(unsigned short address); 

extern void togglePause();

extern void reset();

extern unsigned char load(char* file);

#endif
