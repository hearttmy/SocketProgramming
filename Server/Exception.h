#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
using namespace std;

class MyException {
private:
    string errMessage;

public:
    MyException(string str) : errMessage(str) {}
    string get() { return errMessage; }
};

#endif