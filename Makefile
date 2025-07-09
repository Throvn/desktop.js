run: main
	DYLD_LIBRARY_PATH=build \
	./djs-aarch64-macos watch .

DEBUGGING := -g

debug: main
	DYLD_LIBRARY_PATH=build \
	lldb djs-aarch64-macos

main: source/main.c source/engine.c source/gui.c source/draw.c source/colors.c source/styles.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	zig cc $(DEBUGGING) -Wall -rpath @executable_path/build $^ -o djs-aarch64-macos -I../lib/quickjs -I../lib/raylib/raylib/include -I../lib/minnet-quickjs -framework IOKit -framework Cocoa

minified: source/main.c source/engine.c source/gui.c source/draw.c source/colors.c source/styles.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	zig cc -O4 -Wall -rpath @executable_path/build $^ -o djs-mini -I../lib/quickjs -I../lib/raylib/raylib/include -I../lib/minnet-quickjs -framework IOKit -framework Cocoa

# lib/quickjs/libquickjs.a:
# 	cd lib/quickjs/ && $(MAKE)

# lib/raylib/raylib/libraylib.a:
# 	cd lib/raylib/
# 	$(MAKE)