OmniChip-8
=======
A Chip-8 emulator with heavy emphasis on being as cross-platform as humanly possible

Building instructions
-----
### Desktop (SDL)
Run `IO=sdl make` or just `make` (SDL is the default IO mode)
### Desktop (curses, UNIX-like OSs)
Run `IO=curses make`
### Commodore 64
Run `IO=c64 make build-cc65`
### GameBoy
Run `IO=gb make build-gb`


Supported platforms
------ 
### Partial
 * Desktop SDL
 * (n)curses
 * Commodore 64
 * GameBoy

### Planned
 * Apple ][
 * NES
 * MS-DOS/FreeDOS/DR-DOS
 * TI-8x
 * ?

## Screenshot
![Screenshots](screenshot.png)