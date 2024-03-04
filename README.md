OmniChip-8
=======
[![8 tests passed](https://camo.githubusercontent.com/bdd31bf1696c33215025e20012715cce1717bb089ec88cb3f3084cb1e797d745/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f74657374732d382532307061737365642d73756363657373)](https://github.com/Eggbertx/OmniChip-8/actions/runs/8145050872/job/22260484280)

A Chip-8 emulator with heavy emphasis on being as cross-platform as humanly possible

Building instructions
-----
## Desktop (SDL)
Run `python make.py`. In Windows, it is able to use Visual Studio (via the msbuild command) or mingw.
### Visual Studio
Visual Studio should automatically download the SDL2 nuget packages when you build the project.
### msbuild (Developer Command Prompt for VS 20XX)
If you have [NuGet](https://www.nuget.org/) installed, you can run `nuget restore` to install the SDL2 nuget dependency packages. Otherwise, you will either need to install NuGet or open OmniChip-8.sln in Visual Studio and build it once to have it download the packages.

After you have them installed, you can run `python make.py build`
### mingw
Run `pacman -S base-devel mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2` before using make.py.

## Desktop (curses, UNIX-like OSs)
Run `python make.py curses`
## Commodore 64 (requires cc65)
Run `python make.py c64`
## sim65 (requires cc65)
Run `python make.py sim6502`
## GameBoy (requires z88dk)
Run `python make.py gb`

Testing
------
To test OmniChip-8, run the following commands
```Shell
cmake -B build
cd build
make && make test
```
To generate a coverage report, run `make lcov` from the build directory after you have run the above commands (including `make test`). This requires [lcov](https://github.com//linux-test-project/lcov) to be installed. If no errors are returned, the report will be generated in the build/lcov/ directory.


Supported platforms
------ 
## Mostly done
 * Desktop SDL

![SDL screenshot](./screenshots/sdl.png)
 * (n)curses

![ncurses screenshot](./screenshots/curses.png)
 * Commodore 64

![Commodore 64 screenshot](./screenshots/c64.png)
 * GameBoy

![Gameboy screenshot](./screenshots/gb.png)

 * sim65


## Planned
 * WebAssembly + SDL
 * TI-8x
 * Apple II
 * NES
 * Sphere, via WebAssembly/Emscripten
 * MS-DOS/FreeDOS/DR-DOS

## Maybe/hopefully
 * Magic-1
 * Bare metal x86


Games
------
All of the games in `games/` are public domain, and came from [here](https://www.zophar.net/pdroms/chip8/chip-8-games-pack.html) except for omnichip8, oc8, and their respective .c8 sources.
