TARGET   ?= linux

ifeq ($(TARGET),win32)
CC        = x86_64-w64-mingw32-g++
CC_ARGS  ?= -ggdb -Wall
CC_FARGS  = -Isrc -lgdi32 -lws2_32 -fpermissive
else 
CC       ?= g++
CC_ARGS  ?= -ggdb -Wall
CC_FARGS  = -Isrc -lxcb -lrt -lm -lX11 -lX11-xcb -fpermissive
endif

OBJS = \
	build/main.o \
	build/server/server.o \
	build/client/client.o \
	build/client/set_nickname_dialog.o \
	build/client/license_dialog.o \
	build/common/args.o \
	build/common/util.o \
	build/common/logger.o \
	build/ui/window.o \
	build/ui/widget.o \
	build/ui/widgets/button.o \
	build/ui/widgets/scrollpane.o \
	build/ui/widgets/textbox.o \
	build/ui/widgets/label.o \
	build/ui/widgets/frame.o \
	build/ui/widgets/menubar.o \
	build/ui/util/font_search.o \
	build/config_parser/config.o \
	build/config_parser/theme.o \
	build/networking/networking.o \
	build/networking/types.o \
	build/tests/tests.o \
	build/compat/compat.o

DOCS = \
	build/docs/ircreborn.info

all: ircreborn docs

.PHONY: binit clean __clean docs install

ifeq ($(MAKECMDGOALS),clean)
else ifeq ($(MAKECMDGOALS),binit)
else
include $(OBJS:.o=.cc.d)
endif

build/docs/%.info: docs/%.tex
	makeinfo -o $@ $<

build/%.cc.d: src/%.cc
	$(CC) $(CC_ARGS) $(CC_FARGS) -M $< -MT $< -o $@

build/%.o: src/%.cc build/%.cc.d
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
	mkdir -p build/docs
	mkdir -p build/compat

ircreborn: binit  $(OBJS)
	$(CC) $(CC_ARGS) $(OBJS) -o ircreborn $(CC_FARGS)

install: ircreborn
	install ircreborn                 $(PREFIX)/usr/bin
	install build/docs/ircreborn.info $(PREFIX)/usr/share/info

__clean:
	rm -rf build

clean: __clean binit

docs: $(DOCS)