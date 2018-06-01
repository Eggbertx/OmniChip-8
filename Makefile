CC=gcc
CC65=cc65
BIN='oc8'
CFLAGS= -g -no-pie #-Wall

usesdl: src/cpu.c src/io_sdl.c src/main.c
	$(CC) -o $(BIN) -DUSE_SDL $(CFLAGS) src/cpu.c src/io_sdl.c src/main.c -lSDL2 


usecurses: src/cpu.c src/io_curses.c src/main.c
	$(CC) -o $(BIN) -DUSE_CURSES $(CFLAGS) src/cpu.c src/io_curses.c src/main.c -lncurses

dos: 
	@echo "Not implemented yet (needs Turbo C)"

c64: src/cpu.c src/io_c64.c src/main.c
	$(CC65) -t c64 src/cpu.c src/io_c64.c src/main.c
	
	# @echo "Not implemented yet (needs cc65)"

#...

clean:
	rm -f $(BIN)
