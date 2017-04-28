
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
//Timeout time for the chatroom name in seconds (300 / 60 = 5 minutes)
#define CHATROOM_NAME_TIMEOUT 300

//Global vectors and variables
vector<User> listOfUsers;
vector<Chatroom> listOfChatrooms;
User localUser;
GUI * gui = new GUI();

int numOfUsers = 0;  //Keeps track of the number of users in listOfUsers vector
bool RUNNING = 1;  //Keeps loops running and allows them to terminame if set to 0
bool printingUsers = 0;  //Keep track if we are activly printing users
bool printingChatrooms = 0;  //Keep track if we are activly printing chatrooms
long double timeWatchingUsers = 0;  //Counts time passed

//Publishes a chatroom with the given idx and name
void nameChatroom(int i, char* CRname)
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

  ReturnCode_t status =  - 1;

  while (RUNNING) // Run until RUNNING is set to 0
  {
    //Get any published messages and iterate through them
    status = messageReader->take(messageList, infoSeq, LENGTH_UNLIMITED,
      ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "messageDataReader::take");
    for (DDS::ULong j = 0; j < messageList.length(); j++)
    {
      //Ignore message if it is not sent to local user's chatroom
      if(messageList[j].chatroom_idx == localUser.chatroom_idx)
      {
        //Find the user that sent the message
        for(int i = 0; i < numOfUsers; i++)
        {
          if(listOfUsers[i].uuid == messageList[j].uuid)
          {
            //Once user is found print the message
            gui->addMessage(listOfUsers[i].nick, messageList[j].message);
            break;
          }
        }
      }
    }
    status = messageReader->return_loan(messageList, infoSeq);
    checkStatus(status, "messageDataReader::return_loan");

    //Get any published chatrooms and iterate through them
    status = chatroomReader->take(chatroomList, infoSeq, LENGTH_UNLIMITED,
      ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "chatroomDataReader::take");
    for (DDS::ULong j = 0; j < chatroomList.length(); j++)
    {
      //Ignore chatroom if the index is out of bounds.
      if(chatroomList[j].chatroom_idx >= 1 && chatroomList[j].chatroom_idx <= 9)
      {
        //Set timeEmpty so the chatroom name will clear after 5 minutes of
        //being empty.
        listOfChatrooms[chatroomList[j].chatroom_idx].timeEmpty = timeWatchingUsers;
        //Update the name of the changed chatroom
        strncpy(listOfChatrooms[chatroomList[j].chatroom_idx].chatroom_name, chatroomList[j].chatroom_name, CHATROOM_NAME_MAX);
        //If we are actively displaying the chatrooms, update that list
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
  //Iterate through the list of users and print them
  for(int i = 0; i < numOfUsers; i++)
  {
    gui->addUser(listOfUsers[i].chatroom_idx, listOfUsers[i].nick, listOfUsers[i].online, !i);
   }
  //Keep track that we are activly printing users
  printingUsers = 1;
  printingChatrooms = 0;
}

void *pub(void* trash)
{
  char *input;

  ReturnCode_t status;
  DDSEntityManager messageMgr;

  // create domain participant
  messageMgr.createParticipant("SuperChat message");

  //create type
  messageTypeSupport_var messageTS = new messageTypeSupport();
  messageMgr.registerType(messageTS.in());

  //create Topic
  char messagetopic_name[] = "SuperChat_message";
  messageMgr.createTopic(messagetopic_name);

  //create Publisher
  messageMgr.createPublisher();

  // create DataWriter :
  // If autodispose_unregistered_instances is set to true (default value),
  // you will have to start the subscriber before the publisher
  bool autodispose_unregistered_instances = false;
  messageMgr.createWriter(autodispose_unregistered_instances);

  // Publish Events
  DataWriter_var messagewriter = messageMgr.getWriter();

  messageDataWriter_var messageWriter = messageDataWriter::_narrow(messagewriter.in());

  message messageInstance; /* Example on Stack */

  //Set checksum to 0 and ignore it
  messageInstance.cksum = 0;

  while(RUNNING)
  {
    //Get input from the user
    input = gui->getInput();

    //Check all possible user commands, if match is found execute the command

    //Change ChatRoom
    if(input[0] == ':' && input[1] == 'C' && input[2] == 'C' && input[3] == 'R' && input[4] == ' ' && input[6] == ' ')
    {
      //Convert char number to int
      int num = input[5] - '0';
      //Ignore if it is not a valid int
      //or if they have entered more than one int
      if(num >= 0 && num <= 9 && input[4] == ' ' && input[6] == ' ')
      {
        //Update local user's chatroom index
        localUser.chatroom_idx = num;
      }
    }
    //Name ChatRoom
    else if(input[0] == ':' && input[1] == 'N' && input[2] == 'C' && input[3] == 'R')
    {
      int num = input[5] - '0';  //Convert char number to int
      char CRname[CHATROOM_NAME_MAX];
      //Ignore if it is not a valid int
      //or if they have entered more than one int
      if(num <= 9 && num >= 1 && input[4] == ' ' && input[6] == ' ')
      {
        //Collect the rest of the input from the user
        for(int i = 0; i < CHATROOM_NAME_MAX; i++)
        {
          CRname[i] = input[i + 7];
          if(CRname[i] == '\n')
          { CRname[i] = '\0'; break; }
        }
        //Publish a chatroom with the given index and name
        nameChatroom(num, CRname);
      }
    }
    //Print ChatRooms
    else if(input[0] == ':' && input[1] == 'P' && input[2] == 'C' && input[3] == 'R')
    {
      //Iterate through the chatrooms and print them
      for(int i = 0; i < CHATROOMS_MAX; i++)
      {
        gui->addChatroom(listOfChatrooms[i].chatroom_idx, listOfChatrooms[i].chatroom_name, !i);
      }
      //Keep track that we are activly printing the chatrooms
      printingChatrooms = 1;
      printingUsers = 0;
    }
    //Print UseRs
    else if(input[0] == ':' && input[1] == 'P' && input[2] == 'U' && input[3] == 'R')
    {
      //Prints the list of users
      printUsers();
    }
    //Print HelP
    else if(input[0] == ':' && input[1] == 'P' && input[2] == 'H' && input[3] == 'P')
    {
      //Prints the user commands
      gui->printHelp();
      //Keep track that we are not activly printing chatrooms or users
      printingUsers = 0;
      printingChatrooms = 0;
    }
   //Change NicK
    else if(input[0] == ':' && input[1] == 'C' && input[2] == 'N' && input[3] == 'K')
    {
      char tempNick[NICK_SIZE_MAX];
      //Collect the input from the user
      for(int i = 0; i < NICK_SIZE_MAX; i++)
      {
        tempNick[i] = input[i + 5];
        if(tempNick[i] == '\n')
        { tempNick[i] = '\0'; break; }
      }
      //Update the local user nick
      strncpy(localUser.nick, tempNick, NICK_SIZE_MAX);
      //Open the superchatdata.txt file and update the default nick
      ofstream writer;
      writer.open("superchatdata.txt");
      writer << localUser.uuid;
      writer << " ";
      writer << localUser.nick;
      writer << "\n";
      writer.close();
    }
    //EXiT
    else if(input[0] == ':' && input[1] == 'E' && input[2] == 'X' && input[3] == 'T')
    {
      //Terminate all the loops
      RUNNING = 0;
    }
    //Regular message
    else
    {
      //Prepare messageInstance,
      strncpy(messageInstance.message, input, MESSAGE_SIZE_MAX);
      messageInstance.uuid = localUser.uuid;
      messageInstance.chatroom_idx = localUser.chatroom_idx;
      //and publish the message
      status = messageWriter->write(messageInstance, DDS::HANDLE_NIL);
      checkStatus(status, "messageDataWriter::write");
    }
    //Clear the input array for future messages
    memset(input, 0, MESSAGE_SIZE_MAX);

  }

  /* Remove the DataWriters */
  messageMgr.deleteWriter();
  /* Remove the Publisher. */
  messageMgr.deletePublisher();
  /* Remove the Topics. */
  messageMgr.deleteTopic();
  /* Remove Participant. */
  messageMgr.deleteParticipant();

  return 0;
}
//Continuously send local user info so that
//other versions of SuperChat know we are present
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
    //Set up the userInstance incase anything has changed since last time
    strncpy(userInstance.nick, localUser.nick, NICK_SIZE_MAX);
    userInstance.chatroom_idx = localUser.chatroom_idx;
    userInstance.uuid = localUser.uuid;
    //Publish user info
    status = userWriter->write(userInstance, DDS::HANDLE_NIL);
    checkStatus(status, "userDataWriter::write");
    //Wait for the predetermined time
    sleep(USER_UPDATE_PERIOD);
  }

  //Cleanup
  userManagerP.deleteWriter();
  userManagerP.deletePublisher();
  userManagerP.deleteTopic();
  userManagerP.deleteParticipant();
}

//Continuously recieve user info to keep track of present users
//Also clear chatroom names when the chatroom is empty for too long
void *watchUsers(void* trash)
{
  os_time delay_2ms = { 0, 2000000 };
  os_time delay_200ms = { 0, 200000000 };
  int check = 0;
  bool match = 0;
  //Create blank chatroom name to compare with
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
    //Get any published user info and iterate through it
    status = userReader->take(userList, infoSeq, LENGTH_UNLIMITED,
      ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    checkStatus(status, "userDataReader::take");

    for (DDS::ULong j = 0; j < userList.length(); j++)
    {
      match = 0;
      //Iterate through the list of users until a match is found
      for(int i = 0; i < numOfUsers && !match; i++)
      {
        //User info belongs to a user on our list, aka a match is found
        if(listOfUsers[i].uuid == userList[j].uuid)
        {
          match = 1;
          //Update the time we last heard from this user
          listOfUsers[i].lastHeard = timeWatchingUsers;
          //If this user just came back online,
          if(listOfUsers[i].online == 0)
          {
            listOfUsers[i].online = 1;
            //Print a message to the user,
            char msg[] = {"Has Come Online"};
            gui->addMessage(listOfUsers[i].nick, msg);
            listOfUsers[i].chatroom_idx = 0;
            //And if we are activly printing users update the list
            if(printingUsers)
            { printUsers(); }
          }
          //If their nick has changed,
          if(strcmp(listOfUsers[i].nick, userList[j].nick))
          {
            //Update their nick in our list
            strncpy(listOfUsers[i].nick, userList[j].nick, NICK_SIZE_MAX);
            //And if we are activly printing users update the list
            if(printingUsers)
            { printUsers(); }
          }
          //If they have changed chatrooms,
          if(listOfUsers[i].chatroom_idx != userList[j].chatroom_idx)
          {
            //Decrement the number of users in the chatroom they were in
            listOfChatrooms[listOfUsers[i].chatroom_idx].numUsers--;
            //And if the number of users in that chatroom reached 0
            if(listOfChatrooms[listOfUsers[i].chatroom_idx].numUsers == 0)
            {
              //Set the time the chatroom became empty
              listOfChatrooms[listOfUsers[i].chatroom_idx].timeEmpty = timeWatchingUsers;
            }
            //If the user was in the same chatroom as the local user
            if(listOfUsers[i].chatroom_idx == localUser.chatroom_idx)
            {
              //Print a message to the user indicating that user left
              char msg[] = {"Has Left the Chatroom"};
              gui->addMessage(listOfUsers[i].nick, msg);
            }
            //Update that user's chatroom in our list
            listOfUsers[i].chatroom_idx = userList[j].chatroom_idx;
            //Increment the number of users in that chatroom
            listOfChatrooms[listOfUsers[i].chatroom_idx].numUsers++;
            //If that user entered the local user's chatroom
            if(listOfUsers[i].chatroom_idx == localUser.chatroom_idx)
            {
              //Print a message to the user indicating that user is in their chatroom
              char msg[] = {"Has Entered the Chatroom"};
              gui->addMessage(listOfUsers[i].nick, msg);
            }
            //If we are activly printing users update the list
            if(printingUsers)
            { printUsers(); }
          }
        }
      }
      //If there was no match there is a new user online
      if(!match)
      {
        //Create a temporary struct user,
        User tempUser;
        //And set up its members
        strncpy(tempUser.nick, userList[j].nick, NICK_SIZE_MAX);
        tempUser.uuid = userList[j].uuid;
        //If the chatroom index is valid,
        if(userList[j].chatroom_idx >= 0 && userList[j].chatroom_idx <= 9)
        {
          //Set the chatroom index to the temporary user
          tempUser.chatroom_idx = userList[j].chatroom_idx;
        }
        else
        {
          //Set the chatroom index to default 0
          tempUser.chatroom_idx = 0;
        }
        //Increment the number of users in the chatroom
        //in which the new user is in
        listOfChatrooms[tempUser.chatroom_idx].numUsers++;
        tempUser.online = 1;  //Mark them as online
        tempUser.lastHeard = timeWatchingUsers;  //Set the last time we heard from them
        listOfUsers.push_back(tempUser); //Add them to our list of users
        numOfUsers++;  //Increment our total number of users
        //Print a message to the local user that this user has come online
        char msg[] = {"Has Come Online"};
        gui->addMessage(tempUser.nick, msg);
        //If we are activly printing users update the list
        if(printingUsers)
        { printUsers(); }
      }
    }

    status = userReader->return_loan(userList, infoSeq);
    checkStatus(status, "userDataReader::return_loan");

    //Every 5 seconds check the list of users for offline users
    //and check the list of chatrooms for empty chatrooms
    if(check == 50)
    {
      check = 0;
      //Iterate through the list of users
      for(int i = 0; i < numOfUsers; i++)
      {
        //If a user is marked as online,
        //but we have not heard from them in too long
        if(listOfUsers[i].online && ((timeWatchingUsers - 5) >= listOfUsers[i].lastHeard))
        {
          //Mark them as offline
          listOfUsers[i].online = 0;
          //And print a message to the local user
          char msg[] = {"Has Gone Offline"};
          gui->addMessage(listOfUsers[i].nick, msg);
          //If we are activly printing users, update the list
          if(printingUsers)
          { printUsers(); }
        }
      }
      //Iterate through the list of chatrooms
      for(int i = 1; i < CHATROOMS_MAX; i++)
      {
        //If a chatroom is empty and it has been empty for too long,
        if(listOfChatrooms[i].numUsers == 0 && ((timeWatchingUsers - CHATROOM_NAME_TIMEOUT) >= listOfChatrooms[i].timeEmpty))
        {
          //And its name is not already empty,
          if(strcmp(listOfChatrooms[i].chatroom_name, CRname))
          {
            //Set the chatroom's name to empty
            nameChatroom(i, CRname);
          }
        }
      }
    }

    //Increase the time that has passed watching users
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
  //Create CHATROOMS_MAX number of chatrooms in the list of chatrooms
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
  char *guiNick = (char*) malloc(sizeof(char) * 8);
  char fileNick[NICK_SIZE_MAX];

  //Open the superchatdata.txt file
  ofstream writer;
  ifstream file;
  file.open("superchatdata.txt");

  //If the open fails there must not be a file
  if(file.fail())
  {
    //Request a nick from the local user
    guiNick = gui->requestName();
    //Generate a unique identifier for the local user
    x = generateUUID();
    //Write the data to the superchatdata.txt file for future use
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
    //Set the local user's nick
    strncpy(localUser.nick, guiNick, NICK_SIZE_MAX);
  }
  else //The open succeeds so there is a file to read from
  {
    //Get the uuid from file
    file >> x;
    file.get();
    //Get the nick from file
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
  //Set the local user's nick
  strncpy(localUser.nick, fileNick, NICK_SIZE_MAX);
  }

  file.close();
  //Set the local user's uuid and chatroom idx
  localUser.uuid = x;
  localUser.chatroom_idx = 0;
  //Add the local user to the list of users and increment the number of users
  listOfUsers.push_back(localUser);
  numOfUsers++;
  listOfUsers[0].online = 1;
}

int main()
{
  pthread_t pub_thread, sub_thread, userinfo, getuserinfo;
  int trash;
  
  //Set up the local list of chatrooms
  initializeChatrooms();
  //Set up the local user
  initializeLocalUser();
  //Print the user commands to the local user
  gui->printHelp();

  //Create threads to execute SuperChat
  pthread_create(&userinfo, NULL, sendUserInfo, (void *) &trash);
  pthread_create(&getuserinfo, NULL, watchUsers, (void *) &trash);
  pthread_create(&pub_thread, NULL, &pub, (void *) &trash);
  pthread_create(&sub_thread, NULL, &sub, (void *) &trash);

  //Wait for the threads to terminate
  //Caused by the user exiting SuperChat
  pthread_join(pub_thread, NULL);
  pthread_join(sub_thread, NULL);
  pthread_join(userinfo, NULL);
  pthread_join(getuserinfo, NULL);
  //Close the gui
  gui->~GUI();

  return 0;
}
