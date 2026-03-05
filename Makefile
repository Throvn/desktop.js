run: main
	DYLD_LIBRARY_PATH=build \
	./djs-aarch64-macos run ./.sandbox/javascript/index.js

debug: main
	DYLD_LIBRARY_PATH=build \
	# lldb djs-aarch64-macos
	leaks --list --atExit -- ./djs-aarch64-macos run ./.sandbox/javascript/index.js

SOURCE_FILES = source/debug.c \
			source/gui/fonts.c \
			source/gui/blob.c \
			source/gui/memory.c \
			source/gui/colors.c \
			source/gui/js.c \
			source/gui/styles.c \
			source/renderer/reconcile.c \
			source/gui/draw.c \
			source/events/events.c \
			source/events/mouse.c \
			source/events/keyboard.c \
			source/platform.c \
			source/main.c

LIBRARY_FILES = lib/txiki.js/libtjs.a \
				lib/txiki.js/deps/quickjs/libqjs.a \
				lib/txiki.js/deps/wasm3/source/libm3.a \
				lib/txiki.js/deps/libuv/libuv.a \
				lib/txiki.js/deps/sqlite3/libsqlite3.a \
				lib/txiki.js/deps/mimalloc/libmimalloc.a \
				lib/raylib/raylib/libraylib.a


UNAME_S := $(shell uname -s)
ARCH := $(shell uname -m)

TARGET := djs-$(ARCH)-$(shell echo $(UNAME_S) | tr A-Z a-z)

ifeq ($(UNAME_S),Darwin)
    OS_FLAGS = -rpath @executable_path/build -framework IOKit -framework Cocoa -lffi -lcurl
else
    # Fixed the elif syntax and ensured curl/ffi are ready for Linux
    OS_FLAGS = -lffi -lcurl -lpthread -Wl,-rpath,\$$ORIGIN/build
endif

CFLAGS = -Ilib/raylib/raylib/include \
		 -Ilib/txiki.js/deps/quickjs \
		 -Ilib/txiki.js/src \
		 -Ilib/txiki.js/deps/libuv/include


OBJS = $(SOURCE_FILES:.c=.o)
%.o: %.c
	clang -g -fsanitize=address -O0 $(CFLAGS) -c $< -o $@

main: $(OBJS) $(LIBRARY_FILES)
	clang -g -fsanitize=address -O0 $(CFLAGS) -o $(TARGET) \
        $(OBJS) \
        $(LIBRARY_FILES) \
        $(OS_FLAGS)

minified: $(LIBRARY_FILES) $(SOURCE_FILES)
	zig cc -O4 -Wall $^ -o $(TARGET)-mini -Ilib/raylib/raylib/include -Ilib/txiki.js/deps/quickjs -Ilib/txiki.js/src -Ilib/txiki.js/deps/libuv/include -lffi -lcurl $(OS_FLAGS)

lib/txiki.js/libtjs.a:
	cd lib/txiki.js/ && cmake . && make

lib/raylib/raylib/libraylib.a:
	cd lib/raylib && cmake . && make

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET)-mini