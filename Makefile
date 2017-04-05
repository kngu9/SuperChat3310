all:./bin/SuperChat

COMMON_LIBS=-L${OSPL_HOME}/lib/ \
            -ldcpssacpp \
            -lddskernel \
            -lpthread

CXX_FLAGS=-O0 -g

FILES_GENERATED_FROM_THE_IDL_BY_IDLPP= ./bin/SuperChatDcps_impl.h ./bin/SuperChat.cpp \
                                     ./bin/SuperChatSplDcps.cpp ./bin/SuperChatDcps.cpp \
                                     ./bin/SuperChatDcps_impl.cpp ./bin/ccpp_SuperChat.h \
                                     ./bin/SuperChatDcps.h ./bin/SuperChat.h ./bin/SuperChatSplDcps.h

${FILES_GENERATED_FROM_THE_IDL_BY_IDLPP}:./idl/SuperChat.idl
	idlpp -I ${OSPL_HOME}/etc/idl -l cpp ./idl/SuperChat.idl -d ./bin

SRC_FILES= src/DDSEntityManager.cpp src/CheckStatus.cpp src/GUI/gui.cpp src/GUI/window.cpp

bin/SuperChat: ${FILES_GENERATED_FROM_THE_IDL_BY_IDLPP} ./src/SuperChat.cpp ${SRC_FILES}
	g++ ${CXX_FLAGS} -o ./bin/SuperChat ./src/SuperChat.cpp \
               -I./src/ -I./include -I./bin \
               -I${OSPL_HOME}/include/dcps/C++/SACPP/ \
               ${SRC_FILES} \
               ${FILES_GENERATED_FROM_THE_IDL_BY_IDLPP} \
               ${COMMON_LIBS} \
               -lncurses

test:
	-g++ tests/main.cpp && ./a.out

clean:
	-rm -f ./bin/SuperChat ${FILES_GENERATED_FROM_THE_IDL_BY_IDLPP}
