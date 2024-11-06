

RAYLIB_DIR = ./vendor/raylib-5.0/src
SRC = src/kite.c

all: raylib kite

kite: ${SRC}
	cc -Wall -I${RAYLIB_DIR} -L${RAYLIB_DIR} -o kite ${SRC} -lraylib -lm
	./kite

raylib:
	${MAKE} CUSTOM_CFLAGS="-DSUPPORT_FILEFORMAT_JPG=1" -C ${RAYLIB_DIR}

rl:
	${MAKE} -C ${RAYLIB_DIR}


clean:
	rm -rf kite
	${MAKE} -C ./vendor/raylib-5.0/src clean
