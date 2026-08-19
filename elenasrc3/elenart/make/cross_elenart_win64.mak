#------------------------------------------------------------------------------#
# ELENA make file                                                              #
# cross-builds bin/elenart60_64.dll on Linux with mingw-w64                    #
#------------------------------------------------------------------------------#

WORKDIR = `pwd`

CC = x86_64-w64-mingw32-gcc
CXX = x86_64-w64-mingw32-g++
AR = x86_64-w64-mingw32-ar
LD = x86_64-w64-mingw32-g++

RESINC =
LIBDIR =
LIB =
LDFLAGS = -m64 -shared -static

INC = -I.. -I../../engine -I../../common -I../../lruntime
CFLAGS = -Wall -std=c++20 -m64 -municode

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../../temp/elenart64_win
DEP_RELEASE =

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/common/config.o $(OBJDIR_RELEASE)/__/__/common/xmltree.o $(OBJDIR_RELEASE)/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/common/paths.o $(OBJDIR_RELEASE)/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/engine/gcroutines.o $(OBJDIR_RELEASE)/__/__/engine/libman.o $(OBJDIR_RELEASE)/__/__/engine/section.o $(OBJDIR_RELEASE)/__/__/engine/module.o $(OBJDIR_RELEASE)/__/__/engine/rtmanager.o $(OBJDIR_RELEASE)/__/__/engine/windows/winroutines.o $(OBJDIR_RELEASE)/__/__/engine/windows/pehelper.o $(OBJDIR_RELEASE)/__/elenartmachine.o $(OBJDIR_RELEASE)/__/__/engine/amd64/amd64routines.o $(OBJDIR_RELEASE)/__/windows/dllmain.o
OUT_RELEASE = ../../../bin/elenart60_64.dll

all: release

clean: clean_release

before_release:
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/windows || mkdir -p $(OBJDIR_RELEASE)/__/windows
	test -d $(OBJDIR_RELEASE)/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/__/engine/amd64 || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/amd64
	test -d $(OBJDIR_RELEASE)/__/__/engine/windows || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/windows
	test -d $(OBJDIR_RELEASE)/__/__/common || mkdir -p $(OBJDIR_RELEASE)/__/__/common

after_release:

release: before_release out_release after_release

$(OBJDIR_RELEASE)/__/__/common/config.o: ../../common/config.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/config.cpp -o $(OBJDIR_RELEASE)/__/__/common/config.o

$(OBJDIR_RELEASE)/__/__/common/dump.o: ../../common/dump.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/dump.cpp -o $(OBJDIR_RELEASE)/__/__/common/dump.o

$(OBJDIR_RELEASE)/__/__/common/files.o: ../../common/files.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/files.cpp -o $(OBJDIR_RELEASE)/__/__/common/files.o

$(OBJDIR_RELEASE)/__/__/common/paths.o: ../../common/paths.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/paths.cpp -o $(OBJDIR_RELEASE)/__/__/common/paths.o

$(OBJDIR_RELEASE)/__/__/common/ustring.o: ../../common/ustring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/ustring.cpp -o $(OBJDIR_RELEASE)/__/__/common/ustring.o

$(OBJDIR_RELEASE)/__/__/common/xmltree.o: ../../common/xmltree.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/xmltree.cpp -o $(OBJDIR_RELEASE)/__/__/common/xmltree.o

$(OBJDIR_RELEASE)/__/__/engine/elenamachine.o: ../../engine/elenamachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/elenamachine.cpp -o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o

$(OBJDIR_RELEASE)/__/__/engine/rtmanager.o: ../../engine/rtmanager.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/rtmanager.cpp -o $(OBJDIR_RELEASE)/__/__/engine/rtmanager.o

$(OBJDIR_RELEASE)/__/__/engine/gcroutines.o: ../../engine/gcroutines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/gcroutines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/gcroutines.o

$(OBJDIR_RELEASE)/__/__/engine/bytecode.o: ../../engine/bytecode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/bytecode.cpp -o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o

$(OBJDIR_RELEASE)/__/__/engine/libman.o: ../../engine/libman.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/libman.cpp -o $(OBJDIR_RELEASE)/__/__/engine/libman.o

$(OBJDIR_RELEASE)/__/__/engine/module.o: ../../engine/module.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/module.cpp -o $(OBJDIR_RELEASE)/__/__/engine/module.o

$(OBJDIR_RELEASE)/__/__/engine/section.o: ../../engine/section.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/section.cpp -o $(OBJDIR_RELEASE)/__/__/engine/section.o

$(OBJDIR_RELEASE)/__/elenartmachine.o: ../elenartmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../elenartmachine.cpp -o $(OBJDIR_RELEASE)/__/elenartmachine.o

$(OBJDIR_RELEASE)/__/__/engine/amd64/amd64routines.o: ../../engine/amd64/amd64routines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/amd64/amd64routines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/amd64/amd64routines.o

$(OBJDIR_RELEASE)/__/__/engine/windows/winroutines.o: ../../engine/windows/winroutines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/windows/winroutines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/windows/winroutines.o

$(OBJDIR_RELEASE)/__/__/engine/windows/pehelper.o: ../../lruntime/windows/pehelper.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../lruntime/windows/pehelper.cpp -o $(OBJDIR_RELEASE)/__/__/engine/windows/pehelper.o

$(OBJDIR_RELEASE)/__/windows/dllmain.o: ../windows/dllmain.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../windows/dllmain.cpp -o $(OBJDIR_RELEASE)/__/windows/dllmain.o

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

clean_release:
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_release after_release clean_release
