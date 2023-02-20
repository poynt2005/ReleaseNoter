cc = gcc
cxx = g++
cflags = -I"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\include"
ldflags = -L"C:\Users\SteveChung\AppData\Local\Programs\Python\Python310\libs"
libs = -lwbemuuid -lOle32 -lOleAut32

finder:
	$(cxx) -fPIC -shared -DBUILDVSFINDERAPI -c  WMI.cc -o WMI.o
	$(cxx) -shared -Wl,--out-implib,VSFinder.lib WMI.o  -o VSFinder.dll $(libs) 

pyd:
	$(cxx) -fPIC -shared $(cflags) -DMS_WIN64 -c  pyBinding.cc -o pyBinding.o
	$(cxx) -shared $(ldflags) -L./ pyBinding.o -o PyVSFinder.pyd -lpython310 -lVSFinder

test:
	$(cxx) -c  test.cc -o test.o
	$(cxx) -L./ test.o -o test.exe -lVSFinder