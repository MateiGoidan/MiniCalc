#include "../Include/gui.h"
#include "../Include/logic.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <complex.h>
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

char input[62] = "";
char error_msg[128] = "";
double memory = 0.0;

int handle_x_error(Display *display, XErrorEvent *error) {
  char error_text[1024];

  XGetErrorText(display, error->error_code, error_text, sizeof(error_text));

  fprintf(stderr, "X11 Error: %s\n", error_text);

  return 0;
}

int validate_parentheses(const char *expr) {
  /* Verificam daca parantezele au fost inchise  */
  int numb = 0;

  while (*expr) {
    if (*expr == '(') {
      numb++;
    } else if (*expr == ')') {
      numb--;
    }

    expr++;
  }

  if (numb != 0) {
    strcpy(error_msg, "Unmatched parentheses.");
    return 0;
  }

  return 1;
}

int check_dot(const char *input) {
  /* Verifica daca un numar este zecimal */
  int i = strlen(input) - 1;

  while (i >= 0 && (isdigit(input[i]) || input[i] == '.')) {
    // Cautam punctul
    if (input[i] == '.') {
      // Am gasit deja un punct
      return 1;
    }
    i--;
  }

  return 0;
}

int main() {
  display = XOpenDisplay(NULL); // Conectare la X server
  if (display == NULL) {
    fprintf(stderr, "Error: Cold not open the default display!\n");
    exit(1);
  }

  XSetErrorHandler(handle_x_error);

  screen = DefaultScreen(display); // Preluam ecranul inplicit

  // Creare fereastra
  // Am folosit RootWindow deoarece lucrez cu mai multe monitoare
  window = XCreateSimpleWindow(display, RootWindow(display, screen), 100, 100,
                               520, 500, 10, BlackPixel(display, screen),
                               WhitePixel(display, screen));

  XStoreName(display, window, "MiniCalc");

  XSizeHints *size_hints = XAllocSizeHints();

  if (!size_hints) {
    fprintf(stderr, "Error: Can't allocate XSizeHints'\n");
    exit(1);
  }

  size_hints->flags = PMinSize | PMaxSize;
  size_hints->min_width = 520;
  size_hints->max_width = 520;
  size_hints->min_height = 500;
  size_hints->max_height = 500;

  XSetNormalHints(display, window, size_hints);
  XFree(size_hints); // Nu doresc memorie pe heap de la size hints

  XSelectInput(display, window, ExposureMask | KeyPress | ButtonPressMask);

  XMapWindow(display, window); // Afisam fereastra pe ecran

  XSync(display, False);

  GraphicContext = XCreateGC(display, window, 0, NULL);

  init_buttons();

  int run = True;
  XEvent e;
  Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);

  while (run) {
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

      if (strcmp(label, "=") == 0) {
        if (validate_parentheses(input)) {
          error_msg[0] = '\0';

          double result = evaluate_expression(input);

          if (!strlen(error_msg)) {
            snprintf(input, sizeof(input), "%.6g", result);
          }
        }
      } else if (!strcmp(label, "+/-")) {
        // Operatia de schimbare a semnului
        error_msg[0] = '\0';

        int len = strlen(input);

        if (len != 0) {
          // Cautam incpeutul ultimului numar
          int i = len - 1;

          while (i >= 0 && (isdigit(input[i]) || input[i] == '.')) {
            i--;
          }

          if (input[i] == '-') {
            // Daca avem deja minus

            if (isdigit(input[i - 1]) || input[i - 1] == ')') {
              // Daca se afla in interiorul expresiei

              input[i] = '+';

            } else {
              // Daca se afla la inceputul expresiei

              memmove(&input[i], &input[i + 1], len - i);
            }

          } else {
            // Inseram minus

            if (len + 1 < 256) {

              memmove(&input[i + 2], &input[i + 1], len - i);

              input[i + 1] = '-';
            }
          }
        }
      } else if (!strcmp(label, "C")) {
        // Operatie de stergere
        error_msg[0] = '\0';

        input[0] = '\0';

      } else if (!strcmp(label, "CE")) {
        // Operatie de stergere a ultimului caracter
        error_msg[0] = '\0';

        size_t len = strlen(input);

        if (len > 0) {
          input[len - 1] = '\0';
        }

      } else if (!strcmp(label, "M+")) {
        // Operatie de salvare in memorie
        error_msg[0] = '\0';

        int valid = 1;
        int i = 0;

        if (input[0] == '-') {
          i++;
        }

        for (; input[i]; i++) {
          if (input[i] != '.' && !isdigit(input[i])) {
            valid = 0;
            break;
          }
        }

        if (valid && i > 0) {
          memory = atof(input);
        } else {
          strcpy(error_msg, "Cannot store expression in memory");
        }

      } else if (!strcmp(label, "MR")) {
        // Operatie de scriere din memorie
        error_msg[0] = '\0';

        char mem_str[64];

        snprintf(mem_str, sizeof(mem_str), "%.6g", memory);

        if (strlen(mem_str) + strlen(input) >= sizeof(input)) {

          strcpy(error_msg, "Not enough space for memory value");

        } else {

          strncat(input, mem_str, sizeof(input) - strlen(input) - 1);
        }

      } else if (!strcmp(label, "MC")) {
        // Operateie de stergere din memorie
        error_msg[0] = '\0';

        memory = 0.0;

      } else if (!strcmp(label, "+") || !strcmp(label, "*") ||
                 !strcmp(label, "/") || !strcmp(label, ":") ||
                 !strcmp(label, "%")) {
        error_msg[0] = '\0';

        size_t len = strlen(input);

        if (len > 0 && (input[len - 1] == '+' || input[len - 1] == '*' ||
                        input[len - 1] == '/' || input[len - 1] == ':' ||
                        input[len - 1] == '%')) {

          input[len - 1] = label[0];

        } else if (len >= 1 && input[len - 1] == '-') {

          input[len - 1] = label[0];

        } else if (len >= 1 && input[len - 1] != '.' && input[len - 1] != '(') {

          strncat(input, label, sizeof(input) - strlen(input) - 1);
        }

      } else if (!strcmp(label, "-") && strlen(input) > 0) {
        error_msg[0] = '\0';

        size_t len = strlen(input);

        if (input[len - 1] != '-' && input[len - 1] != '.') {

          strncat(input, label, sizeof(input) - strlen(input) - 1);
        }

      } else if (!strcmp(label, "(") && strlen(input) > 0) {

        size_t len = strlen(input);

        if (!isdigit(input[len - 1]) && input[len - 1] != '.') {

          strncat(input, label, sizeof(input) - strlen(input) - 1);
        }

      } else if (!strcmp(label, ")") && strlen(input) > 0) {

        size_t len = strlen(input);

        if (isdigit(input[len - 1])) {

          strncat(input, label, sizeof(input) - strlen(input) - 1);
        }

      } else if (!strcmp(label, ".")) {

        if (!check_dot(input)) {

          strncat(input, label, sizeof(input) - strlen(input) - 1);
        }

      } else {
        // Adaugam la input
        error_msg[0] = '\0';

        strncat(input, label, sizeof(input) - strlen(input) - 1);
      }

      // Redesenare ecran
      XClearWindow(display, window);

      draw_display(display, window, GraphicContext, input);

      draw_all_buttons(display, window, GraphicContext);

      break;
    }
    case ClientMessage:
      if ((Atom)e.xclient.data.l[0] == WM_DELETE_WINDOW) {
        run = False;
      }

      break;
    }
  }

  XCloseDisplay(display);

  return EXIT_SUCCESS;
}
