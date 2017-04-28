#ifndef _H_GUI_H_
#define _H_GUI_H_

#include "window.h"
#include <ncurses.h>
#include <stdlib.h>

#define MESSAGE_SIZE_MAX 144
#define CHATROOM_NAME_MAX 25
#define NICK_SIZE_MAX 8

#define _CLRP_NORMAL		0
#define _CLRP_LABEL		8
#define _CLRP_GREEN		9
#define _CLRP_YELLOW		10
#define _CLRP_BLUE		11
#define _CLRP_PINK		12
#define _CLRP_CYAN		13
#define _CLRP_WHITE		14
#define _CLRP_RED		15

#define CLRP_NORMAL		COLOR_PAIR(_CLRP_NORMAL)
#define CLRP_LABEL		COLOR_PAIR(_CLRP_LABEL)
#define CLRP_GREEN		COLOR_PAIR(_CLRP_GREEN)
#define CLRP_YELLOW		COLOR_PAIR(_CLRP_YELLOW)
#define CLRP_BLUE		COLOR_PAIR(_CLRP_BLUE)
#define CLRP_PINK		COLOR_PAIR(_CLRP_PINK)
#define CLRP_CYAN		COLOR_PAIR(_CLRP_CYAN)
#define CLRP_WHITE		COLOR_PAIR(_CLRP_WHITE)
#define CLRP_RED		COLOR_PAIR(_CLRP_RED)


class GUI {
    private:
        int d_row;
        int d_col;

        ChatWindow * chat_window;
        ClientWindow * chat_clients_window;
        TextWindow * input_window;

    public:
        GUI();
        ~GUI();

        char * requestName();
        char * getInput();
        char * addMessage(char * username, char * message);
        char * addChatroom(int chatroom_idx, char * name, bool reset);
        char * addUser(int chatroom_idx, char * name, bool online, bool reset);
        void printHelp();
};

#endif //_H_GUI_H_
