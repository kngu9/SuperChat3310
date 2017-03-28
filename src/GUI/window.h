#ifndef _H_WINDOW_H_
#define _H_WINDOW_H_

#include <ncurses.h>

class Window {
    protected:
    	WINDOW * win;
    	int width;
        int height;
    public:
    	Window(int h, int w, int start_row, int start_col);
    	~Window();
    	WINDOW *getWindow() { return this->win; };
    	void reset();
};

class ChatWindow : public Window {
    public:
        ChatWindow(int h, int w, int start_row, int start_col) : Window(h, w, start_row, start_col) {};
};

class TextWindow : public Window {
    public:
        TextWindow(int h, int w, int start_row, int start_col) : Window(h, w, start_row, start_col) {};
        char * getString(const char * form, char * dst, int max_len);
};

#endif //_H_WINDOW_H_
