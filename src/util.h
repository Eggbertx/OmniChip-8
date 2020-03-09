#ifndef UTIL_H
#define UTIL_H
#include "chip8.h"

uchar loadROM(struct Chip8* chip8, char* file);

int oc8log(const char* format, ...);

#endif