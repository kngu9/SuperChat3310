#ifndef _H_WINDOW_H_
#define _H_WINDOW_H_

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

class Window {
    protected:
    	WINDOW * win;
    	int width;
        int height;
        int d_row;
        int d_col;

    public:
    	Window(int h, int w, int start_row, int start_col, int d_row, int d_col);
    	~Window();
    	WINDOW *getWindow() { return this->win; };
    	void reset();
};

class ChatWindow : public Window {
    private:
        int range;
        void _addMessage(const char * prepend, const char * msg, int pattr, int attr);

    public:
        ChatWindow(int h, int w, int start_row, int start_col, int d_row, int d_col) : Window(h, w, start_row, start_col, d_row, d_col) { range = 1; wrefresh(this->win); };
        void addMessage(const char * userName, const char * msg);
        void reset();
};

class TextWindow : public Window {
    public:
        TextWindow(int h, int w, int start_row, int start_col, int d_row, int d_col) : Window(h, w, start_row, start_col, d_row, d_col) {};
        char * getString(const char * form, char * dst, int max_len);
};

#endif //_H_WINDOW_H_
