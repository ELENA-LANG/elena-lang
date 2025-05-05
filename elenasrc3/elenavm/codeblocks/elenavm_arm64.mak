#------------------------------------------------------------------------------#
# ELENA make file                                                              #
#------------------------------------------------------------------------------#

WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I.. -I../../engine -I../../common
CFLAGS = -Wall -std=c++20 -fPIC -march=armv8-a
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -march=armv8-a -fvisibility=hidden

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../../temp/elenavm64
DEP_RELEASE = 
OUT_RELEASE = ../../../bin/libelenavm60_64.so

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/engine/gcroutines.o $(OBJDIR_RELEASE)/__/__/engine/linux/lnxroutines.o $(OBJDIR_RELEASE)/__/__/engine/linux/elfhelper.o $(OBJDIR_RELEASE)/__/elenavmmachine.o  $(OBJDIR_RELEASE)/__/__/engine/aarch64/aarch64routines.o $(OBJDIR_RELEASE)/__/linux/lnxsection.o $(OBJDIR_RELEASE)/__/linux/elenalnxvmachine.o $(OBJDIR_RELEASE)/__/__/engine/arm64compiler.o $(OBJDIR_RELEASE)/__/linux/main.o

all: release

clean: clean_release

before_release: 
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/__/engine/linux || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/linux
	test -d $(OBJDIR_RELEASE)/__/__/engine/aarch64 || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/aarch64
	test -d $(OBJDIR_RELEASE)/__/linux || mkdir -p $(OBJDIR_RELEASE)/__/linux
	test -d $(OBJDIR_RELEASE)/__/__/common || mkdir -p $(OBJDIR_RELEASE)/__/__/common

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/__/common/dump.o: ../../common/dump.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/dump.cpp -o $(OBJDIR_RELEASE)/__/__/common/dump.o

$(OBJDIR_RELEASE)/__/__/common/files.o: ../../common/files.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/files.cpp -o $(OBJDIR_RELEASE)/__/__/common/files.o 

$(OBJDIR_RELEASE)/__/__/common/ustring.o: ../../common/ustring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/ustring.cpp -o $(OBJDIR_RELEASE)/__/__/common/ustring.o 

$(OBJDIR_RELEASE)/__/__/engine/elenamachine.o: ../../engine/elenamachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/elenamachine.cpp -o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o

$(OBJDIR_RELEASE)/__/__/engine/bytecode.o: ../../engine/bytecode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/bytecode.cpp -o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o

$(OBJDIR_RELEASE)/__/__/engine/gcroutines.o: ../../engine/gcroutines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/gcroutines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/gcroutines.o

$(OBJDIR_RELEASE)/__/__/engine/linux/lnxroutines.o: ../../engine/linux/lnxroutines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux/lnxroutines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux/lnxroutines.o

$(OBJDIR_RELEASE)/__/__/engine/linux/elfhelper.o: ../../engine/linux/elfhelper.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux/elfhelper.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux/elfhelper.o

$(OBJDIR_RELEASE)/__/elenavmmachine.o: ../elenavmmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../elenavmmachine.cpp -o $(OBJDIR_RELEASE)/__/elenavmmachine.o

$(OBJDIR_RELEASE)/__/__/engine/aarch64/aarch64routines.o: ../../engine/aarch64/aarch64routines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/aarch64/aarch64routines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/aarch64/aarch64routines.o

$(OBJDIR_RELEASE)/__/linux/lnxsection.o: ../linux/lnxsection.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/lnxsection.cpp -o $(OBJDIR_RELEASE)/__/linux/lnxsection.o

$(OBJDIR_RELEASE)/__/linux/elenalnxvmachine.o: ../linux/elenalnxvmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/elenalnxvmachine.cpp -o $(OBJDIR_RELEASE)/__/linux/elenalnxvmachine.o

$(OBJDIR_RELEASE)/__/linux/main.o: ../linux/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/main.cpp -o $(OBJDIR_RELEASE)/__/linux/main.o

$(OBJDIR_RELEASE)/__/__/engine/arm64compiler.o: ../../engine/arm64compiler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/arm64compiler.cpp -o $(OBJDIR_RELEASE)/__/__/engine/arm64compiler.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/__
	rm -rf $(OBJDIR_RELEASE)/__/__/engine
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/linux
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/aarch64
	rm -rf $(OBJDIR_RELEASE)/__/linux
	rm -rf $(OBJDIR_RELEASE)/__/__/common

.PHONY: before_release after_release clean_release
