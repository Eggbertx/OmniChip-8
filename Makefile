CC=cc
LD=ld
CC65=cc65
CA65=ca65
LD65=ld65
EMCC=emcc
TURBOCDIR=C:\TurboC

CFLAGS=-g -pedantic -Wall -std=c89 -D_POSIX_SOURCE
BIN=oc8

sdlio: src/chip8.c src/io_sdl.c src/main.c
	$(CC) -o $(BIN) -DSDL_IO -I/usr/include/SDL2 $(CFLAGS) src/chip8.c src/io_sdl.c src/util.c src/main.c -lSDL2 

cursesio: src/chip8.c src/io_curses.c src/main.c
	$(CC) -o $(BIN) -DCURSES_IO $(CFLAGS) src/chip8.c src/io_curses.c src/util.c src/main.c -lncurses

dos:
	$(error Not supported yet (needs Turbo C))
	# $(TURBOCDIR)\TCC.EXE -I$(TURBOCDIR) -DDOS_IO $(CFLAGS) -o $(BIN) src/chip8.c src/io_dos.c src/util.c src/main.c

c64: src/chip8.c src/io_c64.c src/util.c src/main.c
	$(CC65) -t c64 src/chip8.c
	$(CC65) -t c64 src/io_c64.c
	$(CC65) -t c64 src/util.c
	$(CC65) -t c64 src/main.c
	$(CA65) -t c64 src/chip8.s
	$(CA65) -t c64 src/io_c64.s
	$(CA65) -t c64 src/util.s
	$(CA65) -t c64 src/main.s
	
	$(LD65) -o $(BIN)-c64.prg -t c64 src/chip8.o src/io_c64.o src/util.o src/main.o c64.lib

apple2: src/chip8.c src/io_apple2.c src/main.c
	$(error Not supported yet (also AppleCommander.jar will be needed in order to package the program into an Apple ][ disk image))
	# $(CC65) -t apple2 src/chip8.c
	# $(CC65) -t apple2 src/io_apple2.c
	# $(CC65) -t apple2 src/main.c
	# $(CA65) -t apple2 src/chip8.s
	# $(CA65) -t apple2 src/io_apple2.s
	# $(CA65) -t apple2 src/main.s
	# $(LD65) -o $(BIN)-apple2.prg -t apple2 src/chip8.o src/io_apple2.o src/main.o apple2.lib

emscriptenio:
	$(EMCC) -o $(BIN).html -s --embed-file games USE_SDL=2 --shell-file shell.html -DSDL_IO -DEMSCRIPTEN_IO src/chip8.c src/io_sdl.c src/util.c src/main.c

clean:
	rm -f $(BIN)*
	rm -f src/*.s
	rm -f src/*.o
