/*
    Steven Kha : 822700600
*/
#include "disassemFunctions.h"

using namespace std;

Dis disassembler;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Required: ./" << argv[0] << " " << "<filename>.obj <filename>.sym" << endl;
        exit(EXIT_FAILURE);
    }
    
    disassembler.parseFiles(argv[1], argv[2]);
    disassembler.disassembly();
}
