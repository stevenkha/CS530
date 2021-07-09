# Steven Kha : 822700600

CXX = g++
CFLAGS = -std=c++11 -g

TARGET = dissem

$(TARGET): main.o objCodeFunctions.o disassemFunctions.o
	$(CXX) $(CFLAGS) -o $(TARGET) $^

main.o :  main.cpp disassemFunctions.h
	$(CXX) $(CFLAGS) -c main.cpp

objCodeFunctions.o :  objCodeFunctions.cpp objCodeFunctions.h
	$(CXX) $(CFLAGS) -c objCodeFunctions.cpp

disassemFunctions.o : disassemFunctions.cpp disassemFunctions.h
	$(CXX) $(CFLAGS) -c disassemFunctions.cpp
 
clean:
	rm *.o
