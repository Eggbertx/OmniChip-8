#include <stdint.h>

#ifndef CHIP8_H
#define CHIP8_H
/*
 * Based on documentation from the CHIP8 emulator by David Winter (HPMANIAC).
 * Several comments are ripped straight from CHIP8.DOC
 * so I donn't have to go back and forth
 */
#define START_ADDR 0x200

#ifdef __uint8_t_defined
	typedef uint8_t uchar;
	typedef int8_t schar;
	typedef uint16_t ushort;
	typedef int16_t sshort;
#else
	typedef unsigned char uchar;
	typedef signed char schar;
	typedef unsigned short ushort;
	typedef signed short sshort;
#endif

struct Chip8 {
	uchar type; // chip8, superchip8, megachip8
	uchar running;
	uchar *romBytes;
	uchar V[16];		// registers: V[0xF] is used as carry (when using arithmetic instructions) 
						// and collision detector (when drawing sprites).
	ushort PC;			// program counter
	ushort I;			// address register
	uchar delayTimer;	// generally used to make delay loops
	uchar soundTimer;	// speaker will beep while non zero
						// both timers down-count about 60 times per second when non-zero.
	uchar drawFlag;		// if true, run draw function, this will likely be replaced later

	uchar stack[16];	// 16 levels, allowing 16 successive subroutine calls
	uchar stackPointer;
	uchar memory[4096];	// 4 KB, font located at 0x8110
	
	ushort opcode;		// current opcode
	uchar key[16];
	uchar screen[64][32]; 
	
	// All drawings are done in XOR mode. 
	// When one or more pixels are erased while a sprite is drawn, V[F] register is set to 1, otherwise 0.
};


const uchar font[16][5];

typedef struct Chip8 Chip8CPU;

Chip8CPU cpu;

uchar initChip8(char* rom);

void printBytes(uchar* bytes, ushort filesize, uchar* filename);

uchar runCycles();

void clearDisplay();

void draw(unsigned char x, unsigned char y);

void drawChar(unsigned char c, unsigned char x, unsigned char y);

ushort getOpcode(unsigned short address); 

void sendKey(unsigned char key);

void togglePause();



void reset();

uchar load(char* file);

char* decToHex(int dec);

char* hexToDec(int hex);

#endif
