/*
    Steven Kha : 822700600
*/
#ifndef _DASM_
#define _DASM_
#include <stdio.h>
#include <fstream>
#include <vector>
#include <math.h>
#include <map>
#include "objCodeFunctions.h"

class Dis {
    private:
        void storeData();
        string formatLabels(string label, int offset);
        string formatMne(string mne);
        char getRegisterFromNum(int num);
        void gapFill(unsigned int currentAddress, unsigned int startAddress);
        void checkBase(string firstThree, int line, int cursor);
        string checkImmediate(unsigned int disp, string target, string firstThree, int format);
        bool checkLTORG(unsigned int currentAddress, int line, int cursor);
        void analyzeFormat2(string firstThree, int line, int cursor);
        void analyzeFormat3(string firstThree, int line, int cursor);
        void analyzeFormat4(string firstThree, int line, int cursor);
        void analyzeInstruction(string label, string firstThree, int traverse, int i, int cursor);
        void headerRecord();
        void textRecord(int i);
        void endRecord(int line);
    public:
        void parseFiles(string objFilename, string symFilename);
        void disassembly(); 
};

#endif