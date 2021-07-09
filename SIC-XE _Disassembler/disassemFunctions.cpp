/*
    Steven Kha : 822700600
*/

#include <iomanip>
#include "disassemFunctions.h"

ifstream objFileStream;
ifstream symFileStream;
ofstream outFileStream;

unordered_map<unsigned int, string> symMap;
unordered_map<unsigned int, string> litData;

int progLength;
objCode obj;
bool baseCheck = false;
bool insertLabel = false;
int indexValue;
unsigned int currentAddress;
unsigned int baseAddress;
vector<string> objFile;
vector<string> symFile;
vector<string> outFile;
vector<string> symNames;
vector<string> gapNames;
vector<unsigned int> gapAddress;
vector<unsigned int> symAddress;
vector<string> litSymbols;
vector<unsigned int> litAddress;

//store all the necessary data from the obj and sym files. Also store data for the vectors and unordered maps
void Dis::storeData() {
    int flag;
    obj.setMap();
    string line;
    while (getline(objFileStream, line)) {
        //remove any whitespace at the end of line
        while (line[line.length() - 1] == ' ') {
            line.erase(line.length() - 1);
        }

        objFile.push_back(line);
    }

    while (getline(symFileStream, line)) {
        symFile.push_back(line);
    }

    for (int i = 0; i < symFile.size(); i++) {
        if (symFile[i].empty()) {
            flag = i + 3;
        }
    }

    //only save the necesesary data in symbol table
    for (int i = 2; i < flag - 3; i++) {
        size_t pos = symFile[i].find(" ");
        symNames.push_back(symFile[i].substr(0, pos));
        symAddress.push_back(stoi(symFile[i].substr(8, 8), nullptr, 16));
    }
    
    //get literal symbol from the file and save in a vector
    for (int i = flag; i < symFile.size(); i++) {
        string litLine = symFile[i];
        size_t pos = litLine.find("=");
        size_t pos2 = litLine.find("' ");
        litSymbols.push_back(litLine.substr(pos, (pos2 - pos) + 1));
    }

    //get literal address from the file and save in a vector
    for (int i = flag; i < symFile.size(); i++) {
        string litLine = symFile[i];
        size_t pos = litLine.find("' ");
        litAddress.push_back(stoi(litLine.substr(pos + 12, 6), nullptr, 16));
    }

    //set symMap
    for (int i = 0; i < symNames.size(); i++) {
        symMap.insert(make_pair(symAddress[i], symNames[i]));
    }

    //set litData
    for (int i = 0; i < litSymbols.size(); i++) {
        litData.insert(make_pair(litAddress[i], litSymbols[i]));
    }
}

//parse through the obj file and sym file
void Dis::parseFiles(string objFileName, string symFileName) {
    objFileStream.open(objFileName);
    if (!objFileStream.is_open()) {
        cout << "The Object File was not found." << endl;
        exit(EXIT_FAILURE);
    }

    symFileStream.open(symFileName);
    if (!symFileStream.is_open()) {
        cout << "The Symbol File was not found." << endl;
        exit(EXIT_FAILURE);
    }

    outFileStream.open("out.lst");
    storeData();
    objFileStream.close();
    symFileStream.close();
}

string Dis::formatLabels(string label, int offset) {
    while (label.length() < offset) {
        label.append(" ");

    }
    
    return label;
}

string Dis::formatMne(string mne) {
    while (mne.length() < 5) {
        mne.append(" ");
    }

    return mne;
}

//return the char that corresponds with the register value
char Dis::getRegisterFromNum(int num) {
    switch (num) {
        case 0:
            return 'A';
            break;
        case 1:
            return 'X';
            break;
        case 2:
            return 'L';
            break;
        case 3:
            return 'B';
            break;
        case 4:
            return 'S';
            break;
        case 5:
            return 'T';
            break;
        case 6:
            return 'F';
            break;
    }

    return ' ';
}

//find the symbols that are used in between address jumps and their addresses
void Dis::gapFill(unsigned int currentAddress, unsigned int startAddress) {
    unsigned int wordSize = 0;
    string resw = "RESW";
    resw = formatMne(resw);
    //get the symbols that are used in the gap
    for (int i = 0; i < symMap.size(); i++) {
        if (currentAddress - 1 > symAddress[i] && symAddress[i] >= startAddress) {
            gapNames.push_back(symNames[i]);
            gapAddress.push_back(symAddress[i]);
        }
    }
    
    //iterate through the symbols within the gap and output their address and labels
    for (int i = 0; i < gapNames.size(); i++) {
        string symbol = gapNames[i];
        symbol = formatLabels(symbol, 9);
        if (i + 1 == gapNames.size()) {
            wordSize = (currentAddress - startAddress) / 3;
        } else {
            wordSize = (gapAddress[i + 1] - startAddress) / 3;
        }

        string size = to_string(wordSize);
        outFileStream << setfill('0') << setw(4) << gapAddress[i] << setfill(' ') << setw(5 + symbol.length()) << symbol
                << setw(1 + resw.length()) << resw << setw(3 + size.length()) << size << endl;
        startAddress = gapAddress[i + 1];
    }

    gapNames.clear();
    gapAddress.clear();
}

//checks if base register needs to be used
void Dis::checkBase(string firstThree, int line, int cursor) {
    string target = objFile[line].substr(cursor + 4, 4);
    target.insert(0, "0x");
    int TA = stoi(target, nullptr, 16);
    string symbol;
    if (symMap.find(TA) != symMap.end()) {
        symbol = symMap.find(TA) -> second;
        symbol = formatLabels(symbol, 10);
        if (obj.getMnemonic(firstThree) == "LDB") {
        outFileStream << setw(23) << "BASE" << setw(4 + symbol.length()) << symbol << endl;
        baseAddress = symMap.find(TA) -> first;
        }

        baseCheck = true;
    } 
}

//checks if the value needs to be loaded into a register
string Dis::checkImmediate(unsigned int disp, string target, string firstThree, int format) {
    string symbol;
    if (symMap.find(disp) == symMap.end() || disp == 0) {
        symbol = target.substr(format + 1, 1);
    }

    if (obj.getMnemonic(firstThree) == "LDX") {
        indexValue = stoi(symbol);
    }

    return symbol;
}

//check for LTORG insertion and output literals
bool Dis::checkLTORG(unsigned int currentAddress, int line , int cursor) {
    if (litData.find(currentAddress) != litData.end()) {
        string symbol = litData.find(currentAddress) -> second;
        size_t pos = symbol.find("'");
        string objectCode = symbol.substr(3, symbol.length() - 4);
        symbol = formatLabels(symbol, 10);
        string astr = formatMne("*");
        outFileStream << setw(24) << "LTORG" << endl;
        outFileStream << setfill('0') << setw(4) << currentAddress << setfill(' ') << setw(15 + astr.length()) << astr
                      << setw(3 + symbol.length()) << symbol << setw(7 + objectCode.length()) << objectCode << endl;
        return true;
    }

    return false;
}

void Dis::analyzeFormat2(string firstThree, int line, int cursor) {
    string target = objFile[line].substr(cursor, 4);
    string mnemonic = obj.getMnemonic(firstThree);
    int* regis = obj.getRegister(target);
    char registerName = getRegisterFromNum(regis[0]);
    mnemonic = formatMne(mnemonic);
    if (mnemonic == "CLEAR" && registerName == 'X') {
        indexValue = 0;
    }

    outFileStream << setw(15 + mnemonic.length()) << mnemonic
                  << setw(4) << registerName << setw(20) << target << endl;
}

void Dis::analyzeFormat3(string firstThree, int line, int cursor) {
    int addrType = obj.getAddressingType(firstThree);
    int addrMode = obj.getAddressingMode(firstThree);
    int format = obj.getFormat(firstThree);
    string mnemonic = obj.getMnemonic(firstThree);
    mnemonic = formatMne(mnemonic);
    string objectCode = objFile[line].substr(cursor, 6);
    string target = objFile[line].substr(cursor + 3, 3);
    string symbol;
    target.insert(0, "0x");
    int disp = stoi(target, nullptr, 16);
    
    //check for RSUB mnemonic and output its object code appropriately
    if (mnemonic == "RSUB ") {
        outFileStream << setw(20) << mnemonic << setw(26) << objectCode << endl;
    } else if (addrMode == 3) {
        //check if the immediate mode uses a direct value
        symbol = checkImmediate(disp, target, firstThree, format);
        symbol = formatLabels(symbol, 10);
        if (insertLabel) {
            outFileStream << " "  << mnemonic;
        } else {
            outFileStream << setw(15 + mnemonic.length()) << mnemonic;
        }

        /*
            output formatting
        */

        switch (addrMode) {
            //Indirect
            case 2:
                outFileStream << setw(3) << "@" << symbol << setw(7 + objectCode.length()) << objectCode << endl;
                break;
            //Immediate
            case 3:
                outFileStream << setw(3) << "#" << symbol << setw(7 + objectCode.length()) << objectCode << endl;
                break;
            default:
                outFileStream << setw(3 + symbol.length()) << symbol << setw(7 + objectCode.length()) << objectCode << endl;
        }

    } else {
        //perform two's comp to get sigend int value
        if (disp > 2047 || disp < -2048) {
            int x = log2(disp) + 1;
            for (int i = 0; i < x; i++) {
                disp = (disp ^ (1 << i));
            }

            disp += 1;
            disp *= -1;
        }

        switch (addrType) {
            //Direct
            case 1: 
                disp = disp;
                break;
            //Direct, Indexed
            case 10:
                disp += 1;
                break;
            //PC-Relative
            case 2:
                disp = disp + (currentAddress + obj.getFormat(firstThree));
                break;
            //PC-Relative, Indexed
            case 20:
                disp = disp + currentAddress + obj.getFormat(firstThree) + indexValue;
                break;
            //Base-Relative
            case 3:
                disp += baseAddress;
                break;
            //Base-Relative, Indexed
            case 30:
                disp = disp + baseAddress + indexValue;
                break;
        }

        //get symbol from the target address
        if (litData.find(disp) != litData.end()) {
            symbol = litData.find(disp) -> second;
        } else {
            symbol = symMap.find(disp) -> second;
            if (addrType == 20 || addrType == 30) {
                symbol.append(",X");
            }
        }

        symbol = formatLabels(symbol, 10);

        /*
            output formatting:
        */

        if (insertLabel) {
            outFileStream << " "  << mnemonic;
        } else {
            outFileStream << setw(15 + mnemonic.length()) << mnemonic;
        }
        
        switch (addrMode) {
            //Indirect
            case 2:
                outFileStream << setw(3) << "@" << symbol << setw(7 + objectCode.length()) << objectCode << endl;
                break;
            //Immediate
            case 3:
                outFileStream << setw(3) << "#" << symbol << setw(7 + objectCode.length()) << objectCode << endl;
                break;
            default:
                outFileStream << setw(3 + symbol.length()) << symbol << setw(7 + objectCode.length()) << objectCode << endl;
        }
    }
}

void Dis::analyzeFormat4(string firstThree, int line, int cursor) {
    string target = objFile[line].substr(cursor + 4, 4);
    target.insert(0, "0x");
    int addr = stoi(target, nullptr, 16);
    int addrMode = obj.getAddressingMode(firstThree);
    string symbol;

    if (symMap.find(addr) == symMap.end()) {
        symbol = litData.find(addr) -> second;
    } else {
        symbol = symMap.find(addr) -> second;
    }

    symbol = formatLabels(symbol, 10);
    string mnemonic = obj.getMnemonic(firstThree);
    mnemonic = formatMne(mnemonic);
    string objectCode = objFile[line].substr(cursor, 8);
    if (insertLabel) {
        outFileStream << "+" << mnemonic;
    } else {
        outFileStream << setw(10 + mnemonic.length()) << "+" << mnemonic;
    }

    /*
        output formatting:
    */

    switch (addrMode) {
        //Indirect
        case 2:
            outFileStream << setw(3) <<  "@" <<  symbol << setw(7 + objectCode.length()) << objectCode << endl;
            break;
        //Immediate
        case 3:
            outFileStream << setw(3) << "#" << symbol << setw(7 + objectCode.length()) << objectCode << endl;
            break;
        default:
            outFileStream << setw(3 + symbol.length()) << symbol << setw(7 + objectCode.length()) << objectCode << endl;       
            break;
    }
}

void Dis::analyzeInstruction(string label, string firstThree, int traverse, int i, int cursor) {
    //check if symbol needs to be inserted
    insertLabel = false;
    if (symMap.find(currentAddress) != symMap.end()) {
        label = symMap.find(currentAddress) -> second;
        label = formatLabels(label, 9);
        outFileStream << setfill('0') << setw(4) << hex << uppercase << currentAddress << setfill(' ') << setw(5 + label.length()) << label;
        insertLabel = true;
    } else {
        outFileStream << setfill('0') << setw(4) << hex << uppercase << currentAddress << setfill(' ');
    }
}

void Dis::headerRecord() {
    string start = "START";
    string fileName = objFile[0].substr(1, 6);
    fileName = formatLabels(fileName, 9);
    string lengthOfProgram = objFile[0].substr(14, 6);
    progLength = stoi(lengthOfProgram, nullptr, 16);
    lengthOfProgram.insert(0, "0x");
    currentAddress = stoi(objFile[0].substr(7, 6), nullptr, 16);
    outFileStream << objFile[0].substr(9, 4) << setw(5 + fileName.length()) << fileName << setw(9 + start.length()) << start << setw(4) << currentAddress << endl;
}

void Dis::textRecord(int i) {
    int traverse;
    //set start address to last known address
    unsigned int startAddress = currentAddress;
    //set current address to col 2-7 of text record
    currentAddress = stoi(objFile[i].substr(1, 6), nullptr, 16);
    //check for missing gaps needed to be filled between text records
    if (currentAddress - startAddress > 4) {
        gapFill(currentAddress, startAddress);
    } 

    //parse through text record line
    for (int cursor = 9; cursor < objFile[i].length(); cursor += traverse) {
        traverse = 0;
        string symLabel;
        string firstThree = objFile[i].substr(cursor, 3);
        //checks for LTORG insertion and updates data accordingly
        if (checkLTORG(currentAddress, i, cursor)) {
            string symbol = litData.find(currentAddress) -> second;
            int lengthSkip;
            size_t pos = symbol.find("'");
            lengthSkip = symbol.length() - 4;
            cursor += lengthSkip;
            currentAddress += lengthSkip/2;
        }

        if (!objFile[i].substr(cursor, 3).empty()) {
            firstThree = objFile[i].substr(cursor, 3);
        } else {
            //checks if cursor is at end of the line;
            firstThree = "ENDLINE";
        }
        
        if (firstThree != "ENDLINE") {
            analyzeInstruction(symLabel, firstThree, traverse, i, cursor);
            switch (obj.getFormat(firstThree)) {
                case 2:
                    analyzeFormat2(firstThree, i, cursor);
                    traverse += 4;
                    break;
                case 3:
                    analyzeFormat3(firstThree, i, cursor);
                    traverse += 6;
                    break;
                case 4:
                    analyzeFormat4(firstThree, i, cursor);
                    traverse += 8;
                    break;
                }

            //checks if BASE should be inserted
            if (!baseCheck) {
                checkBase(firstThree, i, cursor);
            }

            //update address
            currentAddress += obj.getFormat(firstThree);

            //check for gaps between the current address and program length
            if (cursor + traverse + 1 >= objFile[i].length() && objFile[i + 1].substr(0, 1) != "T" && progLength - currentAddress > 4) {
                gapFill(progLength, currentAddress);
                break;
            }
        } 
    }

}

void Dis::endRecord(int line) {
    string end = formatMne("END");
    currentAddress = stoi(objFile[line].substr(1, 6), nullptr, 16);
    string label = symMap.find(currentAddress) -> second;
    label = formatLabels(label, 10);
    outFileStream << setw(19 + end.length()) << end << setw(3 + label.length()) << label << endl;
}

void Dis::disassembly() {
    for (int i = 0; i < objFile.size(); i++) {
        char record = objFile[i].substr(0, 1)[0];
        switch (record) {
            case 'H':
                headerRecord();
                break;
            case 'T':
                textRecord(i);
                break;
            case 'E':
                endRecord(i);
                break;
        }
    }
}
    
