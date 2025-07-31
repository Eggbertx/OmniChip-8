# OmniChip-8
![9 tests passed](https://camo.githubusercontent.com/0d2dde873dda4d7bad1ee1f669b215cb425fb15baf6c21e13bf0180709edbd77/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f74657374732d392532307061737365642d73756363657373)

OmniChip-8 is a Chip-8 emulator with the goal of making it as cross-platform as possible, with as little platform-specific assembly code as possible.

# Building instructions
## Desktop (SDL)
Run `python make.py`. In Windows, it is able to use Visual Studio (via the msbuild command) or mingw.

### Screenshot
![SDL screenshot](./screenshots/sdl.png)

### Using Visual Studio
Visual Studio should automatically download the SDL2 nuget packages when you build the project.

### Using msbuild (Developer Command Prompt for VS 20XX)
If you have [NuGet](https://www.nuget.org/) installed, you can run `nuget restore` to install the SDL2 nuget dependency packages. Otherwise, you will either need to install NuGet or open OmniChip-8.sln in Visual Studio and build it once to have it download the packages.

After you have them installed, you can run `python make.py build`

### Using mingw
Run `pacman -S base-devel mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2` before using make.py.


## Desktop (curses, UNIX-like OSs)
Run `python make.py curses`

### Screenshot
![ncurses screenshot](./screenshots/curses.png)


## Commodore 64 (requires cc65)
Run `python make.py c64`
To choose a specific game run `python make.py c64 --embed <path>`.
To build a .d64 disk image, run `python make.py c64 -d` or `python make.py c64 --disk --embed <path>`. You can use a disk editor like V1541Commander to add other CHIP-8 games to the disk image.

If the disk flag is set, OmniChip-8 will ask for the game name to run when it starts. Otherwise, it will embed the game specified by the `--embed` flag, or default to `games/omnichip8`.

### Screenshot
![Commodore 64 screenshot](./screenshots/c64.png)


## GameBoy (requires z88dk)
Run `python make.py gb`. To choose a specific game run `python make.py gb --embed <path>`.

### Screenshot
![GameBoy screenshot](./screenshots/gb.png)


## sim65 (requires cc65)
Run `python make.py sim6502`


# Testing
To test OmniChip-8, run `./make.py test`. This will build and run the test suites, and generate a test coverage report which you can view in a browser. This requires cmake, ctest, and lcov.

If you want to just run the tests without generating a coverage report (removing the need for lcov), run `./make test --no-coverage`.


# Supported platforms
## Mostly done
 * Desktop SDL
 * WebAssembly
 * (n)curses
 * Commodore 64
 * GameBoy
 * sim65

## Planned
 * TI-8x
 * Apple II
 * DOS

## Maybe/hopefully
 * NES
 * Sphere, via WebAssembly/Emscripten
 * Magic-1
 * Bare metal x86


# Credit
All of the games in `games/` are public domain, and came from [here](https://www.zophar.net/pdroms/chip8/chip-8-games-pack.html) except for omnichip8, oc8, and their respective .c8 sources. Some of the opcode comments in src/chip8.c come from [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#font)