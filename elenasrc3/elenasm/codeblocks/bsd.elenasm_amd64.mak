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
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -m64 -fvisibility=hidden
CFLAGS = -Wall -std=c++20 -m64 -fPIC

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../../temp/elenasm64
DEP_RELEASE = 
OUT_RELEASE = ../../../bin/libelenasm60_64.so

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/common/paths.o $(OBJDIR_RELEASE)/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/engine/scriptreader.o $(OBJDIR_RELEASE)/__/__/engine/syntaxtree.o $(OBJDIR_RELEASE)/__/cfparser.o $(OBJDIR_RELEASE)/__/scriptmachine.o $(OBJDIR_RELEASE)/__/transformer.o $(OBJDIR_RELEASE)/__/treeparser.o $(OBJDIR_RELEASE)/__/vmparser.o  $(OBJDIR_RELEASE)/__/linux/main.o $(OBJDIR_RELEASE)/__/regex.o $(OBJDIR_RELEASE)/__/scriptparser.o

all: release

clean: clean_release

before_release: 
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/linux || mkdir -p $(OBJDIR_RELEASE)/__/linux
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

$(OBJDIR_RELEASE)/__/__/engine/scriptreader.o: ../../engine/scriptreader.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/scriptreader.cpp -o $(OBJDIR_RELEASE)/__/__/engine/scriptreader.o

$(OBJDIR_RELEASE)/__/__/engine/syntaxtree.o: ../../engine/syntaxtree.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/syntaxtree.cpp -o $(OBJDIR_RELEASE)/__/__/engine/syntaxtree.o

$(OBJDIR_RELEASE)/__/cfparser.o: ../cfparser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../cfparser.cpp -o $(OBJDIR_RELEASE)/__/cfparser.o

$(OBJDIR_RELEASE)/__/scriptmachine.o: ../scriptmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../scriptmachine.cpp -o $(OBJDIR_RELEASE)/__/scriptmachine.o

$(OBJDIR_RELEASE)/__/transformer.o: ../transformer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../transformer.cpp -o $(OBJDIR_RELEASE)/__/transformer.o

$(OBJDIR_RELEASE)/__/treeparser.o: ../treeparser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../treeparser.cpp -o $(OBJDIR_RELEASE)/__/treeparser.o

$(OBJDIR_RELEASE)/__/vmparser.o: ../vmparser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../vmparser.cpp -o $(OBJDIR_RELEASE)/__/vmparser.o

$(OBJDIR_RELEASE)/__/regex.o: ../regex.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../regex.cpp -o $(OBJDIR_RELEASE)/__/regex.o

$(OBJDIR_RELEASE)/__/scriptparser.o: ../scriptparser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../scriptparser.cpp -o $(OBJDIR_RELEASE)/__/scriptparser.o

$(OBJDIR_RELEASE)/__/linux/main.o: ../linux/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/main.cpp -o $(OBJDIR_RELEASE)/__/linux/main.o

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/__
	rm -rf $(OBJDIR_RELEASE)/__/__/engine
	rm -rf $(OBJDIR_RELEASE)/__/linux
	rm -rf $(OBJDIR_RELEASE)/__/__/common

.PHONY: before_release after_release clean_release
