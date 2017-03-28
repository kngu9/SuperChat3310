#include "window.h"

Window::Window(int height, int width, int start_row, int start_col) {
    this->height = height;
    this->width = width;

    this->win = newwin(height, width, start_row, start_col);
    box(win, 0, 0);
    wrefresh(win);
}

Window::~Window() {
    delwin(this->win);
}

void Window::reset() {
    wclear(win);
    box(win, 0, 0);
    wmove(win, 1, 1);
    wrefresh(win);
}

char * TextWindow::getString(const char * form, char * dst, int max_len) {
    int cur_len = 0;
    echo();
	wmove(this->win, 1, 1);

    do{
        this->reset();
        wprintw(this->win, "%s: ", form);
        wgetnstr(this->win, dst, max_len);

   } while(*dst == 0);

   noecho();
   this->reset();

   return dst;
}
