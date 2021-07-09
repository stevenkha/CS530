Steven Kha : 822700600

My diassembler uses aspects of object oriented programming as its core. 

objCodeFunctions:
The objCodeFunctions file utilizes an unordered map that pairs the opcode and their
corresponding mnemonic names. Most of the algorithm inside this file is coded to return the 
corresponding information based on the role of the functions. For example, the getAddressingMode 
functions reads in the first three hex numbers of the object code and returns the addressing mode of
the object code. Most functions in this file perform a similar task with reading in the first three 
hex numbers. The objCodeFunctions file is a framework built to be used by the disassemFunctions file.

disassemFunctions:
The disassemFunctions file is the main bulk of the diassembler algorithm. This file utilizes many unordered
maps, vectors, and uses streams to output to the output file. First, the file reads in the two command line
argument files which are expected to be the object file and symbol file. If either file cannot be found the file
outputs an error and reveals which file could not be found. It then stores the data from these two files into 
the corresponding vector or map which organizes the data efficiently and makes it easy to get the information 
when needed. The file also calls the functions in the objCodeFunctions file at the appropriate times. Afterwards, 
the disassembly function is expected to be ran which parses through the object file and stores the record such as 
'H', 'T', or 'E'. The function uses a for loop that will loop until it reaches the end of the object file. A switch 
case is then used to call the corresponding functions below that will analyze the data from these records. 

    headerRecord():
        Does all the functions and logic to output the header record correctly. It parses the header record line
        of the object file and outputs all the corresponding information about the header record.

    textRecord():
        The text record funtion is the most complex function as it is responsible for many logic instructions.
        It uses a for loop with a cursor to loop through the current line of the object file and read in the object codes.
        The cursor will traverse by a certain amount based on the format of the object code.
        This function will also check for any gaps in address and if insertion of symbols and literals are needed.
        The function will then call the corresponding analyzeFormat function based on the format of the object code.
        A checkBase function is called afterwards to check if a base value needs to be stored. The functions then
        updates the currentAddress accordingly and repeats until the end of the object file line.

    endRecord():
        This function essentially works the same as the headerRecord function as it is responsible for the logic 
        to output the end record.

The disassemFunctions file also utilizes many helper functions that help format the output of the information being 
outputted and helps make the code a bit more organized in terms of what each function is responsible for.

main:
This file is equivalent to a driver file. The main file checks for the correct amount of command line arguments and 
calls the parseFile function and disassembly function from the disassemFunctions file. If it reads that the correct ammount
of command line arguemnts is not correct it will output to the user the correct way of using the program.
