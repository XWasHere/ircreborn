#include <ui/widget.h>
#include <ui/window.h>
#include <ui/widgets/label.h>

void label_draw(widget_t* widget ,window_t* window) {
    label_t* label = widget->extra_data;
    
    PAINTSTRUCT* hi = malloc(sizeof(PAINTSTRUCT));
    RECT *rect = malloc(sizeof(RECT));
    
    SetRect(rect, widget->x, widget->y, widget->x + widget->width, widget->y + widget->height);
    InvalidateRect(window->window, rect, 1);
    BeginPaint(window->window, hi);

    Rectangle(hi->hdc, rect->left, rect->top, rect->right, rect->bottom);
    DrawText(hi->hdc, label->text, strlen(label->text), rect, 0);

    EndPaint(window->window, hi);
    
    free(hi);
    free(rect);
}

widget_t* label_init() {
    widget_t* label = widget_init();
    label_t*  lab   = malloc(sizeof(label_t));

    lab->len  = 0;
    lab->text = 0;

    lab->widget = label;
    label->extra_data = lab;

    label->draw = &label_draw;
    
    return label;
}