
/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2013 PrismTech
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE
 *
 *   for full copyright notice and license terms.
 *
 */

/************************************************************************
 * LOGICAL_NAME:    HelloWorldDataSubscriber.cpp
 * FUNCTION:        OpenSplice HelloWorld example code.
 * MODULE:          HelloWorld for the C++ programming language.
 * DATE             September 2010.
 ************************************************************************
 *
 * This file contains the implementation for the 'HelloWorldDataSubscriber' executable.
 *
 ***/
//This file was Modified by Tyler Gustaf on 3/24/17
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "DDSEntityManager.h"
#include "ccpp_SuperChat.h"
#include "os.h"
#include "GUI/gui.h"
#include <fstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <ncurses.h>
#include "globals.h"

using namespace DDS;
using namespace SuperChat;

#define MESSAGE_SIZE_MAX 144
#define CHATROOM_NAME_MAX 25
#define NICK_SIZE_MAX 8
#define CHATROOMS_MAX 10
#define USER_UPDATE_PERIOD 2.5


vector<User> listOfUsers;
vector<Chatroom> listOfChatrooms;
User localUser;
GUI * gui = new GUI();

int numOfUsers = 0;
bool RUNNING = 1;
bool printingUsers = 0;
bool printingChatrooms = 0;


void *sub(void* trash)
{
  os_time delay_2ms = { 0, 2000000 };
  os_time delay_200ms = { 0, 200000000 };
  messageSeq messageList;
  chatroomSeq chatroomList;
  SampleInfoSeq infoSeq;

  DDSEntityManager messageMgr;
  DDSEntityManager CRmanager;

  // create domain participant
  messageMgr.createParticipant("SuperChat message");
  CRmanager.createParticipant("SuperChat chatroom");

  //create type
  messageTypeSupport_var messageTS = new messageTypeSupport();
  messageMgr.registerType(messageTS.in());

  chatroomTypeSupport_var chatroomTS = new chatroomTypeSupport();
  CRmanager.registerType(chatroomTS.in());

  //create Topic
  char messagetopic_name[] = "SuperChat_message";
  messageMgr.createTopic(messagetopic_name);
  char chatroomtopic_name[] = "SuperChat_chatroom";
  CRmanager.createTopic(chatroomtopic_name);

  //create Subscriber
  messageMgr.createSubscriber();
  CRmanager.createSubscriber();

  // create DataReader
  messageMgr.createReader();
  CRmanager.createReader();

  DataReader_var messagereader = messageMgr.getReader();
  DataReader_var chatroomreader = CRmanager.getReader();

  messageDataReader_var messageReader = messageDataReader::_narrow(messagereader.in());
  checkHandle(messageReader.in(), "messageDataReader::_narrow");
  chatroomDataReader_var chatroomReader = chatroomDataReader::_narrow(chatroomreader.in());
  checkHandle(chatroomReader.in(), "chatroomDataReader::_narrow");

  //cout << "=== [Subscriber] Ready ..." << endl;

  ReturnCode_t status =  - 1;

  while (RUNNING) // We dont want the example to run indefinitely
  {
    status = messageReader->take(messageList, infoSeq, LENGTH_UNLIMITED,
      ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "messageDataReader::take");
    for (DDS::ULong j = 0; j < messageList.length(); j++)
    {
      if(messageList[j].chatroom_idx == localUser.chatroom_idx)
      {
        for(int i = 0; i < numOfUsers; i++)
        {
          if(listOfUsers[i].uuid == messageList[j].uuid)
          {
            gui->addMessage(listOfUsers[i].nick, messageList[j].message);
            break;
          }
        }
      }
    }
    status = messageReader->return_loan(messageList, infoSeq);
    checkStatus(status, "messageDataReader::return_loan");

    status = chatroomReader->take(chatroomList, infoSeq, LENGTH_UNLIMITED,
      ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "chatroomDataReader::take");
    for (DDS::ULong j = 0; j < chatroomList.length(); j++)
    {
      if(chatroomList[j].chatroom_idx > 0 && chatroomList[j].chatroom_idx <= 9)
      {
        strncpy(listOfChatrooms[chatroomList[j].chatroom_idx].chatroom_name, chatroomList[j].chatroom_name, CHATROOM_NAME_MAX);
        if(printingChatrooms)
        {
          for(int i = 0; i < CHATROOMS_MAX; i++)
          {
            gui->addChatroom(listOfChatrooms[i].chatroom_idx, listOfChatrooms[i].chatroom_name, !i);
          }
        }
      }

    }
    status = chatroomReader->return_loan(chatroomList, infoSeq);
    checkStatus(status, "chatroomDataReader::return_loan");

    os_nanoSleep(delay_200ms);
  }

  os_nanoSleep(delay_2ms);

  //cleanup
  messageMgr.deleteReader();
  messageMgr.deleteSubscriber();
  messageMgr.deleteTopic();
  messageMgr.deleteParticipant();

  CRmanager.deleteReader();
  CRmanager.deleteSubscriber();
  CRmanager.deleteTopic();
  CRmanager.deleteParticipant();

  return 0;
}

void printUsers()
{
  for(int i = 0; i < numOfUsers; i++)
  {
    gui->addUser(listOfUsers[i].chatroom_idx, listOfUsers[i].nick, listOfUsers[i].online, !i);
   }
  printingUsers = 1;
  printingChatrooms = 0;
}

void *pub(void* trash)
{
  int j = 0;
  char *input;

  ReturnCode_t status;
  DDSEntityManager messageMgr;
  DDSEntityManager CRmanager;

  // create domain participant
  messageMgr.createParticipant("SuperChat message");
  CRmanager.createParticipant("SuperChat chatroom");

  //create type
  messageTypeSupport_var messageTS = new messageTypeSupport();
  messageMgr.registerType(messageTS.in());

  chatroomTypeSupport_var chatroomTS = new chatroomTypeSupport();
  CRmanager.registerType(chatroomTS.in());

  //create Topic
  char messagetopic_name[] = "SuperChat_message";
  messageMgr.createTopic(messagetopic_name);
  char chatroomtopic_name[] = "SuperChat_chatroom";
  CRmanager.createTopic(chatroomtopic_name);

  //create Publisher
  messageMgr.createPublisher();
  CRmanager.createPublisher();

  // create DataWriter :
  // If autodispose_unregistered_instances is set to true (default value),
  // you will have to start the subscriber before the publisher
  bool autodispose_unregistered_instances = false;
  messageMgr.createWriter(autodispose_unregistered_instances);
  CRmanager.createWriter(autodispose_unregistered_instances);

  // Publish Events
  DataWriter_var messagewriter = messageMgr.getWriter();
  DataWriter_var chatroomwriter = CRmanager.getWriter();

  messageDataWriter_var messageWriter = messageDataWriter::_narrow(messagewriter.in());
  chatroomDataWriter_var chatroomWriter = chatroomDataWriter::_narrow(chatroomwriter.in());

  message messageInstance; /* Example on Stack */
  chatroom chatroomInstance;

  messageInstance.cksum = 0;

  //Publish Chatroom and User information
  //status = chatroomWriter->write(chatroomInstance, DDS::HANDLE_NIL);
  //checkStatus(status, "chatroomDataWriter::write");

  //cout << "===[Publisher] Ready ..." <<endl;
  while(RUNNING)
  {
    j = 0;
    input = gui->getInput();

    if(input[0] == ':' && input[1] == 'C' && input[2] == 'C' && input[3] == 'R')
    {
      int num = input[5] - '0';
      if(num >= 0 && num <= 9)
      {
        localUser.chatroom_idx = num;
      }
    }

    else if(input[0] == ':' && input[1] == 'N' && input[2] == 'C' && input[3] == 'R')
    {
      int num = input[5] - '0';
      if(num <= 9 && num >= 0)
      {
        chatroomInstance.chatroom_idx = num;
        for(int i = 0; i < CHATROOM_NAME_MAX; i++)
        {
          chatroomInstance.chatroom_name[i] = input[i + 7];
          if(chatroomInstance.chatroom_name[i] == '\n')
          { chatroomInstance.chatroom_name[i] = '\0'; break; }
        }
        status = chatroomWriter->write(chatroomInstance, DDS::HANDLE_NIL);
        checkStatus(status, "chatroomDataWriter::write");
      }
    }

    else if(input[0] == ':' && input[1] == 'P' && input[2] == 'C' && input[3] == 'R')
    {
      for(int i = 0; i < CHATROOMS_MAX; i++)
      {
        gui->addChatroom(listOfChatrooms[i].chatroom_idx, listOfChatrooms[i].chatroom_name, !i);
      }
      printingChatrooms = 1;
      printingUsers = 0;
    }

    else if(input[0] == ':' && input[1] == 'P' && input[2] == 'U' && input[3] == 'R')
    {
      printUsers();
    }

    else if(input[0] == ':' && input[1] == 'P' && input[2] == 'H' && input[3] == 'P')
    {
      gui->printHelp();
      printingUsers = 0;
      printingChatrooms = 0;
    }

    else if(input[0] == ':' && input[1] == 'C' && input[2] == 'N' && input[3] == 'K')
    {
      char tempNick[NICK_SIZE_MAX];
      for(int i = 0; i < NICK_SIZE_MAX; i++)
      {
        tempNick[i] = input[i + 5];
        if(tempNick[i] == '\n')
        { tempNick[i] = '\0'; break; }
      }  
      strncpy(localUser.nick, tempNick, NICK_SIZE_MAX); 
      ofstream writer;
      writer.open("superchatdata.txt");
      writer << localUser.uuid;
      writer << " ";
      writer << localUser.nick;
      writer << "\n";
      writer.close();
    }

    else if(input[0] == ':' && input[1] == 'E' && input[2] == 'X' && input[3] == 'T')
    {
      RUNNING = 0;
    }

    else
    {
      strncpy(messageInstance.message, input, MESSAGE_SIZE_MAX);
      messageInstance.uuid = localUser.uuid;
      messageInstance.chatroom_idx = localUser.chatroom_idx;

      status = messageWriter->write(messageInstance, DDS::HANDLE_NIL);
      checkStatus(status, "messageDataWriter::write");
    }

    memset(input, 0, MESSAGE_SIZE_MAX);

  }



  /* Remove the DataWriters */
  messageMgr.deleteWriter();
  CRmanager.deleteWriter();

  /* Remove the Publisher. */
  messageMgr.deletePublisher();
  CRmanager.deletePublisher();

  /* Remove the Topics. */
  messageMgr.deleteTopic();
  CRmanager.deleteTopic();

  /* Remove Participant. */
  messageMgr.deleteParticipant();
  CRmanager.deleteParticipant();

  return 0;
}


void clearChatroom(int i, char* CRname)
{
  ReturnCode_t status;
  DDSEntityManager CRmanager;

  // create domain participant
  CRmanager.createParticipant("SuperChat chatroom");

  //create type

  chatroomTypeSupport_var chatroomTS = new chatroomTypeSupport();
  CRmanager.registerType(chatroomTS.in());

  //create Topic
  char chatroomtopic_name[] = "SuperChat_chatroom";
  CRmanager.createTopic(chatroomtopic_name);

  //create Publisher
  CRmanager.createPublisher();

  // create DataWriter :
  // If autodispose_unregistered_instances is set to true (default value),
  // you will have to start the subscriber before the publisher
  bool autodispose_unregistered_instances = false;
  CRmanager.createWriter(autodispose_unregistered_instances);

  // Publish Events
  DataWriter_var chatroomwriter = CRmanager.getWriter();

  chatroomDataWriter_var chatroomWriter = chatroomDataWriter::_narrow(chatroomwriter.in());

  chatroom chatroomInstance;

  chatroomInstance.chatroom_idx = i;
  strncpy(chatroomInstance.chatroom_name, CRname, CHATROOM_NAME_MAX);

  status = chatroomWriter->write(chatroomInstance, DDS::HANDLE_NIL);
  checkStatus(status, "chatroomDataWriter::write");

  /*Clean Up*/
  CRmanager.deleteWriter();
  CRmanager.deletePublisher();
  CRmanager.deleteTopic();
  CRmanager.deleteParticipant();
}

void *sendUserInfo(void* trash)
{
  ReturnCode_t status;
  userSeq userList;
  SampleInfoSeq infoSeq;

  DDSEntityManager userManagerP;
  //Create Participant
  userManagerP.createParticipant("SuperChat user");
  //Create TypeSupport
  userTypeSupport_var userTS = new userTypeSupport();
  userManagerP.registerType(userTS.in());
  //Create Topic
  char usertopic_name[] = "SuperChat_user";
  userManagerP.createTopic(usertopic_name);
  //Create Publisher
  userManagerP.createPublisher();
  // create DataWriter :
  // If autodispose_unregistered_instances is set to true (default value),
  // you will have to start the subscriber before the publisher
  bool autodispose_unregistered_instances = false;
  userManagerP.createWriter(autodispose_unregistered_instances);
  //Create Data Writer
  DataWriter_var userwriter = userManagerP.getWriter();
  userDataWriter_var userWriter = userDataWriter::_narrow(userwriter.in());
  user userInstance;

  while(RUNNING)
  {
    strncpy(userInstance.nick, localUser.nick, NICK_SIZE_MAX);
    userInstance.chatroom_idx = localUser.chatroom_idx;
    userInstance.uuid = localUser.uuid;

    status = userWriter->write(userInstance, DDS::HANDLE_NIL);
    checkStatus(status, "userDataWriter::write");

    sleep(USER_UPDATE_PERIOD);
  }

  //Cleanup
  userManagerP.deleteWriter();
  userManagerP.deletePublisher();
  userManagerP.deleteTopic();
  userManagerP.deleteParticipant();


}

void *watchUsers(void* trash)
{
  os_time delay_2ms = { 0, 2000000 };
  os_time delay_200ms = { 0, 200000000 };
  int check = 0;
  bool match = 0;
  long double timeWatchingUsers = 0;
  char CRname[CHATROOM_NAME_MAX];
  memset(CRname, 0, CHATROOM_NAME_MAX);

  ReturnCode_t status;
  userSeq userList;
  SampleInfoSeq infoSeq;

  DDSEntityManager userManagerS;
  //Create Participant
  userManagerS.createParticipant("SuperChat user");
  //Create TypeSupport
  userTypeSupport_var userTS = new userTypeSupport();
  userManagerS.registerType(userTS.in());
  //Create Topic
  char usertopic_name[] = "SuperChat_user";
  userManagerS.createTopic(usertopic_name);
  //Create Subscriber
  userManagerS.createSubscriber();
  //Create Reader
  userManagerS.createReader();
  //Create Data Reader
  DataReader_var userreader = userManagerS.getReader();
  userDataReader_var userReader = userDataReader::_narrow(userreader.in());
  checkHandle(userReader.in(), "userDataReader::_narrow");


  while(RUNNING)
  {
    status = userReader->take(userList, infoSeq, LENGTH_UNLIMITED,
      ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "userDataReader::take");

    for (DDS::ULong j = 0; j < userList.length(); j++)
    {
      match = 0;
      for(int i = 0; i < numOfUsers && !match; i++)
      {
        if(listOfUsers[i].uuid == userList[j].uuid)
        {
          //cout << "MATCH\n";
          match = 1;
          listOfUsers[i].lastHeard = timeWatchingUsers;
          if(listOfUsers[i].online == 0)
          {
            char msg[] = {"Has Come Online"};
            gui->addMessage(listOfUsers[i].nick, msg);
            listOfUsers[i].chatroom_idx = 0;
            if(printingUsers)
            { printUsers(); }
          }
          listOfUsers[i].online = 1;
          if(strcmp(listOfUsers[i].nick, userList[j].nick))
          {
            strncpy(listOfUsers[i].nick, userList[j].nick, NICK_SIZE_MAX);
            if(printingUsers)
            { printUsers(); }
          }
          if(listOfUsers[i].chatroom_idx != userList[j].chatroom_idx)
          {
            listOfChatrooms[listOfUsers[i].chatroom_idx].numUsers--;
            if(listOfChatrooms[listOfUsers[i].chatroom_idx].numUsers == 0)
            {
              listOfChatrooms[listOfUsers[i].chatroom_idx].timeEmpty = timeWatchingUsers;
            }
            if(listOfUsers[i].chatroom_idx == localUser.chatroom_idx)
            {
              char msg[] = {"Has Left the Chatroom"};
              gui->addMessage(listOfUsers[i].nick, msg);
            }
            listOfUsers[i].chatroom_idx = userList[j].chatroom_idx;
            listOfChatrooms[listOfUsers[i].chatroom_idx].numUsers++;
            if(listOfUsers[i].chatroom_idx == localUser.chatroom_idx)
            {
              char msg[] = {"Has Entered the Chatroom"};
              gui->addMessage(listOfUsers[i].nick, msg);
            }
            if(printingUsers)
            { printUsers(); }
          }
        }
      }
      if(!match)
      {
        //cout << "NEW USER\n";
        User tempUser;
        strncpy(tempUser.nick, userList[j].nick, NICK_SIZE_MAX);
        tempUser.uuid = userList[j].uuid;
        if(userList[j].chatroom_idx >= 0 && userList[j].chatroom_idx <= 9)
        {
          tempUser.chatroom_idx = userList[j].chatroom_idx;
        }
        else
        {
          tempUser.chatroom_idx = 0;
        }
        listOfChatrooms[tempUser.chatroom_idx].numUsers++;
        tempUser.online = 1;
        tempUser.lastHeard = timeWatchingUsers;
        listOfUsers.push_back(tempUser);
        numOfUsers++;
        char msg[] = {"Has Come Online"};
        gui->addMessage(tempUser.nick, msg);
        if(printingUsers)
        { printUsers(); }
      }
    }

    status = userReader->return_loan(userList, infoSeq);
    checkStatus(status, "userDataReader::return_loan");

    if(check == 50)
    {
      check = 0;
      for(int i = 0; i < numOfUsers; i++)
      {
        if(listOfUsers[i].online && ((timeWatchingUsers - 5) >= listOfUsers[i].lastHeard))
        {
          //cout << "USER OFFLINE\n";
          listOfUsers[i].online = 0;
          char msg[] = {"Has Gone Offline"};
          gui->addMessage(listOfUsers[i].nick, msg);
          if(printingUsers)
          { printUsers(); }
        }
      }
      for(int i = 0; i < CHATROOMS_MAX; i++)
      {
        if(listOfChatrooms[i].numUsers == 0 && ((timeWatchingUsers - 25) >= listOfChatrooms[i].timeEmpty))
        {
          if(strcmp(listOfChatrooms[i].chatroom_name, CRname))
          {
            clearChatroom(i, CRname);
          }
        }
      }
    }

    timeWatchingUsers += 0.2;
    check += 2;
    os_nanoSleep(delay_200ms);

  }

  //Cleanup
  userManagerS.deleteReader();
  userManagerS.deleteSubscriber();
  userManagerS.deleteTopic();
  userManagerS.deleteParticipant();


}

void initializeChatrooms()
{
  //Create a temp chatroom to fill the chatroom vector
  Chatroom temp;
  temp.chatroom_idx = 0;
  temp.timeEmpty = 0;
  temp.numUsers = 0;
  char CRname[CHATROOM_NAME_MAX] = {"Public"};
  //Copy the CRname into the temp chatroom name
  strncpy(temp.chatroom_name, CRname, CHATROOM_NAME_MAX);
  listOfChatrooms.push_back(temp);
  //Clear CRname so that all other chatroom names are initialized to empty
  memset(CRname, 0, CHATROOM_NAME_MAX);
  strncpy(temp.chatroom_name, CRname, CHATROOM_NAME_MAX);

  for(int i = 1; i < CHATROOMS_MAX; i++)
  {
    temp.chatroom_idx = i;
    listOfChatrooms.push_back(temp);
  }
}

void initializeLocalUser()
{
  int j = 0;
  unsigned long long x;
  char *guiNick;
  char fileNick[NICK_SIZE_MAX];


  ofstream writer;
  ifstream file;
  file.open("superchatdata.txt");


  if(file.fail())
  {
      
      guiNick = gui->requestName();
      x = generateUUID();
      
      writer.open("superchatdata.txt");
      writer << x;
      writer << " ";
      writer << guiNick;
      //Remove any newline character in the Nick
      for(int i = 0; i < NICK_SIZE_MAX; i++)
      {
        if(guiNick[i] == '\n')
        {
          guiNick[i] = '\0';
          break;
        }
      }
      writer << "\n";
      writer.close();
      strncpy(localUser.nick, guiNick, NICK_SIZE_MAX);
  }
  else
  {
    file >> x;
    file.get();
    do
    {
      file.get(fileNick[j]);
      j++;
    }while(j < NICK_SIZE_MAX && (char)fileNick[j-1] != '\n');
    //Remove any newline character in the Nick
    for(int i = 0; i < NICK_SIZE_MAX; i++)
    {
      if(fileNick[i] == '\n')
      {
        fileNick[i] = '\0';
        break;
      }
    }
  strncpy(localUser.nick, fileNick, NICK_SIZE_MAX);
  }

  file.close();
  localUser.uuid = x;
  localUser.chatroom_idx = 0;
  listOfUsers.push_back(localUser);
  numOfUsers++;
  listOfUsers[0].online = 1;
}

int main()
{
  pthread_t pub_thread, sub_thread, userinfo, getuserinfo;
  int trash;
    
  initializeChatrooms();
  initializeLocalUser();

  
  gui->printHelp();
  pthread_create(&userinfo, NULL, sendUserInfo, (void *) &trash);
  pthread_create(&getuserinfo, NULL, watchUsers, (void *) &trash);
  pthread_create(&pub_thread, NULL, &pub, (void *) &trash);
  pthread_create(&sub_thread, NULL, &sub, (void *) &trash);


  pthread_join(pub_thread, NULL);
  pthread_join(sub_thread, NULL);
  pthread_join(userinfo, NULL);
  pthread_join(getuserinfo, NULL);

  gui->~GUI();

  return 0;
}
