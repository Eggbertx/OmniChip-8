#include <stdio.h>
#include "chip8.h"

Chip8CPU cpu;
unsigned short opcode;

bool init(Chip8CPU *cpu) {
	cpu->PC = 0x200;
}

bool runCycles(Chip8CPU *cpu) {
	while(cpu->running) {
		//opcode = getOpcode(cpu, cpu->PC);
		switch(opcode) {
			case 0x01:
				// 
			default:
				printf("Error: Unrecognized opcode.\n");
				
		}
	}
}
