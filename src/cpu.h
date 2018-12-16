#include <stdint.h>

#ifndef CHIP8_H
#define CHIP8_H

#define FONT_START_ADDR 0x050
#define FONT_END_ADDR 0X0A0
#define ROM_START_ADDR 0x200
#define ROM_END_ADDR 0xFFF /* 4096 */
#define CHIP8_MEMORY 4096
/* #define SCHIP8_MEMORY ? */

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

#define CPU_CHIP8 0
#define CPU_SUPERCHIP8 1
#define CPU_MEGACHIP8 2

#define STATUS_STOPPED 0
#define STATUS_RUNNING 1
#define STATUS_PAUSED 2

struct Chip8CPU {
	uchar type; /* chip8, superchip8, megachip8 */
	uchar status;
	uchar* romBytes;
	uchar romSize;
	uchar V[16];		/* registers: VF is used as carry for arithmetic and sprite collisions */
	ushort PC;
	ushort I;			/* address register */
	uchar delayTimer;	/* generally used to make delay loops */
	uchar soundTimer;	/* speaker will beep while non zero */
						/* both timers down-count about 60 times per second when non-zero. */
	uchar drawFlag;		/* if true, run draw function, this will likely be replaced later */

	uchar stack[16];
	uchar stackPointer;
	uchar memory[4096];	/* 4 KB, font located at 0x8110 */
	
	ushort opcode;
	uchar key[16];
	schar currentKey;
	uchar screen[64 * 32]; 
	/* All drawings are done in XOR mode. */
};

uchar font[80];

struct Chip8CPU cpu;

uchar initChip8(char* rom);

void dumpBytes(uchar* bytes, short filesize, char* filename);

void printStatus();

uchar runCycles(uchar printdebug);

void clearDisplay();

void reset();

uchar load(char* file);

#endif
