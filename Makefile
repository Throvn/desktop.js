run: main
	DYLD_LIBRARY_PATH=build \
	./main

DEBUGGING := -g

debug: main
	DYLD_LIBRARY_PATH=build \
	lldb main

main: source/main.c source/engine.c source/hashmap.c build/libgui.so build/libwatcher-c.so lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	gcc $(DEBUGGING) -Wall -rpath @executable_path/build $^ -o main -I../lib/quickjs -I../lib/raylib/raylib/include -I../lib/minnet-quickjs -framework IOKit -framework Cocoa

lib/quickjs/libquickjs.a:
	cd lib/quickjs/ && $(MAKE)

lib/raylib/raylib/libraylib.a:
	cd lib/raylib/
	cmake .
	$(MAKE)

build/libgui.so: source/gui.c source/draw.c source/colors.c source/styles.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	gcc $(DEBUGGING) $^ -Wall -shared  -fPIC -DJS_SHARED_LIBRARY -L../lib/quickjs -o build/libgui.so  -framework IOKit -framework Cocoa 