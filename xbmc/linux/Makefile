INCLUDES=-I. -I/usr/include/SDL -I.. -I../../guilib -I../utils -I../FileSystem 
#CFLAGS+=-DHAS_SDL

SRCS=ConvUtils.cpp XEventUtils.cpp XFileUtils.cpp XHandle.cpp XSyncUtils.cpp XTimeUtils.cpp XMemUtils.cpp XThreadUtils.cpp NetworkLinux.cpp NetworkLinux.cpp LinuxResourceCounter.cpp LinuxTimezone.cpp XRandR.cpp LinuxFileSystem.cpp XCriticalSection.cpp XLCDproc.cpp HalManager.cpp

LIB=linux.a

include ../../Makefile.include

-include Makefile.depend

x: NetworkLinux.o
	g++ -o x NetworkLinux.o
