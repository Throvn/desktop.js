run: main
	./main

main: main.c libgui.so lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	gcc -Wall main.c libgui.so lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a -o main -I./lib/quickjs -I./lib/raylib/raylib/include -I./lib/minnet-quickjs -framework CoreFoundation -framework IOKit -framework Cocoa -framework AppKit

lib/quickjs/libquickjs.a:
	cd lib/quickjs/ && $(MAKE)

lib/raylib/raylib/libraylib.a:
	cd lib/raylib/
	cmake .
	$(MAKE)

libgui.so: gui.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	gcc gui.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a -Wall -shared  -fPIC -DJS_SHARED_LIBRARY -L./lib/quickjs -o libgui.so  -framework CoreFoundation -framework IOKit -framework Cocoa -framework AppKit