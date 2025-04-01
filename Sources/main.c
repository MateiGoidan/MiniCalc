#include "../Include/gui.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ANSI_COLOR_GREEN "\033[3;92m"
#define ANSI_COLOR_SUCCESS "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\033[0m"

Display *display;
Window window;
GC GraphicContext;
int screen;

char input[256] = "";

int main() {
  display = XOpenDisplay(NULL); // Conectare la X server
  if (display == NULL) {
    fprintf(stderr, "Error: cold not open the default display!\n");
    exit(1);
  }

  screen = DefaultScreen(display); // Preluam ecranul inplicit

  // Creare fereastra
  window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100,
                               520, 560, 10, BlackPixel(display, screen),
                               WhitePixel(display, screen));

  XSelectInput(display, window, ExposureMask | KeyPress | ButtonPressMask);
  XMapWindow(display, window);

  GraphicContext = XCreateGC(display, window, 0, NULL);

  init_buttons();

  XEvent e;
  while (True) {
    XNextEvent(display, &e); // Preluam urmatorul eveniment

    switch (e.type) {
    case Expose:
      draw_display(display, window, GraphicContext, input);
      draw_all_buttons(display, window, GraphicContext);
      break;
    case ButtonPress: {
      int x = e.xbutton.x;
      int y = e.xbutton.y;
      int index = get_button_at_position(x, y);
      if (index != -1) {
        printf("Apăsat pe buton: %s\n", buttons[index].label);
      }
      strncat(input, buttons[index].label, sizeof(input) - strlen(input) - 1);

      // Redesenare ecran
      XClearWindow(display, window);
      draw_display(display, window, GraphicContext, input);
      draw_all_buttons(display, window, GraphicContext);
      break;
    }
    }

    if (e.type == Expose) {
      // redesenează interfața
      draw_all_buttons(display, window, GraphicContext);
    }
  }

  XCloseDisplay(display);

  return EXIT_SUCCESS;
}
