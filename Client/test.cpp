#include <iostream>
#include <string>

using namespace std;

int main() {
    char *c = "123123";
    {
        string str = c;
        str.erase(str.length() - 1, 1);
        cout << str << endl;
    }
    cout << c << endl;
}