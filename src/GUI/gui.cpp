#include <ncurses.h>
#include "gui.h"

GUI::GUI() {
    initscr();
	start_color();
	cbreak();
	noecho();
	getmaxyx(stdscr, this->d_row, this->d_col);

    init_pair(_CLRP_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(_CLRP_LABEL, COLOR_BLUE, COLOR_WHITE);
    init_pair(_CLRP_RED, COLOR_RED, COLOR_BLACK);
    init_pair(_CLRP_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(_CLRP_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(_CLRP_PINK, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(_CLRP_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(_CLRP_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(_CLRP_YELLOW, COLOR_YELLOW, COLOR_BLACK);

    this->menu_window = new Window(3, this->d_col, 0, 0, this->d_row, this->d_col);
    this->chat_window = new ChatWindow(this->d_row - 8, this->d_col - 50, 3, 0, this->d_row, this->d_col);
    this->chat_clients_window = new Window(this->d_row - 8, 50, 3, this->d_col - 50, this->d_row, this->d_col);
    this->input_window = new TextWindow(5, this->d_col, this->d_row-5, 0, this->d_row, this->d_col);
}

char * GUI::requestName() {
    char * temp_buffer = (char*)malloc(sizeof(char) * 9);

    this->input_window->getString("Username?", temp_buffer, 8);

    return temp_buffer;
}

char * GUI::getInput() {
    char * temp_buffer = (char*)malloc(sizeof(char) * 145);

    this->input_window->getString("Message", temp_buffer, 144);

    return temp_buffer;
}

char * GUI::addMessage(char * username, char * message) {
    this->chat_window->addMessage(username, message);
}

GUI::~GUI(){
    delete this->menu_window;
    delete this->chat_window;
    delete this->chat_clients_window;
    delete this->input_window;
}
