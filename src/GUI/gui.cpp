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

    this->chat_window = new ChatWindow((5*this->d_row)/6, (3*this->d_col)/4, 0, 0, this->d_row, this->d_col);
    this->chat_clients_window = new ClientWindow((5*this->d_row)/6, this->d_col/4, 0, (3*this->d_col)/4, this->d_row, this->d_col);
    this->input_window = new TextWindow(this->d_row/6, this->d_col, (5*this->d_row)/6, 0, this->d_row, this->d_col);
}

char * GUI::requestName() {
    char * temp_buffer = (char*)malloc(sizeof(char) * 8);

    this->input_window->getString("Username?", temp_buffer, 8);

    return temp_buffer;
}

char * GUI::getInput() {
    char * temp_buffer = (char*)malloc(sizeof(char) * 144);

    this->input_window->getString("Message", temp_buffer, 144);

    return temp_buffer;
}

char * GUI::addMessage(char * username, char * message) {
    this->chat_window->addMessage(username, message);
}

char * GUI::addChatroom(int chatroom_idx, char * name, bool reset) {
    this->chat_clients_window->addChatroom(chatroom_idx, name, reset);
}

char * GUI::addUser(int chatroom_idx, char * name, bool online, bool reset) {
    this->chat_clients_window->addUser(chatroom_idx, name, online, reset);
}

void GUI::printHelp() {
    this->chat_clients_window->printHelp();
}

GUI::~GUI(){
    delete this->chat_window;
    delete this->chat_clients_window;
    delete this->input_window;

    clear();
    endwin();
}
