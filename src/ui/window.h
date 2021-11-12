#ifndef IRCREBORN_UI_WINDOW_H
#define IRCREBORN_UI_WINDOW_H

#include <ui/uitypes.h>

window_t* window_init      ();
void      window_display   (window_t* window);
void      window_add_widget(window_t* window, widget_t* widget);
void      window_set_focus (window_t* window, widget_t* widget);

#endif
