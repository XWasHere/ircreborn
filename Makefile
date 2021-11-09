TARGET   ?= linux

CC       ?= gcc
CC_ARGS  ?=
CC_FARGS  = -ggdb -Isrc

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
	build/config_parser/config.o

all: ircreborn

.PHONY: binit

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

ircreborn: binit  $(OBJS)
ifeq ($(TARGET),linux)
	$(CC) $(CC_ARGS) $(CC_FARGS) $(OBJS) -o ircreborn
else ifeq ($(TARGET),win32)
	$(CC) $(CC_ARGS) $(CC_FARGS) $(OBJS) -o ircreborn -lgdi32
endif
