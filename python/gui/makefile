cc = gcc
cxx = g++
liber = ar
cflags = -I"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\include"
ldflags = -L"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\libs"
libs = -lpython310

test:
	$(cxx) -c test.cc -o test.o
	$(cxx) test.o -o test.exe

comp:
	$(cxx) -std=c++17 -c compileByteCode.cc -o compileByteCode.o
	$(cxx) compileByteCode.o -o compileByteCode.exe

main:
	$(cc) -DMS_WIN64 $(cflags) -c pyExec.c -o pyExec.o
	$(cc)  $(ldflags) pyExec.o -o  gui.exe $(libs)

util:
	$(cxx) -fPIC -shared -c utilities.cc -o utilities.o
	$(cxx) -fPIC -shared -DMS_WIN64 $(cflags) -c pyBinding.cc -o pyBinding.o
	$(cxx) -shared $(ldflags) utilities.o pyBinding.o -o PyUsefulModule.pyd $(libs)

pyd:
	python -m cython gui.py -o gui.c
	python -m cython main.py -o main.c
	$(cxx) -fPIC -shared -DMS_WIN64 $(cflags) -c gui.c -o gui.o
	$(cxx) -fPIC -shared -DMS_WIN64 $(cflags) -c main.c -o main.o
	$(cc) -shared $(ldflags) gui.o -o  gui.pyd $(libs)
	$(cc) -shared $(ldflags) main.o -o  main.pyd $(libs)

