#include "../Include/gui.h"
#include "../Include/logic.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
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
double memory = 0.0;

int validate_parentheses(const char *expr) {
  /* Verificam daca parantezele au fost inchise  */
  int numb = 0;

  while (*expr) {
    if (*expr == '(') {
      numb++;
    } else if (*expr == ')') {
      numb--;
    }

    if (numb < 0) {
      return 0;
    }

    expr++;
  }

  return numb == 0;
}

int main() {
  display = XOpenDisplay(NULL); // Conectare la X server
  if (display == NULL) {
    fprintf(stderr, "Error: cold not open the default display!\n");
    exit(1);
  }

  screen = DefaultScreen(display); // Preluam ecranul inplicit

  // Creare fereastra
  // Am folosit RootWindow deoarece lucrez cu mai multe monitoare
  window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100,
                               520, 560, 10, BlackPixel(display, screen),
                               WhitePixel(display, screen));

  XSelectInput(display, window, ExposureMask | KeyPress | ButtonPressMask);

  XMapWindow(display, window); // Afisam fereastra pe ecran

  GraphicContext = XCreateGC(display, window, 0, NULL);

  init_buttons();

  XEvent e;
  while (True) {
    XNextEvent(display, &e); // Preluam urmatorul eveniment

    switch (e.type) {
    case Expose:
      // Daca parte din fereastra devine vizibila

      draw_display(display, window, GraphicContext, input);

      draw_all_buttons(display, window, GraphicContext);

      break;

    case ButtonPress: {
      // Apasare butoane
      int x = e.xbutton.x;
      int y = e.xbutton.y;
      int index = get_button_at_position(x, y);

      char *label = buttons[index].label;
      if (index != -1) {
        printf("Apăsat pe buton: %s\n", label);
      }

      if (strcmp(label, "=") == 0 && validate_parentheses(input)) {

        double result = evaluate_expression(input);

        snprintf(input, sizeof(input), "%.6g", result);

      } else if (strcmp(label, "+/-") == 0) {
        // Operatia de schimbare a semnului
        int len = strlen(input);

        if (len != 0) {
          // Cautam incpeutul ultimului numar
          int i = len - 1;
          while (i >= 0 && (isdigit(input[i]) || input[i] == '.')) {
            i--;
          }

          if (input[i] == '-') {
            // Daca avem deja minus, il stergem 
            memmove(&input[i], &input[i + 1], len - i);
          } else {
            // Inseram minus
            if (len + 1 < 256) {
              memmove(&input[i + 2], &input[i + 1], len - i);
              input[i + 1] = '-';
            }
          }
        }
      } else if (strcmp(label, "C") == 0) {
        // Operatie de stergere

        input[0] = '\0';

      } else if (strcmp(label, "CE") == 0) {
        // Operatie de stergere a ultimului caracter

        size_t len = strlen(input);

        if (len > 0) {
          input[len - 1] = '\0';
        }

      } else if (strcmp(label, "M+") == 0) {
        // Operatie de salvare in memorie

        memory = atof(input);

      } else if (strcmp(label, "MR") == 0) {
        // Operatie de scriere din memorie

        char mem_str[64];

        snprintf(mem_str, sizeof(mem_str), "%.6g", memory);

        strncat(input, mem_str, sizeof(input) - strlen(input) - 1);

      } else if (strcmp(label, "MC") == 0) {
        // Operateie de stergere din memorie

        memory = 0.0;

      } else {
        // Adaugam semnul la input normal

        strncat(input, label, sizeof(input) - strlen(input) - 1);
      }

      // Redesenare ecran
      XClearWindow(display, window);

      draw_display(display, window, GraphicContext, input);

      draw_all_buttons(display, window, GraphicContext);

      break;
    }
    }
  }

  XCloseDisplay(display);

  return EXIT_SUCCESS;
}
