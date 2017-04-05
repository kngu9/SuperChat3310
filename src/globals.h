#ifndef _H_GLOBALS_H_
#define _H_GLOBALS_H_

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#define CONFIG_FILE ".config"

#define MESSAGE_SIZE_MAX 144
#define CHATROOM_NAME_MAX 25
#define NICK_SIZE_MAX 8
#define CHATROOMS_MAX 10
#define USER_UPDATE_PERIOD 2.5

using namespace std;

struct Chatroom
{
  // an integer representing the checkroom index.
  unsigned long chatroom_idx;
  // name of the chatroom
  char chatroom_name[CHATROOM_NAME_MAX];
  // number of users in the chatroom
  int numUsers;
  //  time when the chatroom became empty
  long double timeEmpty;
};

struct User
{
  // users nickname
  char nick[NICK_SIZE_MAX];
  // unique identifier for a user.  contents TBD.
  unsigned long long uuid;
  // an integer representing the checkroom index.
  unsigned long chatroom_idx;
  // Previouse time heard from
  long double lastHeard;
  // User's online status
  bool online;
};

GUI * gui = new GUI();

long long generateUUID() {
    unsigned long long x;

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    memcpy(&x, &uuid, sizeof(x));

    return x;
}

User initializeLocalUser()
{
  int j = 0;
  unsigned long long x;
  char *guiNick;
  char fileNick[NICK_SIZE_MAX];
  User localUser;


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
  return localUser;
}

#endif //_H_GLOBALS_H_
