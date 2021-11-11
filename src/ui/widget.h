#ifndef LIBUI_WIDGET_H
#define LIBUI_WIDGET_H

typedef struct __widget widget_t;
struct __widget {
    // position
    int x;
    int y;

    // size
    int width;
    int height;

    // for widgets
    void* extra_data;

    // for measuring the state in the window code
    int hovered;

    // draw function prototype (pretend window is window_t*)
    void (*draw)(widget_t* widget, void* window);
    void (*clicked)(widget_t* widget, void* window, int x, int y);
    void (*mousein)(widget_t* widget, void* window);
    void (*mouseout)(widget_t* widget, void* window);
    void (*mousedown)(widget_t* widget, void* window, int x, int y);
    void (*mouseup)(widget_t* widget, void* window, int x, int y);
    void (*mousemove)(widget_t* widget, void* window, int x, int y);
    void (*keypress)(widget_t* widget, void* window, char key);
};

widget_t* widget_init();

#endif
