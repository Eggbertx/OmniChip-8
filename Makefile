CC=cc
LD=ld
CL65=cl65
EMCC=emcc
ZCC=zcc
TURBOCDIR=C:\TurboC
ifeq (${IO},)
	IO=sdl
endif
IO_CONST=$(shell echo ${IO} | tr a-z A-Z)
ifeq (${IO},sdl)
	INC_DIRS=-I/usr/include/SDL2
	LIB=-lSDL2
else ifeq (${IO},curses)
	LIB=-lcurses
endif

SOURCES=src/chip8.c src/io_${IO}.c src/util.c src/main.c
CFLAGS=-g -pedantic -Wall -std=c89 -D_POSIX_SOURCE
BIN=oc8

build-native:
	${CC} -o ${BIN} -D${IO_CONST}_IO ${INC_DIRS} ${CFLAGS} ${SOURCES} ${LIB}

build-dos:
	$(error Not supported yet (needs Turbo C))

build-cc65:
	${CL65} -o ${BIN}-${IO}.prg -t ${IO} ${SOURCES}

build-gb:
	${ZCC} +gb -create-app -o ${BIN}.bin -DGB_IO ${SOURCES}

build-emscripten:
	${EMCC} -o ${BIN}.html -s --embed-file games USE_SDL=2 --shell-file shell.html -DSDL_IO -DEMSCRIPTEN_IO ${SOURCES}

clean:
	rm -f $(BIN)*
	rm -f src/*.s
	rm -f src/*.o
