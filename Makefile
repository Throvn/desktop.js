run: main
	DYLD_LIBRARY_PATH=build \
	./djs-aarch64-macos run ./.internals/javascript/index.js

DEBUGGING := -g

debug: main
	DYLD_LIBRARY_PATH=build \
	# lldb djs-aarch64-macos
	leaks --list --atExit -- ./djs-aarch64-macos run ./.internals/javascript/index.js

main:lib/txiki.js/build/deps/quickjs/libqjs.a lib/txiki.js/build/deps/wasm3/source/libm3.a lib/txiki.js/build/deps/libuv/libuv.a lib/txiki.js/build/deps/sqlite3/libsqlite3.a lib/txiki.js/build/deps/mimalloc/libmimalloc.a lib/txiki.js/build/libtjs.a source/gui/colors.c source/gui/js.c source/gui/styles.c source/gui/draw.c source/events/mouse.c source/main.c lib/raylib/raylib/libraylib.a 
	zig cc $(DEBUGGING) -Wall -rpath @executable_path/build $^ -o djs-aarch64-macos -Ilib/raylib/raylib/include -Ilib/quickjs -Ilib/txiki.js/src -lffi -lcurl -lpthread -framework IOKit -framework Cocoa

minified: source/main.c source/engine.c source/gui.c source/draw.c source/colors.c source/styles.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	zig cc -O4 -Wall -rpath @executable_path/build $^ -o djs-mini -I../lib/quickjs -I../lib/raylib/raylib/include -I../lib/minnet-quickjs -framework IOKit -framework Cocoa

lib/quickjs/libquickjs.a:
	cd lib/quickjs/ && $(MAKE)
