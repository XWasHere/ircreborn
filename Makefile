TARGET   ?= linux

ifeq ($(TARGET),win32)
CC        = x86_64-w64-mingw32-gcc
CC_ARGS  ?= 
CC_FARGS  = -Isrc -lgdi32 -lws2_32
else 
CC       ?= gcc
CC_ARGS  ?=
CC_FARGS  = -Isrc
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
	build/config_parser/config.o \
	build/networking/networking.o

all: ircreborn

.PHONY: binit clean

build/%.o: src/%.c
	$(CC) $(CC_ARGS) $(CC_FARGS) -c $< -o $@

binit:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/client
	mkdir -p build/common
	mkdir -p build/config_parser
	mkdir -p build/ui
	mkdir -p build/ui/widgets
	mkdir -p build/networking

ircreborn: binit  $(OBJS)
	$(CC) $(CC_ARGS) $(OBJS) -o ircreborn $(CC_FARGS) 

clean:
	rm -rf build