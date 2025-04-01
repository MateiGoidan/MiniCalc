#include "../Include/gui.h"
#include <X11/Xlib.h>
#include <string.h>

Button buttons[32];
int button_count = 0;

void init_buttons() {
  /* Initializam structura butoanelor si pozitiile lor. */
  const char *labels[] = {
      "C", "CE", "+/-", "(", ")",
      "7", "8",  "9",   "/", "&",
      "4", "5",  "6",   "*", "%",
      "1", "2",  "3",   "-", "M-",
      "0", ".",  "=",   "+", "M+"
  };
  int rows = 5;
  int cols = 5;
  int btn_width = 80;
  int btn_height = 60;
  int spacing = 20;
  int start_x = 20;
  int start_y = 150;

  button_count = 0;

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
    if (x >= b.x && x <= b.x + b.width && 
      y >= b.y && y <= b.y + b.height) {
      return i;
    }
  }

  return -1;
}

void draw_display(Display *display, Window window, GC gc, const char *text) {
  /* Desenam ecranul calculatorului */
  int x = 20, y = 40;
  int width = 260, height = 40;

  // Dreptunghiul
  XDrawRectangle(display, window, gc, x, y - 25, width, height);

  // Textul
  XDrawString(display, window, gc, x + 10, y, text, strlen(text));
}
