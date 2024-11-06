.SILENT:

# Directory where raylib source code is located
RAYLIB_DIR=./vendor/raylib-5.0/src

# Compiler flags
CFLAGS=-Wall -Wextra -ggdb
INCLUDE_FLAGS=-I${RAYLIB_DIR}
LIB_FLAGS=-L${RAYLIB_DIR} -lraylib -lm
BUILD_DIR=./bin

SRC_DIR=./src

# Target rules
all: raylib main

# Build the shared library for app
app: ${SRC_DIR}/app.c
	mkdir -p ${BUILD_DIR}
	gcc ${CFLAGS} -o ${BUILD_DIR}/libapp.so -fPIC -shared ${INCLUDE_FLAGS} ${LIB_FLAGS} ${SRC_DIR}/app.c

# Build main application
main: ${SRC_DIR}/main.c
	mkdir -p ${BUILD_DIR}
ifdef HOT
	# Build app as a shared library for hot reloading
	${MAKE} app
	# Compile main with HOTRELOAD flag and run it with libapp.so in LD_LIBRARY_PATH
	${MAKE} raylib
	gcc ${CFLAGS} -DHOTRELOAD ${INCLUDE_FLAGS} ${LIB_FLAGS} -o ${BUILD_DIR}/app ${SRC_DIR}/main.c
	LD_LIBRARY_PATH="${RAYLIB_DIR}:${BUILD_DIR}:$LD_LIBRARY_PATH" ${BUILD_DIR}/app
else
	# Clean up existing shared libraries if not in hot-reload mode
	[ -f ${BUILD_DIR}/libapp.so ] && rm ${BUILD_DIR}/libapp.so || echo ""
	# Compile statically linking raylib.a
	gcc ${CFLAGS} ${INCLUDE_FLAGS} -o ${BUILD_DIR}/app ${SRC_DIR}/main.c ${SRC_DIR}/app.c ${RAYLIB_DIR}/libraylib.a -lm
endif

# Build raylib library
raylib:
ifdef HOT
	# Build raylib as a shared library
	${MAKE} RAYLIB_LIBTYPE=SHARED -C ${RAYLIB_DIR}
else
	# Build raylib as a static library
	${MAKE} -C ${RAYLIB_DIR}
endif

# Clean build directory and raylib artifacts
clean:
	rm -rf ${BUILD_DIR}
	${MAKE} -C ${RAYLIB_DIR} clean
	mkdir -p ${BUILD_DIR}



# Hot-reload target: rebuilds the app and waits for further input
hot:
	# Rebuild the app's shared library and then run it
	${MAKE} app
	# Wait for input, rebuild, and rerun the app on Enter
	while true; do \
		read -p "Press Enter to rebuild and rerun the app, or type 'exit' to quit: " input; \
		if [ "$$input" = "exit" ]; then \
			echo "Exiting hot-reload loop..."; \
			break; \
		fi; \
		${MAKE} app; \
	done
