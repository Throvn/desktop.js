run: main
	./main

DEBUGGING := -g

main: main.c engine.c libgui.so lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	gcc $(DEBUGGING) -Wall main.c engine.c libgui.so lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a -o main -I./lib/quickjs -I./lib/raylib/raylib/include -I./lib/minnet-quickjs -framework IOKit -framework Cocoa

lib/quickjs/libquickjs.a:
	cd lib/quickjs/ && $(MAKE)

lib/raylib/raylib/libraylib.a:
	cd lib/raylib/
	cmake .
	$(MAKE)

libgui.so: gui.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	gcc $(DEBUGGING) gui.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a -Wall -shared  -fPIC -DJS_SHARED_LIBRARY -L./lib/quickjs -o libgui.so  -framework IOKit -framework Cocoa 