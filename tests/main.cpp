#include <iostream>
#include <vector>
#include <./src/globals.h>

using namespace std;

void testGenerateUUID() {
    long long a = generateUUID();

    if(a == 0) {
        cout << "Properly Generate UUID Failed! a is 0" << endl;
        return;
    }

    //Generate 1000 uuids
    vector<long long> uuids;

    for(int i = 0; i < 1000; i++) {
        uuids.push_back(generateUUID());
    }

    for(int i = 0; i < 1000; i++) {
        for(int j = i; j < 1000; j++) {
            if(uuids[i] == uuids[j]) {
                cout << "Properly Generate UUID Failed! Generated 1000 uuids and one or more of them were not unique!" << endl;
                return;
            }
        }
    }

    cout << "Properly Generate UUID Passed!" << endl;
}

int main() {
    cout << "Running Test Case 1: Properly Generate UUID" << endl;
    generateUUID();

    cout << "Ran all test cases!" << endl;
}
