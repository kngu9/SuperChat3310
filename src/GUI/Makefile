CPPFLAGS=-g
LDFLAGS=-g
LDLIBS=-lncurses

all: app

app: main.o window.o gui.o
	    g++ main.o window.o gui.o $(LDLIBS)

main.o: main.cpp
	    g++ $(CPPFLAGS) -c main.cpp

gui.o: gui.cpp
	    g++ $(CPPFLAGS) -c gui.cpp gui.h

window.o: window.cpp
	    g++ $(CPPFLAGS) -c window.cpp window.h

