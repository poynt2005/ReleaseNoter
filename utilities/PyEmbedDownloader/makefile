cc = gcc
cxx = g++
liber = ar
cflags =  -I"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\include"
ldflags =  -L"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\libs"
libs = -lpython310 -lUrlmon 

util:
	$(cxx) -fPIC -shared $(cflags) -c pyUtilities.cc -o pyUtilities.o
	$(cxx) $(ldflags) -shared pyUtilities.o -o PyBuildUtilities.pyd $(libs)

