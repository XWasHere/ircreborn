TARGET   ?= linux

ifeq ($(TARGET),win32)
CC        = x86_64-w64-mingw32-gcc
CC_ARGS  ?= 
CC_FARGS  = -Isrc -lgdi32 -lws2_32
else 
CC       ?= gcc
CC_ARGS  ?= -ggdb
CC_FARGS  = -Isrc -lxcb -lrt -lm -lX11 -lX11-xcb
endif

OBJS = \
	build/main.o \
	build/server/server.o \
	build/client/client.o \
	build/common/args.o \
	build/common/util.o \
	build/ui/window.o \
	build/ui/widget.o \
	build/ui/widgets/button.o \
	build/ui/widgets/scrollpane.o \
	build/ui/widgets/textbox.o \
	build/ui/widgets/label.o \
	build/ui/util/font_search.o \
	build/config_parser/config.o \
	build/networking/networking.o \
	build/networking/types.o \
	build/tests/tests.o

all: ircreborn

.PHONY: binit clean __clean

ifeq ($(MAKECMDGOALS),clean)
else ifeq ($(MAKECMDGOALS),binit)
else
include $(OBJS:.o=.c.d)
endif

build/%.c.d: src/%.c
	$(CC) $(CC_ARGS) $(CC_FARGS) -M $< -o $@

build/%.o: src/%.c build/%.c.d
	$(CC) $(CC_ARGS) $(CC_FARGS) -c $< -o $@

binit:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/client
	mkdir -p build/common
	mkdir -p build/config_parser
	mkdir -p build/ui
	mkdir -p build/ui/widgets
	mkdir -p build/ui/util
	mkdir -p build/networking
	mkdir -p build/tests
	mkdir -p build/tests/tests

ircreborn: binit  $(OBJS)
	$(CC) $(CC_ARGS) $(OBJS) -o ircreborn $(CC_FARGS) 

__clean:
	rm -rf build

clean: __clean binit
