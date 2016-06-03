CC=gcc
BIN='oc8'

usesdl: src/chip8.c src/io_sdl.c src/main.c
	$(CC) -o $(BIN) -DUSE_SDL src/chip8.c src/io_sdl.c src/main.c -lSDL2 

usencurses: src/chip8.c src/io_ncurses.c src/main.c
	$(CC) -o $(BIN) -DUSE_NCURSES src/chip8.c src/io_ncurses.c src/main.c -lncurses

dos: 
	@echo "Not implemented yet (needs Turbo C)"
c64:
	@echo "Not implemented yet (needs cc65)"

solaris:
	@echo "Not implemented yet"

#...

clean:
	rm -f $(BIN)
