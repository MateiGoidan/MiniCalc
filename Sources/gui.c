#include "../Include/gui.h"
#include <X11/Xlib.h>
#include <string.h>

Button buttons[27];
int button_count = 0;

void init_buttons() {
  /* Initializam structura butoanelor si pozitiile lor. */
  const char *labels[] = {"M+", "C",  "CE", "(", ")", "MR", "7", "8",  "9",
                          "/",  "MC", "4",  "5", "6", "*",  ":", "1",  "2",
                          "3",  "-",  "%",  "0", ".", "=",  "+", "+/-"};

  int rows = 5;
  int cols = 5;
  int btn_width = 80;
  int btn_height = 60;
  int spacing = 20;
  int start_x = 20;
  int start_y = 100;

  // Buton de memorie pus pe acelasi rand cu ecranul
  Button memory;
  memory.x = 420;
  memory.y = 20;
  memory.width = btn_width;
  memory.height = btn_height;
  strncpy(memory.label, labels[0], sizeof(memory.label) - 1);
  memory.label[sizeof(memory.label) - 1] = '\0';
  buttons[0] = memory;

  button_count = 1;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      Button b;
      b.x = start_x + col * (btn_width + spacing);
      b.y = start_y + row * (btn_height + spacing);
      b.width = btn_width;
      b.height = btn_height;
      strncpy(b.label, labels[button_count], sizeof(b.label) - 1);
      b.label[sizeof(b.label) - 1] = '\0';
      buttons[button_count++] = b;
    }
  }
}

void draw_button(Display *display, Window window, GC gc, Button b) {
  /* Desenam un buton */
  XDrawRectangle(display, window, gc, b.x, b.y, b.width, b.height);
  int text_x = b.x + b.width / 2 - 5;
  int text_y = b.y + b.height / 2 + 5;
  XDrawString(display, window, gc, text_x, text_y, b.label, strlen(b.label));
}

void draw_all_buttons(Display *display, Window window, GC gc) {
  /* Desenam toate butoanele */
  for (int i = 0; i < button_count; i++) {
    draw_button(display, window, gc, buttons[i]);
  }
}

int get_button_at_position(int x, int y) {
  /* Preia butonul de pe pozitia (x, y) */
  for (int i = 0; i < button_count; i++) {
    Button b = buttons[i];
    if (x >= b.x && x <= b.x + b.width && y >= b.y && y <= b.y + b.height) {
      return i;
    }
  }

  return -1;
}

void draw_display(Display *display, Window window, GC gc, const char *text) {
  /* Desenam ecranul calculatorului */
  int x = 20, y = 40;
  int width = 380, height = 60;

  // Dreptunghiul
  XDrawRectangle(display, window, gc, x, y - 20, width, height);

  // Textul principal (negru)
  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  XDrawString(display, window, gc, x + 10, y, text, strlen(text));

  // Dacă avem mesaj de eroare
  extern char error_msg[];
  if (strlen(error_msg) > 0) {
    XColor red;
    Colormap colormap = DefaultColormap(display, 0);
    XParseColor(display, colormap, "#FF0000", &red);
    XAllocColor(display, colormap, &red);

    XSetForeground(display, gc, red.pixel);
    XDrawString(display, window, gc, x + 10, y + 30, error_msg,
                strlen(error_msg));

    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  }
}
