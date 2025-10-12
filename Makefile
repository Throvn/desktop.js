run: main
	DYLD_LIBRARY_PATH=build \
	./djs-aarch64-macos run ./.sandbox/javascript/index.js

debug: main
	DYLD_LIBRARY_PATH=build \
	# lldb djs-aarch64-macos
	leaks --list --atExit -- ./djs-aarch64-macos run ./.sandbox/javascript/index.js

SOURCE_FILES = source/debug.c \
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

LIBRARY_FILES = lib/txiki.js/build/deps/quickjs/libqjs.a \
				lib/txiki.js/build/deps/wasm3/source/libm3.a \
				lib/txiki.js/build/deps/libuv/libuv.a \
				lib/txiki.js/build/deps/sqlite3/libsqlite3.a \
				lib/txiki.js/build/deps/mimalloc/libmimalloc.a \
				lib/txiki.js/build/libtjs.a \
				lib/raylib/raylib/libraylib.a

main: $(LIBRARY_FILES) $(SOURCE_FILES)
	clang -g -fsanitize=address $(CFLAGS) -O0 -rpath @executable_path/build $^ -o djs-aarch64-macos -Ilib/raylib/raylib/include -Ilib/txiki.js/deps/quickjs -Ilib/txiki.js/src -Ilib/txiki.js/deps/libuv/include -lffi -lcurl -framework IOKit -framework Cocoa



minified: $(LIBRARY_FILES) $(SOURCE_FILES)
	zig cc -O4 -Wall -rpath @executable_path/build $^ -o djs-aarch64-macos-mini -Ilib/raylib/raylib/include -Ilib/txiki.js/deps/quickjs -Ilib/txiki.js/src -Ilib/txiki.js/deps/libuv/include -lffi -lcurl -framework IOKit -framework Cocoa

lib/quickjs/libquickjs.a:
	cd lib/quickjs/ && $(MAKE)
