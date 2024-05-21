#include <string.h>

#include "chip8.h"
#include "io.h"
#include "opcode_funcs.h"

void _OC8_FASTCALL drawSprite(uchar x, uchar y, uchar height) {
	uchar sx;
	uchar sy;
	uchar pixel;
	ushort offset;
	for(sy = 0; sy < height && (sy+y) < 32; sy++) {
		pixel = chip8.memory[chip8.I+sy];
		for(sx = 0; sx < 8; sx++) {
			if((pixel & (0x80 >> sx)) != 0) {
				offset = (chip8.V[x] + sx + ((chip8.V[y] + sy) * 64));
				if(chip8.screen[offset] == 1) {
					chip8.V[0xF] = 1;
				}
				chip8.screen[offset] ^= 1;
			}
		}
	}
	chip8.drawFlag = 1;
}

void clearDisplay() {
	memset(chip8.screen, 0, 64*32);
	clearScreen();
}