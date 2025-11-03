#------------------------------------------------------------------------------#
# ELENA make file                                                              #
#------------------------------------------------------------------------------#

WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -m64 -fvisibility=hidden

ifeq ($(OS),Windows_NT)

INC = -I.. -I../../engine -I../../common -I../../lruntime
CFLAGS = -Wall -std=c++20 -m64 -fPIC -municode

else

INC = -I.. -I../../engine -I../../common
CFLAGS = -Wall -std=c++20 -m64 -fPIC

endif

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../../temp/elenavm64
DEP_RELEASE = 

ifeq ($(OS),Windows_NT)

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/common/paths.o $(OBJDIR_RELEASE)/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/engine/gcroutines.o $(OBJDIR_RELEASE)/__/__/engine/libman.o $(OBJDIR_RELEASE)/__/__/engine/section.o $(OBJDIR_RELEASE)/__/__/engine/module.o $(OBJDIR_RELEASE)/__/__/engine/rtmanager.o $(OBJDIR_RELEASE)/__/__/engine/windows/winroutines.o $(OBJDIR_RELEASE)/__/__/engine/windows/pehelper.o $(OBJDIR_RELEASE)/__/elenavmmachine.o $(OBJDIR_RELEASE)/__/__/engine/amd64/amd64routines.o $(OBJDIR_RELEASE)/__/windows/winsection.o $(OBJDIR_RELEASE)/__/windows/elenawinvmachine.o $(OBJDIR_RELEASE)/__/windows/dllmain.o $(OBJDIR_RELEASE)/__/__/engine/x86_64compiler.o $(OBJDIR_RELEASE)/__/__/engine/xmlprojectbase.o $(OBJDIR_RELEASE)/__/__/engine/jitcompiler.o $(OBJDIR_RELEASE)/__/__/engine/windows/presenter.o  $(OBJDIR_RELEASE)/__/__/engine/codescope.o $(OBJDIR_RELEASE)/__/__/engine/x86helper.o $(OBJDIR_RELEASE)/__/__/common/xmltree.o $(OBJDIR_RELEASE)/__/__/common/config.o $(OBJDIR_RELEASE)/__/__/engine/jitlinker.o
OUT_RELEASE = ../../../bin/elenavm60_64.dll

else

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/common/paths.o $(OBJDIR_RELEASE)/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/engine/gcroutines.o $(OBJDIR_RELEASE)/__/__/engine/libman.o $(OBJDIR_RELEASE)/__/__/engine/section.o $(OBJDIR_RELEASE)/__/__/engine/module.o $(OBJDIR_RELEASE)/__/__/engine/rtmanager.o $(OBJDIR_RELEASE)/__/__/engine/linux/lnxroutines.o $(OBJDIR_RELEASE)/__/__/engine/linux/elfhelper.o $(OBJDIR_RELEASE)/__/elenavmmachine.o $(OBJDIR_RELEASE)/__/__/engine/amd64/amd64routines.o $(OBJDIR_RELEASE)/__/linux/lnxsection.o $(OBJDIR_RELEASE)/__/linux/elenalnxvmachine.o $(OBJDIR_RELEASE)/__/linux/main.o $(OBJDIR_RELEASE)/__/__/engine/x86_64compiler.o $(OBJDIR_RELEASE)/__/__/engine/xmlprojectbase.o $(OBJDIR_RELEASE)/__/__/engine/jitcompiler.o $(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o $(OBJDIR_RELEASE)/__/__/engine/codescope.o $(OBJDIR_RELEASE)/__/__/engine/x86helper.o $(OBJDIR_RELEASE)/__/__/common/xmltree.o $(OBJDIR_RELEASE)/__/__/common/config.o $(OBJDIR_RELEASE)/__/__/engine/jitlinker.o
OUT_RELEASE = ../../../bin/libelenavm60_64.so

endif

all: release

clean: clean_release

before_release: 
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/__/engine/amd64 || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/amd64
ifeq ($(OS),Windows_NT)
	test -d $(OBJDIR_RELEASE)/__/windows || mkdir -p $(OBJDIR_RELEASE)/__/windows
	test -d $(OBJDIR_RELEASE)/__/__/engine/windows || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/windows
else
	test -d $(OBJDIR_RELEASE)/__/linux || mkdir -p $(OBJDIR_RELEASE)/__/linux
	test -d $(OBJDIR_RELEASE)/__/__/engine/linux || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/linux
endif
	test -d $(OBJDIR_RELEASE)/__/__/common || mkdir -p $(OBJDIR_RELEASE)/__/__/common

after_release: 

release: before_release out_release after_release

$(OBJDIR_RELEASE)/__/__/common/dump.o: ../../common/dump.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/dump.cpp -o $(OBJDIR_RELEASE)/__/__/common/dump.o

$(OBJDIR_RELEASE)/__/__/common/files.o: ../../common/files.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/files.cpp -o $(OBJDIR_RELEASE)/__/__/common/files.o 

$(OBJDIR_RELEASE)/__/__/common/paths.o: ../../common/paths.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/paths.cpp -o $(OBJDIR_RELEASE)/__/__/common/paths.o 

$(OBJDIR_RELEASE)/__/__/common/ustring.o: ../../common/ustring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/ustring.cpp -o $(OBJDIR_RELEASE)/__/__/common/ustring.o 

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

$(OBJDIR_RELEASE)/__/elenavmmachine.o: ../elenavmmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../elenavmmachine.cpp -o $(OBJDIR_RELEASE)/__/elenavmmachine.o

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/__/engine/amd64/amd64routines.o: ../../engine/amd64/amd64routines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/amd64/amd64routines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/amd64/amd64routines.o

$(OBJDIR_RELEASE)/__/__/engine/x86/x86routines.o: ../../engine/x86/x86routines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/x86/x86routines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/x86/x86routines.o

$(OBJDIR_RELEASE)/__/__/engine/x86_64compiler.o: ../../engine/x86_64compiler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/x86_64compiler.cpp -o $(OBJDIR_RELEASE)/__/__/engine/x86_64compiler.o

$(OBJDIR_RELEASE)/__/__/engine/xmlprojectbase.o: ../../engine/xmlprojectbase.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/xmlprojectbase.cpp -o $(OBJDIR_RELEASE)/__/__/engine/xmlprojectbase.o

ifeq ($(OS),Windows_NT)

$(OBJDIR_RELEASE)/__/__/engine/windows/winroutines.o: ../../engine/windows/winroutines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/windows/winroutines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/windows/winroutines.o

$(OBJDIR_RELEASE)/__/__/engine/windows/pehelper.o: ../../lruntime/windows/pehelper.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../lruntime/windows/pehelper.cpp -o $(OBJDIR_RELEASE)/__/__/engine/windows/pehelper.o

$(OBJDIR_RELEASE)/__/windows/winsection.o: ../windows/winsection.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../windows/winsection.cpp -o $(OBJDIR_RELEASE)/__/windows/winsection.o

$(OBJDIR_RELEASE)/__/windows/elenawinvmachine.o: ../windows/elenawinvmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../windows/elenawinvmachine.cpp -o $(OBJDIR_RELEASE)/__/windows/elenawinvmachine.o

$(OBJDIR_RELEASE)/__/windows/dllmain.o: ../windows/dllmain.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../windows/dllmain.cpp -o $(OBJDIR_RELEASE)/__/windows/dllmain.o

$(OBJDIR_RELEASE)/__/__/engine/windows/presenter.o: ../../engine/windows/presenter.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/windows/presenter.cpp -o $(OBJDIR_RELEASE)/__/__/engine/windows/presenter.o

else

$(OBJDIR_RELEASE)/__/__/engine/linux/lnxroutines.o: ../../engine/linux/lnxroutines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux/lnxroutines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux/lnxroutines.o

$(OBJDIR_RELEASE)/__/__/engine/linux/elfhelper.o: ../../engine/linux/elfhelper.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux/elfhelper.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux/elfhelper.o

$(OBJDIR_RELEASE)/__/linux/lnxsection.o: ../linux/lnxsection.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/lnxsection.cpp -o $(OBJDIR_RELEASE)/__/linux/lnxsection.o

$(OBJDIR_RELEASE)/__/linux/elenalnxvmachine.o: ../linux/elenalnxvmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/elenalnxvmachine.cpp -o $(OBJDIR_RELEASE)/__/linux/elenalnxvmachine.o

$(OBJDIR_RELEASE)/__/linux/main.o: ../linux/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/main.cpp -o $(OBJDIR_RELEASE)/__/linux/main.o

$(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o: ../../engine/linux/presenter.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux/presenter.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o

endif

$(OBJDIR_RELEASE)/__/__/engine/jitcompiler.o: ../../engine/jitcompiler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/jitcompiler.cpp -o $(OBJDIR_RELEASE)/__/__/engine/jitcompiler.o   

$(OBJDIR_RELEASE)/__/__/engine/codescope.o: ../../engine/codescope.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/codescope.cpp -o $(OBJDIR_RELEASE)/__/__/engine/codescope.o   

$(OBJDIR_RELEASE)/__/__/engine/x86helper.o: ../../engine/x86helper.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/x86helper.cpp -o $(OBJDIR_RELEASE)/__/__/engine/x86helper.o

$(OBJDIR_RELEASE)/__/__/common/xmltree.o: ../../common/xmltree.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/xmltree.cpp -o $(OBJDIR_RELEASE)/__/__/common/xmltree.o  

$(OBJDIR_RELEASE)/__/__/common/config.o: ../../common/config.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/config.cpp -o $(OBJDIR_RELEASE)/__/__/common/config.o

$(OBJDIR_RELEASE)/__/__/engine/jitlinker.o: ../../engine/jitlinker.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/jitlinker.cpp -o $(OBJDIR_RELEASE)/__/__/engine/jitlinker.o   

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/__
	rm -rf $(OBJDIR_RELEASE)/__/__/engine
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/amd64
ifeq ($(OS),Windows_NT)
	rm -rf $(OBJDIR_RELEASE)/__/windows
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/windows
else
	rm -rf $(OBJDIR_RELEASE)/__/linux
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/linux
endif
	rm -rf $(OBJDIR_RELEASE)/__/__/common

.PHONY: before_release after_release clean_release
