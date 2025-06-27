run: main
	./main

main: main.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a
	gcc main.c lib/quickjs/libquickjs.a lib/raylib/raylib/libraylib.a -o main -I./lib/quickjs -I./lib/raylib/raylib/include \
		-framework CoreFoundation -framework IOKit -framework Cocoa -framework AppKit

lib/quickjs/libquickjs.a:
	cd lib/quickjs/ && $(MAKE)

lib/raylib/raylib/libraylib.a:
	cd lib/raylib/
	cmake .
	$(MAKE)
