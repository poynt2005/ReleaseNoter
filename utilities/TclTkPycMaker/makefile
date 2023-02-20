cc = gcc
cxx = g++
liber = ar
cflags = -I./c++cli/TclTkPycMaker
ldflags = -L./c++cli/x64/Debug
libs = -lTclTkPycMaker

test:
	$(cxx) $(cflags) -c test.cc -o test.o
	$(cxx) $(ldflags) test.o -o test.exe $(libs)