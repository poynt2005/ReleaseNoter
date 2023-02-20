cc = gcc
cxx = g++
liber = ar
cflags = -I"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\include"
ldflags = -L"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\libs"
libs = -lpython310

glober:
	$(cxx) -fPIC -shared -DMS_WIN64 $(cflags) -DBUILDPYGLOBER -c pyglober.cc -o pyglober.o
	$(cxx) -shared $(ldflags) pyglober.o -Wl,--out-implib,pyglober.lib -o pyglober.dll $(libs)

test:
	$(cxx) -c tester.cc -o tester.o
	$(cxx) -L./ tester.o -o tester.exe -lpyglober