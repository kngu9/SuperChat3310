#include <iostream>
#include "../src/GUI/gui.h"
#include <./src/globals.h>

using namespace std;

void testGenerateUUID() {
    generateUUID();
}

int main() {
    cout << "Running Test Case 1: Properly Generate UUID" << endl;
    generateUUID();
}
