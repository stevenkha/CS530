/*
    Steven Kha : 822700600
*/
#ifndef _OPCODE_
#define _OPCODE_

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

class objCode {
public:
    void setMap();
    bool checkBitSet(int num, int pos);
    int firstThreeNums(string objectCode);
    int getAddressingMode(string objectCode);
    int getAddressingType(string objectCode);
    int getFormat(string objectCode);
    int* getRegister(string objectCode);
    string getOpcode(string objectCode);
    string getMnemonic(string objectCode);
};
#endif