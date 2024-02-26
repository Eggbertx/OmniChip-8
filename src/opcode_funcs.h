#ifndef OPCODE_FUNCS_H
#define OPCODE_FUNCS_H

#include "chip8.h"

void drawSprite(struct Chip8* chip8, uchar x, uchar y, uchar n);

void clearDisplay(struct Chip8* chip8);

#endif