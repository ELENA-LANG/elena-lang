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
CFLAGS = -march=pentium3 -Wall -std=c++20 -m32
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -m32 -static-libgcc -static-libstdc++ -ldl -llibelenavm60 -llibelenasm60 -L../../../../bin

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../../../temp/elt-cli/
DEP_RELEASE = 
OUT_RELEASE = ../../../../bin/elt-cli

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/__/common/paths.o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/vmsession.o $(OBJDIR_RELEASE)/__/linux/elt.o $(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o

all: release

clean: clean_release

before_release: 
	test -d ../../../../bin || mkdir -p ../../../../bin
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/__/__/common || mkdir -p $(OBJDIR_RELEASE)/__/__/__/common
	test -d $(OBJDIR_RELEASE)/__/linux || mkdir -p $(OBJDIR_RELEASE)/__/linux

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/__/__/common/files.o: ../../../common/files.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/files.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/files.o

$(OBJDIR_RELEASE)/__/__/__/common/paths.o: ../../../common/paths.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/paths.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/paths.o

$(OBJDIR_RELEASE)/__/__/__/common/ustring.o: ../../../common/ustring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/ustring.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o

$(OBJDIR_RELEASE)/__/vmsession.o  : ../vmsession.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../vmsession.cpp -o $(OBJDIR_RELEASE)/__/vmsession.o

$(OBJDIR_RELEASE)/__/linux/elt.o  : ../linux/elt.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux/elt.cpp -o $(OBJDIR_RELEASE)/__/linux/elt.o

$(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o: ../../engine/linux/presenter.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux/presenter.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/__
	rm -rf $(OBJDIR_RELEASE)/__/__/__/engine
	rm -rf $(OBJDIR_RELEASE)/__/__/__/common
	rm -rf $(OBJDIR_RELEASE)/__/linux

.PHONY: before_release after_release clean_release
