#------------------------------------------------------------------------------#
# ELENA make file                                                              #
#------------------------------------------------------------------------------#

WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I.. -I../../../engine -I../../../common
RESINC = 
LIBDIR = 
LIB = 

ifeq ($(OS),Windows_NT)

CFLAGS = -Wall -std=c++20 -m64 -municode
LDFLAGS = -m64 -static-libgcc -static-libstdc++

else

CFLAGS = -Wall -std=c++20 -m64
LDFLAGS = -m64 -static-libgcc -static-libstdc++ -ldl

endif

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../../../temp/sg64-cli/
DEP_RELEASE = 
OUT_RELEASE = ../../../../bin/sg64-cli

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/__/engine/parsertable.o $(OBJDIR_RELEASE)/__/__/__/engine/scriptreader.o $(OBJDIR_RELEASE)/__/sg.o


all: release

clean: clean_release

before_release: 
	test -d ../../../../bin || mkdir -p ../../../../bin
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/__/__/common || mkdir -p $(OBJDIR_RELEASE)/__/__/__/common

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/__/__/common/dump.o: ../../../common/dump.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/dump.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/dump.o

$(OBJDIR_RELEASE)/__/__/__/common/files.o: ../../../common/files.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/files.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/files.o

$(OBJDIR_RELEASE)/__/__/__/common/ustring.o: ../../../common/ustring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/ustring.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o

$(OBJDIR_RELEASE)/__/__/__/engine/parsertable.o: ../../../engine/parsertable.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/parsertable.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/parsertable.o

$(OBJDIR_RELEASE)/__/__/__/engine/scriptreader.o: ../../../engine/scriptreader.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/scriptreader.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/scriptreader.o

$(OBJDIR_RELEASE)/__/sg.o  : ../sg.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../sg.cpp -o $(OBJDIR_RELEASE)/__/sg.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/__
	rm -rf $(OBJDIR_RELEASE)/__/__/__/engine
	rm -rf $(OBJDIR_RELEASE)/__/__/__/common

.PHONY: before_release after_release clean_release
