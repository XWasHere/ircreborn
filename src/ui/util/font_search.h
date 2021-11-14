#ifndef IRCREBORN_FONT_SEARCH_H
#define IRCREBORN_FONT_SEARCH_H

#ifndef WIN32
#include <xcb/xcb.h>

#define FSEARCH_STYLE_NONE 0

#define FSEARCH_SLANT_UPRIGHT 0

#define FSEARCH_SPACING_MONO 0

typedef struct font_request font_request_t;
struct font_request {
    int want_slant;
    int want_style;
    int want_resx;
    int want_resy;
    int want_spacing;
    int slant;
    int style;
    int resx;
    int resy;
    int spacing;
};

xcb_font_t request_font(xcb_connection_t* connection, font_request_t* req);

#endif
#endif