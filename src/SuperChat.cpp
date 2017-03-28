#include <signal.h>
#include <assert.h>
#include <ncurses.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <pthread.h>

//#include "globals.h"
#include "GUI/gui.h"
#include "DDSEntityManager.h"
#include "ccpp_SuperChat.h"

// This code is derived from the PrismTech HelloWorld examples

using namespace DDS;
using namespace SuperChat;

// these classes abstract out the sending and recieving of topics
// perhaps with some cleverness, generics could be used to eliminate
// some of the code

typedef std::vector<chatroom> chatroom_list_t;

class chatroom_data
{
    private:
        DDSEntityManager em;
        chatroomDataWriter_var Writer;
        chatroomDataReader_var Reader;
    public:
        void send ( chatroom message_instance )
        {
            ReturnCode_t status = Writer->write(message_instance, DDS::HANDLE_NIL);
            checkStatus(status, "Writer::write");
        }
        void recv ( chatroom_list_t* chatroomList )
        {
            // if any data is available, it returns a list
            assert ( chatroomList );
            chatroomSeq msgList;
            SampleInfoSeq infoSeq;
            ReturnCode_t status =  - 1;
            status = Reader->take ( msgList, infoSeq, LENGTH_UNLIMITED,
                    ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE );
            checkStatus(status, "msgDataReader::take");
            for (DDS::ULong j = 0; j < msgList.length(); j++)
            {
                chatroomList->push_back ( msgList[j] );
            }
            status = Reader->return_loan ( msgList, infoSeq );
            checkStatus(status, "MsgDataReader::return_loan");
        }
        chatroom_data (char *topic_name)
        {
            // participant
            em.createParticipant ("");

            // create types
            chatroomTypeSupport_var T = new chatroomTypeSupport ();
            em.registerType ( T.in() );

            // create Topic
            em.createTopic ( topic_name );

            // create Publisher
            em.createPublisher ();

            // create dataWriter
            em.createWriter ( false );
            DataWriter_var dwriter = em.getWriter ();
            Writer = chatroomDataWriter::_narrow ( dwriter.in() );

            //create Subscriber
            em.createSubscriber ();

            // create DataReader
            em.createReader ();
            DataReader_var dreader = em.getReader ();
            Reader = chatroomDataReader::_narrow( dreader.in () );
            checkHandle( Reader.in (), "MsgDataReader::_narrow" );
        }
        ~chatroom_data ()
        {
            // Remove the DataWriters.
            em.deleteWriter ();

            // Remove the DataReaders
            em.deleteReader ();

            // Remove the Publisher.
            em.deletePublisher ();

            // Remove the Subscriber.
            em.deleteSubscriber ();

            // Remove the Topics.
            em.deleteTopic ();

            // Remove Participant.
            em.deleteParticipant ();
        }
};

typedef std::vector<message> message_list_t;

class message_data
{
    private:
        DDSEntityManager em;
        messageDataWriter_var Writer;
        messageDataReader_var Reader;
    public:
        void send ( message message_instance )
        {
            ReturnCode_t status = Writer->write(message_instance, DDS::HANDLE_NIL);
            checkStatus(status, "Writer::write");
        }
        void recv ( message_list_t* messageList )
        {
            // if any data is available, it returns a list
            assert ( messageList );
            messageSeq msgList;
            SampleInfoSeq infoSeq;
            ReturnCode_t status =  - 1;
            status = Reader->take ( msgList, infoSeq, LENGTH_UNLIMITED,
                    ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE );
            checkStatus(status, "msgDataReader::take");
            for (DDS::ULong j = 0; j < msgList.length(); j++)
            {
                messageList->push_back ( msgList[j] );
            }
            status = Reader->return_loan ( msgList, infoSeq );
            checkStatus(status, "MsgDataReader::return_loan");
        }
        message_data (char *topic_name)
        {
            // participant
            em.createParticipant ("");

            // create types
            messageTypeSupport_var T = new messageTypeSupport ();
            em.registerType ( T.in() );

            // create Topic
            em.createTopic ( topic_name );

            // create Publisher
            em.createPublisher ();

            // create dataWriter
            em.createWriter ( false );
            DataWriter_var dwriter = em.getWriter ();
            Writer = messageDataWriter::_narrow ( dwriter.in() );

            //create Subscriber
            em.createSubscriber ();

            // create DataReader
            em.createReader ();
            DataReader_var dreader = em.getReader ();
            Reader = messageDataReader::_narrow( dreader.in () );
            checkHandle( Reader.in (), "MsgDataReader::_narrow" );
        }
        ~message_data ()
        {
            // Remove the DataWriters.
            em.deleteWriter ();

            // Remove the DataReaders
            em.deleteReader ();

            // Remove the Publisher.
            em.deletePublisher ();

            // Remove the Subscriber.
            em.deleteSubscriber ();

            // Remove the Topics.
            em.deleteTopic ();

            // Remove Participant.
            em.deleteParticipant ();
        }
};

typedef std::vector<user> user_list_t;

class user_data
{
    private:
        DDSEntityManager em;
        userDataWriter_var Writer;
        userDataReader_var Reader;
    public:
        void send ( user userInstance )
        {
            ReturnCode_t status = Writer->write(userInstance, DDS::HANDLE_NIL);
            checkStatus(status, "Writer::write");
        }
        void recv ( user_list_t* userList )
        {
            // if any data is available, it returns a list
            assert ( userList );
            userSeq msgList;
            SampleInfoSeq infoSeq;
            ReturnCode_t status =  - 1;
            status = Reader->take ( msgList, infoSeq, LENGTH_UNLIMITED,
                    ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE );
            checkStatus(status, "msgDataReader::take");
            for (DDS::ULong j = 0; j < msgList.length(); j++)
            {
                userList->push_back ( msgList[j] );
            }
            status = Reader->return_loan ( msgList, infoSeq );
            checkStatus(status, "MsgDataReader::return_loan");
        }
        user_data (char *topic_name)
        {
            // participant
            em.createParticipant ("");

            // create types
            userTypeSupport_var T = new userTypeSupport ();
            em.registerType ( T.in() );

            // create Topic
            em.createTopic ( topic_name );

            // create Publisher
            em.createPublisher ();

            // create dataWriter
            em.createWriter ( false );
            DataWriter_var dwriter = em.getWriter ();
            Writer = userDataWriter::_narrow ( dwriter.in() );

            //create Subscriber
            em.createSubscriber ();

            // create DataReader
            em.createReader ();
            DataReader_var dreader = em.getReader ();
            Reader = userDataReader::_narrow( dreader.in () );
            checkHandle( Reader.in (), "MsgDataReader::_narrow" );
        }
        ~user_data ()
        {
            std::cout << "deconstructor" << '\n';
            // Remove the DataWriters.
            em.deleteWriter ();

            // Remove the DataReaders
            em.deleteReader ();

            // Remove the Publisher.
            em.deletePublisher ();

            // Remove the Subscriber.
            em.deleteSubscriber ();

            // Remove the Topics.
            em.deleteTopic ();

            // Remove Participant.
            em.deleteParticipant ();
        }
};

bool exit_flag = false;
GUI * gui = NULL;
chatroom_data * chat_room;
user_data * cur_user;
message_data * global_message;

void ctrlc ( int dummy )
{
    exit_flag = true;
}

void * pubThread(void * dummy) {
    char * username = gui->requestName();
    global_message = new message_data((char*)"temporary");
    cur_user = new user_data(username);

    while(!exit_flag) {
        char * temp_buffer = gui->getInput();

        if(strlen(temp_buffer) > 0) {
            message * message_instance = new message();

            strncpy(message_instance->message, temp_buffer, sizeof(message_instance->message));

            message_instance->uuid = 123; //TODO: Need to generate uuid of user
            message_instance->chatroom_idx = 0;
            message_instance->cksum = 0;

            global_message->send(*message_instance);

            delete message_instance;
        }

        gui->addMessage(username, temp_buffer);

        free(temp_buffer);
    }
}

void * subThread(void * dummy) {

}

int main()
{
    pthread_t pub_thread, sub_thread;

    gui = new GUI();
    // set up ctrl-c handler
    signal ( SIGINT, ctrlc );

    pthread_create(&pub_thread, NULL, &pubThread, NULL);
    pthread_create(&sub_thread, NULL, &subThread, NULL);

    // instantiate classes
    chatroom_data chatRoom ( (char*) "chatroom" );
    user_data User ( (char*) "user" );
    message_data Message ( (char*) "msg" );

    pthread_join(pub_thread, NULL);

    pthread_join(sub_thread, NULL);

    std::cout << "normal exit" << '\n';

    delete gui;

    return 0;
}
