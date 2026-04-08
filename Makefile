# Get system info
# Darwin, Linux
OS := $(strip $(shell uname -s))
# amd64, arm64, x86_64, aarch64
ARCH := $(shell uname -m)

# Name of the Desktop.js Runtime file
EXE_NAME := djs-$(shell echo $(OS) | tr A-Z a-z)-$(ARCH) # e.g. djs-darwin-arm64

SOURCE_FILES = debug.c \
			gui/fonts.c \
			gui/blob.c \
			gui/memory.c \
			gui/colors.c \
			gui/js.c \
			gui/styles.c \
			renderer/reconcile.c \
			gui/draw/draw.c \
			gui/draw/image.c \
			events/events.c \
			events/mouse.c \
			events/keyboard.c \
			platform.c \
			main.c

LIBRARY_FILES = lib/txiki.js/libtjs.a \
				lib/txiki.js/deps/quickjs/libqjs.a \
				lib/txiki.js/deps/wasm3/source/libm3.a \
				lib/txiki.js/deps/libuv/libuv.a \
				lib/txiki.js/deps/sqlite3/libsqlite3.a \
				lib/txiki.js/deps/mimalloc/libmimalloc.a \
				lib/raylib/raylib/libraylib.a


# If this Makefile is run on macOS, include OS specific flags.
# Else if it's run on Linux, include their respective flags.
ifeq ($(OS),Darwin)
    OS_FLAGS = -rpath @executable_path/build -framework IOKit -framework Cocoa -lffi -lcurl 
else
    # Fixed the elif syntax and ensured curl/ffi are ready for Linux
    OS_FLAGS = -lpthread -Wl,-rpath,\$$ORIGIN/build -lffi -lcurl -lm
endif

CFLAGS = -Ilib/raylib/src \
		 -Ilib/txiki.js/deps/quickjs \
		 -Ilib/txiki.js/src \
		 -Ilib/txiki.js/deps/libuv/include


# Read all source files in source/ and turn them into .o files in build/.
# This we need because we want to build the .o files with a different amount of optimization and debug information.
# Create the build/ folder first, as otherwise the path doesn't exist and the .o files cannot be created there.
SOURCE_DIR = source
BUILD_DIR = build
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(dir $@)
	clang $(CFLAGS) -c $< -o $@

# Map the .c source file to the correct .o file.
# This is needed for the linking step down below.
OBJS = $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)


# We want to:
#	-g Generate 	source-level debug information
#	-fsanitize 		Turn on runtime checks for various forms of undefined or suspicious behavior.
# 	-O0				Disable optimization passes
#	-Wall -Wextra	Enable all error messages
DEBUG_FLAGS = -g -fsanitize=address,undefined -O0 -Wall -Wextra

RELEASE_FLAGS = -O3

.PHONY: debug
debug: CFLAGS += $(DEBUG_FLAGS)

.PHONY: release
release: CFLAGS += $(RELEASE_FLAGS)

# Link the source .o files.
# Link all the static libraries.
# Assemble all of that together.
# Create the final executable.
# No matter which OS we are on (Max/Linux)
debug release: clean $(OBJS) $(LIBRARY_FILES)
	clang $(CFLAGS) -o $(EXE_NAME) \
        $(OBJS) \
        $(LIBRARY_FILES) \
        $(OS_FLAGS)

# If we don't have the static library for txiki yet,
# build the entire library first.
lib/txiki.js/libtjs.a:
	cd lib/txiki.js/ && cmake . && make

# If we don't have the static library for raylib yet,
# build the entire library first.
lib/raylib/raylib/libraylib.a:
	cd lib/raylib && cmake . && make

# Runs DEBUG executable with default JS debug path
# Needed by the ./run.sh script in project root.
.PHONY: run
run: debug
	DYLD_LIBRARY_PATH=build \
	./$(EXE_NAME) run ./.sandbox/javascript/index.js

# PHONY is a target which is always out of date 
# (so it will always update all of its dependencies)
.PHONY: clean
clean:
	rm -f $(OBJS) $(EXE_NAME)
	rm -rf $(BUILD_DIR)

.PHONY: install
install:
	git submodule update --init --recursive; \
	cd lib/raylib && cmake .; \
	cd ../..; \
	$(MAKE) debug
	echo "Project setup successful: $(EXE_NAME) was built."