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
//Timeout time for the chatroom name in seconds (300 / 60 = 5 minutes)
#define CHATROOM_NAME_TIMEOUT 300

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



long long generateUUID() {
    unsigned long long x;

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    memcpy(&x, &uuid, sizeof(x));

    return x;
}


#endif //_H_GLOBALS_H_
