CC=cc
LD=ld
CC65=cc65
CA65=ca65
LD65=ld65
EMCC=emcc
TURBOCDIR=C:\TurboC

CFLAGS=-g
BIN=oc8

sdlio: src/cpu.c src/io_sdl.c src/main.c
	$(CC) -o $(BIN) -DSDL_IO -I/usr/include/SDL2 $(CFLAGS) src/cpu.c src/io_sdl.c src/main.c -lSDL2 

cursesio: src/cpu.c src/io_curses.c src/main.c
	$(CC) -o $(BIN) -DCURSES_IO $(CFLAGS) src/cpu.c src/io_curses.c src/main.c -lncurses

dos:
	@echo "Not supported yet (needs Turbo C)"
	# $(TURBOCDIR)\TCC.EXE -I$(TURBOCDIR) -DDOS_IO $(CFLAGS) -o $(BIN) src/cpu.c src/io_dos.c src/main.c

c64: src/cpu.c src/io_c64.c src/main.c
	$(CC65) -t c64 src/cpu.c src/io_c64.c src/main.c
	$(CA65) -t c64 src/cpu.s src/io_c64.s src/main.s
	$(LD65) -o $(BIN)-c64.prg -t c64 src/cpu.o src/io_c64.o src/main.o c64.lib

emscripten:
	$(EMCC) -o $(BIN).html -s USE_SDL=2 $(CFLAGS) src/cpu.c src/io_sdl.c src/main.c

clean:
	rm -f $(BIN)
