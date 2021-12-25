#ifndef IRCREBORN_COLOR_H
#define IRCREBORN_COLOR_H

#ifdef WIN32
#include <windows.h>
#endif
#include <stdint.h>

#define RGBA(x)     (((x&0xff000000)>>24)|((x&0x00ff0000)>>8)|((x&0x0000ff00)<<8)|((x&0x000000ff)<<24))
#ifdef WIN32
#define W32RGBAC(x) (RGB(x.r, x.g, x.b))
#endif

typedef struct rgba rgba_t;

struct rgba {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

#endif