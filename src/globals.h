#ifndef _H_GLOBALS_H_
#define _H_GLOBALS_H_

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#define CONFIG_FILE ".config"

using namespace std;

typedef struct settings_t {
    long long uuid;
    string username;
} settings;

long long generateUUID() {
    long long x;

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    memcpy(&x, &uuid, sizeof(x));

    return x;
}

void writeSettings(settings * s) {
    ofstream oFile;

    oFile.open(CONFIG_FILE);

    oFile << s->username;
    oFile << "\n";
    oFile << s->uuid;
    oFile << "\n";

    oFile.close();
}

settings * loadInitialSettings() {
    settings * initial_settings = (settings *)malloc(sizeof(settings));
    fstream inFile(CONFIG_FILE);

    if(inFile.good()) {
        //config file exists
        string line, username;
        long long uuid;

        getline(inFile, line);
        username = line;
        getline(inFile, line);
        uuid = atol(line.c_str());

        initial_settings->username = username;
        initial_settings->uuid = uuid;

        inFile.close();
    } else {
        initial_settings->uuid = generateUUID();
        initial_settings->username = NULL;
        writeSettings(initial_settings);
    }
    return initial_settings;
}

#endif //_H_GLOBALS_H_
