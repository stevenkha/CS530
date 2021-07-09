/*
    Steven Kha : 822700600
*/
#include "objCodeFunctions.h"

unordered_map<int, string> umap;

const static int ops[] = {
    0x18, 0x58, 0x90, 0x40, 0xB4, 0x28, 0x88, 0xA0, 0x24, 0x64, 0x9C, 0xC4,
    0xC0, 0xF4, 0x3C, 0x30, 0x34, 0x38, 0x48, 0x00, 0x68, 0x50, 0x70, 0x08,
    0x6C, 0x74, 0x04, 0xD0, 0x20, 0x60, 0x98, 0xC8, 0x44, 0xD8, 0xAC, 0x4C,
    0xA4, 0xA8, 0xF0, 0xEC, 0x0C, 0x78, 0x54, 0x80, 0xD4, 0x14, 0x7C, 0xE8,
    0x84, 0x10, 0x1C, 0x5C, 0x94, 0xB0, 0xE0, 0xF8, 0x2C, 0xB8, 0xDC
};

const static string mnemonics[] = {
    "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP","COMPF", "COMPR", "DIV", "DIVF", "DIVR", 
    "FIX","FLOAT", "HIO", "J", "JEQ", "JGT", "JLT","JSUB", "LDA", "LDB", "LDCH", "LDF", 
    "LDL","LDS", "LDT", "LDX", "LPS", "MUL", "MULF","MULR", "NORM", "OR", "RD", "RMO", 
    "RSUB","SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB","STCH", "STF", "STI", "STL","STS", 
    "STSW","STT", "STX", "SUB", "SUBF", "SUBR", "SVC","TD", "TIO", "TIX", "TIXR", "WD"
};

//functions creates the key and value pairs for the opcodes and mnemonics
void objCode::setMap() {
    for (int i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
        umap.insert(make_pair(ops[i], mnemonics[i]));
    }
}

//functions checks if bit at a certain position is set
bool objCode::checkBitSet(int num, int pos) {
    return ((num >> pos) & 1);
}

//turn the first three hex numbers of the object code into integers
int objCode::firstThreeNums(string objectCode) {
    string firstThreeNums = objectCode.substr(0, 3);
    /*
        Turn into hex and then get integer value hex number
    */
    firstThreeNums.insert(0, "0x");
    int asInt = stoi(firstThreeNums, nullptr, 16);
    return asInt;
}

//find the appropriate addressing mode of the Object Code and perform a calculation to find the corresponding op code
int objCode::getAddressingMode(string objectCode) {
    /*
        1 : Simple
        2 : Indirect
        3 : Immediate
    */
    int firstThreeAsInt = objCode::firstThreeNums(objectCode);
    if (objCode::checkBitSet(firstThreeAsInt, 5) == 1 && objCode::checkBitSet(firstThreeAsInt, 4) == 1) {
        return 1;
    } else if (objCode::checkBitSet(firstThreeAsInt, 5) == 1 && objCode::checkBitSet(firstThreeAsInt, 4) == 0) {
        return 2;
    } else {
        return 3;
    }

    return 0;
}

//return corresponding addressing type based on the first 3 numbers of the object code and checking if nixbpe bits are set
int objCode::getAddressingType(string objectCode) {
    /*
        1 : Direct
        10: Direct, Indexed
        2 : PC-Relative
        20: PC-Relative, Indexed
        3 : Base-Relative
        30: Base-Relative, Indexed
    */
    int firstThreeAsInt = objCode::firstThreeNums(objectCode);
    if (objCode::checkBitSet(firstThreeAsInt, 0)) {
        if (objCode::checkBitSet(firstThreeAsInt, 3) == 1) {
            return 10;
        } else {
            return 1;
        }
    } else if (objCode::checkBitSet(firstThreeAsInt, 2) == 0 && objCode::checkBitSet(firstThreeAsInt, 1) == 1) {
        if (objCode::checkBitSet(firstThreeAsInt, 3) == 1) {
            return 20;
        } else {
            return 2;
        }
    } else {
        if (objCode::checkBitSet(firstThreeAsInt, 3) == 1) {
            return 30;
        } else {
            return 3;
        }
    }
    return 0;
}

//find the format of the object code by checking bit position
int objCode::getFormat(string objectCode) {
    int firstThreeAsInt = objCode::firstThreeNums(objectCode);
    string mnemonicPart = objectCode.substr(0, 2);
    mnemonicPart.insert(0, "0x");
    int opsAsInt = stoi(mnemonicPart, nullptr, 16);
    if (umap.find(opsAsInt) != umap.end()) {
        return 2;
    } else if ((firstThreeAsInt >> 0) & 1 == 1) {
        return 4;
    } else {
        return 3;
    }

    return 0;
}

//function returns an int array with the two register values
int* objCode::getRegister(string objectCode) {
    char* t = new char[2];
    int* regs = new int[2];
    t[0] = objectCode[2];
    t[1] = objectCode[3];

    regs[0] = t[0] - '0';
    regs[1] = t[1] - '0';
    return regs;
}

//find the correct op code by using the addressing mode calculation
string objCode::getOpcode(string objectCode) {
    string mnemonicPart = objectCode.substr(0, 2);
    //turn opcode into hex
    mnemonicPart.insert(0, "0x");
    //convert hex opcode into integer
    int opsAsInt = stoi(mnemonicPart, nullptr, 16);
    //clear the last two bits of the opcode
    opsAsInt &= ~(1UL << 0);
    opsAsInt &= ~(1UL << 1);
    stringstream ss;
    ss << hex << opsAsInt;
    return ss.str();
}

//find the corresponding mnemonic 
string objCode::getMnemonic(string objectCode) {
    string opCode = objCode::getOpcode(objectCode);
    string mnemonic;
    //turn opcode into hex
    opCode.insert(0, "0x");
    //convert hex opcode into integer
    int intOpCode = stoi(opCode, nullptr, 16);
    if (umap.find(intOpCode) == umap.end()) {
        return "Not found";
    } else {
        return umap.find(intOpCode) -> second;
    }  

    return "";
}
