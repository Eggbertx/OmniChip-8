/*
 * Based on documentation from the CHIP8 emulator by David Winter (HPMANIAC).
 * Several comments are ripped straight from CHIP8.DOC
 * so I donn't have to go back and forth
 */
 
 //VF is used as carry (when using arithmetic instructions) and collision detector (when drawing sprites).
unsigned char registers[16][2] = {
	"V0","V1","V2","V3","V4","V5","V6","V7","V8","V9","VA","VB","VC","VD","VE","VF",
};

typedef unsigned char byte;
typedef unsigned char bool;

// font comes from http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
byte font[80] = { 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


struct Chip8 {
	byte type; // chip8, superchip8, megachip8
	byte running;
	unsigned short PC; // program counter
	unsigned short I; // address register 
	byte delayTimer; // generally used to make delay loops
	byte soundTimer; //speaker will beep while non zero
	// both timers down-count about 60 times per second when non-zero.

	byte stack[16]; // 16 levels, allowing 16 successive subroutine calls
	byte memory[4096];  // 4 KB, font located at 0x8110
	byte key[16]; 
	byte display[64][32]; 
	// All drawings are done in XOR mode. 
	// When one or more pixels are erased while a sprite is drawn, the VF register is set to 01, otherwise 00.
};

typedef struct Chip8 Chip8CPU;

bool init(Chip8CPU *cpu);

bool runCycles(Chip8CPU *cpu);

void draw(Chip8CPU *cpu, byte x, byte y);

unsigned short getOpcode(Chip8CPU *cpu, unsigned short address); 

void togglePause();

void reset();

unsigned char load(char* file);