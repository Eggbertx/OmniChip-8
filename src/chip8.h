#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define FONT_START_ADDR 0x050
#define FONT_END_ADDR 0X0A0
#define ROM_START_ADDR 0x200
#define ROM_END_ADDR 0xFFF /* 4096 */
#define CHIP8_MEMORY 4096
#define SCREEN_MEMORY 64 * 32


#if defined(UINT8_MAX)
	typedef uint8_t uchar;
	typedef int8_t schar;
	typedef uint16_t ushort;
	typedef int16_t sshort;
#elif defined(INT_LEAST8_MAX)
	typedef uint_least8_t uchar;
	typedef int_least8_t schar;
	typedef uint_least16_t ushort;
	typedef int_least16_t sshort;
#else
	typedef unsigned char uchar;
	typedef signed char schar;
	typedef unsigned short ushort;
	typedef signed short sshort;
#endif

#define STATUS_STOPPED 0
#define STATUS_RUNNING 1
#define STATUS_PAUSED 2
#define STATUS_ERROR 4

#define OPCODE() (chip8.memory[chip8.PC] << 8) | (chip8.memory[chip8.PC + 1])
#define OPCODE_INSTRUCTION() (chip8.opcode & 0xF000) >> 12
#define OPCODE_X() (chip8.opcode & 0x0F00) >> 8
#define OPCODE_Y() (chip8.opcode & 0x00F0) >> 4
#define OPCODE_NNN() chip8.opcode & 0x0FFF
#define OPCODE_NN() chip8.opcode & 0xFF
#define OPCODE_N() chip8.opcode & 0xF

#ifdef __CC65__
#define _OC8_FASTCALL __fastcall__
#else
#define _OC8_FASTCALL
#endif

struct Chip8 {
	uchar status;
	char* romPath;
	uchar* romBytes;
	ushort romSize;
	uchar V[16];		/* registers: VF is used as carry for arithmetic and sprite collisions */
	ushort PC;
	ushort I;			/* address register */
	uchar delayTimer;	/* generally used to make delay loops */
	uchar soundTimer;	/* speaker will beep while non zero */
						/* both timers down-count about 60 times per second when non-zero. */
	uchar drawFlag;		/* if true, run draw function, this will likely be replaced later */

	ushort stack[16];
	uchar stackPointer;
	uchar memory[4096];	/* 4 KB, font located at 0x8110 */
	
	ushort opcode;
	uchar key[16];
	schar currentKey;
	uchar screen[SCREEN_MEMORY]; /* All drawings are done in XOR mode. */
};
extern struct Chip8 chip8;

extern uchar font[80];

extern char currentOpcode[20];

uchar initChip8();

void printStatus();

void _OC8_FASTCALL doCycle();

void _OC8_FASTCALL drawScreen();

void resetChip8();

#ifdef DEBUG_KEYS

void togglePause();

void stepIn();

void stepOut();

#endif

#endif
