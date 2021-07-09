#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {

    string test = "TABLE " ;
    size_t pos = test.find(" ");
    cout << pos << endl;
    return 0;
}