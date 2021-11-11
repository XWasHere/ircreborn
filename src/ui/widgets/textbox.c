#include <stdlib.h>

#include <ui/widgets/textbox.h>
#include <ui/widget.h>
#include <ui/window.h>

#ifdef WIN32
#include <windows.h>
#endif

void __DEFAULT_textbox_submit(widget_t* a, void* b, char* c, int d) {};

void textbox_keypress(widget_t* widget, window_t* window, char key) {
    textbox_t* tb = widget->extra_data;

    if (key == 8) {
        tb->textlen--;
        tb->cursorpos--;
        
        tb->text[tb->cursorpos] = 0;
    } else {
        if ((key == 10 || key == 13) && tb->multiline == 0) {
            tb->submit(widget, window, tb->text, tb->textlen);
            return;
        }
        tb->textlen++;

        if (tb->text == 0) tb->text = malloc(1);
        
        tb->text = realloc(tb->text, tb->textlen + 1);
        
        tb->text[tb->cursorpos] = key;
        tb->cursorpos++;
        tb->text[tb->cursorpos] = 0;
    }

    widget->draw(widget, window);
}

void textbox_clicked(widget_t* widget, window_t* window) {
    window_set_focus(window, widget);
}

void textbox_draw(widget_t* widget, window_t* window) {
    textbox_t* tb = widget->extra_data;
    
    PAINTSTRUCT* ps   = malloc(sizeof(PAINTSTRUCT));
    RECT*        rect = malloc(sizeof(RECT));

    int h = 0;

    if (tb->text != 0) {
        SetRect(rect, widget->x, widget->y, widget->x + widget->width, widget->y + widget->height);
        InvalidateRect(window->window, rect, 1);

        BeginPaint(window->window, ps);

        Rectangle(ps->hdc, rect->left, rect->top, rect->right, rect->bottom);
        DrawText(ps->hdc, tb->text, strlen(tb->text), rect, 0);

        EndPaint(window->window, ps);
    } else {
        SetRect(rect, widget->x, widget->y, widget->x + widget->width, widget->y + widget->height);

        InvalidateRect(window->window, rect, 1);

        BeginPaint(window->window, ps);

        Rectangle(ps->hdc, rect->left, rect->top, rect->right, rect->bottom);

        EndPaint(window->window, ps);
    }
    
    free(ps);
    free(rect);
}

widget_t* textbox_init() {
    widget_t* textbox = widget_init();
    textbox_t* tb = malloc(sizeof(textbox_t));

    tb->widget = textbox;
    tb->textlen = 0;
    tb->cursorpos = 0;
    tb->text = 0;
    tb->multiline = 0;
    tb->submit = &__DEFAULT_textbox_submit;

    textbox->extra_data = tb;
    textbox->draw = &textbox_draw;
    textbox->clicked = &textbox_clicked;
    textbox->keypress = &textbox_keypress;

    return textbox;
}