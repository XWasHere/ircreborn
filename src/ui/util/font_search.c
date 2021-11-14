#ifndef WIN32

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>

#include <ui/util/font_search.h>

xcb_font_t request_font(xcb_connection_t* connection, font_request_t* req) {
    char* fn = malloc(255 * 14);
    char* slant = malloc(255);
    char* style = malloc(255);
    char* resx = malloc(255);
    char* resy = malloc(255);
    char* spacing = malloc(255);

    memset(fn, 0, 255 * 14);
    memset(slant, 0, 255);
    memset(style, 0, 255);
    memset(resx, 0, 255);
    memset(resy, 0, 255);
    memset(spacing, 0, 255);

    if (!req->want_slant) {
        sprintf(slant, "*");
    } else {
        if (req->slant == FSEARCH_SLANT_UPRIGHT) {
            sprintf(slant, "r");
        } else {
            abort();
        }
    }

    if (!req->want_style) {
        sprintf(style, "*");
    } else {
        if (req->style == FSEARCH_STYLE_NONE) {
            sprintf(style, "");
        } else {
            abort();
        }
    }

    if (!req->want_resx) {
        sprintf(resx, "*");
    } else {
        sprintf(resx, "%i", req->resx);
    }

    if (!req->want_resy) {
        sprintf(resy, "*");
    } else {
        sprintf(resy, "%i", req->resy);
    }

    if (!req->want_spacing) {
        sprintf(spacing, "*");
    } else {
        if (req->spacing == FSEARCH_SPACING_MONO) {
            sprintf(spacing, "m");
        } else {
            abort();
        }
    }

    sprintf(fn, "-*-*-*-%s-*-%s-*-*-%s-%s-%s-*-*-*", slant, style, resx, resy, spacing);

    xcb_font_t font = xcb_generate_id(connection);

    xcb_open_font(
        connection,
        font,
        strlen(fn),
        fn
    );

    free(slant);
    free(style);
    free(resx);
    free(resy);
    free(spacing);
    free(fn);
    
    return font;
}

#endif