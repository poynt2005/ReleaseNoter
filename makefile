cc = gcc
cxx = g++
liber = ar
cflags = -I./libgit2/include -I./csdll/ManagedCallBridge/ManagedCallBridge -I./libfmt/include
ldflags = -L./libgit2/lib -L./csdll/ManagedCallBridge/x64/Debug
libs = -lManagedCallBridge -lgit2




utility:
	$(cxx) $(cflags) -std=c++17 -c Utilities.cc -o Utilities.o
	$(liber) -rcs Utilities.a Utilities.o 

noter:
	$(cxx) -fPIC -shared -DBUILDNOTERAPI $(cflags) -c ReleaseNoter.cc -o ReleaseNoter.o
	$(cxx) -shared $(ldflags) ReleaseNoter.o libfmt/lib/libfmt.a Utilities.a -Wl,--out-implib,ReleaseNoter.lib -o ReleaseNoter.dll $(libs)
	-del ReleaseNoter.o

testgit:
	$(cxx) $(cflags) -c testgit.cc -o testgit.o
	$(cxx) $(ldflags) -L./ testgit.o -o testgit.exe $(libs) -lReleaseNoter
	-del testgit.o