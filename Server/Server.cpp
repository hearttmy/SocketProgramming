#include <winsock2.h>

#include <iostream>

#include "SocketManager.h"

using namespace std;

int main() {
    SocketManager::init();
    try {
        SocketManager sm;
        sm.socketSelect();
    } catch(MyException &e) {
        cout << e.get() << endl;
    } catch(...) {
        cout << "ERROR" << endl;
    }
}
