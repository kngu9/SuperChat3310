#include "window.h"
#include "gui.h"

Window::Window(int height, int width, int start_row, int start_col, int d_row, int d_col) {
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

    do {
        this->reset();

        wprintw(this->win, "%s: ", form);
        wgetnstr(this->win, dst, max_len);

   } while(*dst == 0);

   noecho();
   this->reset();

   return dst;
}

void ChatWindow::reset() {
    wclear(this->win);
    box(this->win, 0, 0);
    wmove(this->win, 1, 1);
    wrefresh(this->win);

    this->range = 1;
}

void ClientWindow::reset() {
    wclear(this->win);
    box(this->win, 0, 0);
    wmove(this->win, 1, 1);
    wrefresh(this->win);

    this->range = 1;
}

void ChatWindow::_addMessage(const char * prepend, const char * msg, int pattr, int attr) {
    if(range > height - 2) {
        this->reset();
    }

    double l1 = strlen(prepend);
    double l2 = strlen(msg);

    if (prepend != NULL) {
        //wattron(this->win, pattr);
        //mvwprintw(this->win, range, 1,"%s",prepend);
        mvwaddnstr(this->win, range, 1, prepend, 8);
        //wattroff(this->win, pattr);

        //wattron(this->win, attr);
        wprintw(this->win, ": ");
        waddnstr(this->win, msg, 144);
        //wattroff(this->win, attr);
    } else {
        //wattron(this->win, pattr);
        mvwaddnstr(this->win, range, 1, msg, 144);
        //mvwprintw(this->win, range, 1, "%s",msg);
        //wattroff(this->win, pattr);
    }

    range += 1;
    wrefresh(this->win);
}

void ChatWindow::addMessage(const char * username, const char * msg) {
    this->_addMessage(username, msg, CLRP_LABEL, CLRP_NORMAL);
}


void ClientWindow::_addChatroom(int chatroom_idx, const char * name, bool reset, int pattr, int attr) {
    if(reset) {
        this->reset();
    }

    double l2 = strlen(name);

   
    //wattron(this->win, pattr);
    mvwprintw(this->win, range, 1, "%d ", chatroom_idx);
    waddnstr(this->win, name, 25);
    //wattroff(this->win, pattr);


    range += 1;
    wrefresh(this->win);
}

void ClientWindow::addChatroom(int chatroom_idx, const char * name, bool reset) {
    this->_addChatroom(chatroom_idx, name, reset, CLRP_LABEL, CLRP_NORMAL);
}

void ClientWindow::_addUser(int chatroom_idx, const char * name, bool online, bool reset, int pattr, int attr) {
    if(reset) {
        this->reset();
    }

    double l2 = strlen(name);

   
    //wattron(this->win, pattr);
    if(online)
    {
      mvwaddnstr(this->win, range, 1, name, 8);
      wprintw(this->win, " CR:%d", chatroom_idx);
    }
    //wattroff(this->win, pattr);
    else
    {
      mvwaddnstr(this->win, range, 1, name, 8);
      wprintw(this->win, " OFFLINE");
    }


    range += 1;
    wrefresh(this->win);
}

void ClientWindow::addUser(int chatroom_idx, const char * name, bool online, bool reset) {
    this->_addUser(chatroom_idx, name, online, reset, CLRP_LABEL, CLRP_NORMAL);
}

void ClientWindow::printHelp() {
    this->reset();
    mvwprintw(this->win, range++, 1, "%s", "Welcome to\n");
    mvwprintw(this->win, range++, 1, "%s", "SuperChat!\n");
    mvwprintw(this->win, range++, 1, "%s", ":CCR #\n");
    mvwprintw(this->win, range++, 1, "%s", ":NCR # [name]\n");
    mvwprintw(this->win, range++, 1, "%s", ":PCR\n");
    mvwprintw(this->win, range++, 1, "%s", ":CNK [nick]\n");
    mvwprintw(this->win, range++, 1, "%s", ":PUR\n");
    mvwprintw(this->win, range++, 1, "%s", ":EXT\n");
    mvwprintw(this->win, range++, 1, "%s", ":PHP");
    wrefresh(this->win);
}
