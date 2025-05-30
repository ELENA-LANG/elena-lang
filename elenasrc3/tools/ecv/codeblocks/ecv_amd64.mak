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
OBJDIR_RELEASE = ../../../temp/ecv64-cli/
DEP_RELEASE = 
OUT_RELEASE = ../../../../bin/ecv64-cli

ifeq ($(OS),Windows_NT)

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/__/common/dump.o  $(OBJDIR_RELEASE)/__/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/__/common/paths.o  $(OBJDIR_RELEASE)/__/__/__/common/xmltree.o  $(OBJDIR_RELEASE)/__/__/__/common/config.o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/__/engine/module.o $(OBJDIR_RELEASE)/__/__/__/engine/section.o $(OBJDIR_RELEASE)/__/__/__/engine/libman.o $(OBJDIR_RELEASE)/__/ecviewer.o $(OBJDIR_RELEASE)/__/windows/ecv.o

else

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/__/common/dump.o  $(OBJDIR_RELEASE)/__/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/__/common/paths.o  $(OBJDIR_RELEASE)/__/__/__/common/xmltree.o  $(OBJDIR_RELEASE)/__/__/__/common/config.o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/__/engine/module.o $(OBJDIR_RELEASE)/__/__/__/engine/section.o $(OBJDIR_RELEASE)/__/__/__/engine/libman.o $(OBJDIR_RELEASE)/__/ecviewer.o $(OBJDIR_RELEASE)/__/linux/ecv.o

endif

all: release

clean: clean_release

before_release: 
	test -d ../../../../bin || mkdir -p ../../../../bin
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/__/__/common || mkdir -p $(OBJDIR_RELEASE)/__/__/__/common
ifeq ($(OS),Windows_NT)
	test -d $(OBJDIR_RELEASE)/__/windows || mkdir -p $(OBJDIR_RELEASE)/__/windows
else
	test -d $(OBJDIR_RELEASE)/__/linux || mkdir -p $(OBJDIR_RELEASE)/__/linux
endif

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/__/__/common/dump.o: ../../../common/dump.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/dump.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/dump.o

$(OBJDIR_RELEASE)/__/__/__/common/files.o: ../../../common/files.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/files.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/files.o

$(OBJDIR_RELEASE)/__/__/__/common/paths.o: ../../../common/paths.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/paths.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/paths.o

$(OBJDIR_RELEASE)/__/__/__/common/xmltree.o: ../../../common/xmltree.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/xmltree.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/xmltree.o

$(OBJDIR_RELEASE)/__/__/__/common/config.o: ../../../common/config.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/config.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/config.o

$(OBJDIR_RELEASE)/__/__/__/common/ustring.o: ../../../common/ustring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/ustring.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o

$(OBJDIR_RELEASE)/__/__/__/engine/bytecode.o: ../../../engine/bytecode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/bytecode.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/bytecode.o

$(OBJDIR_RELEASE)/__/__/__/engine/module.o: ../../../engine/module.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/module.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/module.o

$(OBJDIR_RELEASE)/__/__/__/engine/section.o: ../../../engine/section.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/section.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/section.o

$(OBJDIR_RELEASE)/__/__/__/engine/libman.o: ../../../engine/libman.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/libman.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/libman.o

$(OBJDIR_RELEASE)/__/ecviewer.o  : ../ecviewer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../ecviewer.cpp -o $(OBJDIR_RELEASE)/__/ecviewer.o

ifeq ($(OS),Windows_NT)

$(OBJDIR_RELEASE)/__/windows/ecv.o  : ../windows/ecv.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../windows/ecv.cpp -o $(OBJDIR_RELEASE)/__/windows/ecv.o

else

$(OBJDIR_RELEASE)/__/linux/ecv.o  : ../linux/ecv.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/ecv.cpp -o $(OBJDIR_RELEASE)/__/linux/ecv.o

endif

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/__/__/__/engine
	rm -rf $(OBJDIR_RELEASE)/__/__/__/common
	rm -rf $(OBJDIR_RELEASE)/__/linux

.PHONY: before_release after_release clean_release
