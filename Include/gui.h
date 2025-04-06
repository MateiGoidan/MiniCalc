#ifndef GUI
#define GUI

#include <X11/Xlib.h>

typedef struct {
  int x, y;
  int width, height;
  char label[8];
} Button;

extern Button buttons[27];
extern int button_count;

void init_buttons();
void draw_button(Display *display, Window window, GC gc, Button button);
void draw_all_buttons(Display *display, Window window, GC gc);
int get_button_at_position(int x, int y);
void draw_display(Display *display, Window window, GC gc, const char *text);

#endif // !GUI
