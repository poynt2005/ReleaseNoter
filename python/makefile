cc = gcc
cxx = g++
liber = ar
cflags = -I"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\include"
ldflags = -L../ -L"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\libs"
libs = -lpython310 -lReleaseNoter -lRpcrt4

noter:
	$(cxx) -fPIC -shared -DMS_WIN64 $(cflags) -c PyBinding.cc -o PyBinding.o
	$(cxx) -shared $(ldflags) PyBinding.o -o PyReleaseNoter.pyd $(libs)
	-del PyBinding.o

pyd:
	python -m cython ReleaseNoter.py -o ReleaseNoterTemp.c
	$(cc) -fPIC -shared -DMS_WIN64 $(cflags) -c ReleaseNoterTemp.c -o ReleaseNoterTemp.o
	$(cc) -shared $(ldflags) ReleaseNoterTemp.o -o ReleaseNoter.pyd $(libs)

	

comp:
	$(cxx) -std=c++17 -c compileByteCode.cc -o compileByteCode.o
	$(cxx) compileByteCode.o -o compileByteCode.exe
