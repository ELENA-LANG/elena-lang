#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I../../common -I../../engine -I../linux32 -I..
CFLAGS = -march=pentium2 -Wall -m32 -fexceptions -fvisibility=hidden -fvisibility-inlines-hidden -Wno-switch -D_LINUX
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -m32 -fvisibility=hidden

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = ../temp
DEP_DEBUG = 
OUT_DEBUG = ../../../bin/libelenart.so

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../temp
DEP_RELEASE = 
OUT_RELEASE = ../../../bin/libelenart.so

OBJ_DEBUG = $(OBJDIR_DEBUG)/__/linux32/main.o $(OBJDIR_DEBUG)/__/elenartmachine.o $(OBJDIR_DEBUG)/__/__/engine/x86/x86routines.o $(OBJDIR_DEBUG)/__/__/engine/section.o $(OBJDIR_DEBUG)/__/__/engine/rtman.o $(OBJDIR_DEBUG)/__/__/engine/module.o $(OBJDIR_DEBUG)/__/__/engine/linux32/linx32routines.o $(OBJDIR_DEBUG)/__/__/engine/linux32/elfhelper.o $(OBJDIR_DEBUG)/__/__/common/altstrings.o $(OBJDIR_DEBUG)/__/__/engine/libman.o $(OBJDIR_DEBUG)/__/__/engine/elenamachine.o $(OBJDIR_DEBUG)/__/__/engine/bytecode.o $(OBJDIR_DEBUG)/__/__/common/xmlreader.o $(OBJDIR_DEBUG)/__/__/common/files.o $(OBJDIR_DEBUG)/__/__/common/dump.o $(OBJDIR_DEBUG)/__/__/common/config.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/linux32/main.o $(OBJDIR_RELEASE)/__/elenartmachine.o $(OBJDIR_RELEASE)/__/__/engine/x86/x86routines.o $(OBJDIR_RELEASE)/__/__/engine/section.o $(OBJDIR_RELEASE)/__/__/engine/rtman.o $(OBJDIR_RELEASE)/__/__/engine/module.o $(OBJDIR_RELEASE)/__/__/engine/linux32/linx32routines.o $(OBJDIR_RELEASE)/__/__/engine/linux32/elfhelper.o $(OBJDIR_RELEASE)/__/__/common/altstrings.o $(OBJDIR_RELEASE)/__/__/engine/libman.o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/common/xmlreader.o $(OBJDIR_RELEASE)/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/common/config.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_DEBUG)/__/linux32 || mkdir -p $(OBJDIR_DEBUG)/__/linux32
	test -d $(OBJDIR_DEBUG)/__ || mkdir -p $(OBJDIR_DEBUG)/__
	test -d $(OBJDIR_DEBUG)/__/__/engine/x86 || mkdir -p $(OBJDIR_DEBUG)/__/__/engine/x86
	test -d $(OBJDIR_DEBUG)/__/__/engine || mkdir -p $(OBJDIR_DEBUG)/__/__/engine
	test -d $(OBJDIR_DEBUG)/__/__/engine/linux32 || mkdir -p $(OBJDIR_DEBUG)/__/__/engine/linux32
	test -d $(OBJDIR_DEBUG)/__/__/common || mkdir -p $(OBJDIR_DEBUG)/__/__/common

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) -shared $(LIBDIR_DEBUG) $(OBJ_DEBUG)  -o $(OUT_DEBUG) $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/__/linux32/main.o: ../linux32/main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../linux32/main.cpp -o $(OBJDIR_DEBUG)/__/linux32/main.o

$(OBJDIR_DEBUG)/__/elenartmachine.o: ../elenartmachine.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../elenartmachine.cpp -o $(OBJDIR_DEBUG)/__/elenartmachine.o

$(OBJDIR_DEBUG)/__/__/engine/x86/x86routines.o: ../../engine/x86/x86routines.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/x86/x86routines.cpp -o $(OBJDIR_DEBUG)/__/__/engine/x86/x86routines.o

$(OBJDIR_DEBUG)/__/__/engine/section.o: ../../engine/section.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/section.cpp -o $(OBJDIR_DEBUG)/__/__/engine/section.o

$(OBJDIR_DEBUG)/__/__/engine/rtman.o: ../../engine/rtman.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/rtman.cpp -o $(OBJDIR_DEBUG)/__/__/engine/rtman.o

$(OBJDIR_DEBUG)/__/__/engine/module.o: ../../engine/module.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/module.cpp -o $(OBJDIR_DEBUG)/__/__/engine/module.o

$(OBJDIR_DEBUG)/__/__/engine/linux32/linx32routines.o: ../../engine/linux32/linx32routines.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/linux32/linx32routines.cpp -o $(OBJDIR_DEBUG)/__/__/engine/linux32/linx32routines.o

$(OBJDIR_DEBUG)/__/__/engine/linux32/elfhelper.o: ../../engine/linux32/elfhelper.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/linux32/elfhelper.cpp -o $(OBJDIR_DEBUG)/__/__/engine/linux32/elfhelper.o

$(OBJDIR_DEBUG)/__/__/common/altstrings.o: ../../common/altstrings.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../common/altstrings.cpp -o $(OBJDIR_DEBUG)/__/__/common/altstrings.o

$(OBJDIR_DEBUG)/__/__/engine/libman.o: ../../engine/libman.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/libman.cpp -o $(OBJDIR_DEBUG)/__/__/engine/libman.o

$(OBJDIR_DEBUG)/__/__/engine/elenamachine.o: ../../engine/elenamachine.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/elenamachine.cpp -o $(OBJDIR_DEBUG)/__/__/engine/elenamachine.o

$(OBJDIR_DEBUG)/__/__/engine/bytecode.o: ../../engine/bytecode.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../engine/bytecode.cpp -o $(OBJDIR_DEBUG)/__/__/engine/bytecode.o

$(OBJDIR_DEBUG)/__/__/common/xmlreader.o: ../../common/xmlreader.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../common/xmlreader.cpp -o $(OBJDIR_DEBUG)/__/__/common/xmlreader.o

$(OBJDIR_DEBUG)/__/__/common/files.o: ../../common/files.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../common/files.cpp -o $(OBJDIR_DEBUG)/__/__/common/files.o

$(OBJDIR_DEBUG)/__/__/common/dump.o: ../../common/dump.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../common/dump.cpp -o $(OBJDIR_DEBUG)/__/__/common/dump.o

$(OBJDIR_DEBUG)/__/__/common/config.o: ../../common/config.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../common/config.cpp -o $(OBJDIR_DEBUG)/__/__/common/config.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm ../../../bin/libelenart.so
	rm -rf $(OBJDIR_DEBUG)/__/linux32
	rm -rf $(OBJDIR_DEBUG)/__
	rm -rf $(OBJDIR_DEBUG)/__/__/engine/x86
	rm -rf $(OBJDIR_DEBUG)/__/__/engine
	rm -rf $(OBJDIR_DEBUG)/__/__/engine/linux32
	rm -rf $(OBJDIR_DEBUG)/__/__/common

before_release: 
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_RELEASE)/__/linux32 || mkdir -p $(OBJDIR_RELEASE)/__/linux32
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/__/engine/x86 || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/x86
	test -d $(OBJDIR_RELEASE)/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/__/engine/linux32 || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/linux32
	test -d $(OBJDIR_RELEASE)/__/__/common || mkdir -p $(OBJDIR_RELEASE)/__/__/common

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) $(OBJ_RELEASE)  -o $(OUT_RELEASE) $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/linux32/main.o: ../linux32/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../linux32/main.cpp -o $(OBJDIR_RELEASE)/__/linux32/main.o

$(OBJDIR_RELEASE)/__/elenartmachine.o: ../elenartmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../elenartmachine.cpp -o $(OBJDIR_RELEASE)/__/elenartmachine.o

$(OBJDIR_RELEASE)/__/__/engine/x86/x86routines.o: ../../engine/x86/x86routines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/x86/x86routines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/x86/x86routines.o

$(OBJDIR_RELEASE)/__/__/engine/section.o: ../../engine/section.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/section.cpp -o $(OBJDIR_RELEASE)/__/__/engine/section.o

$(OBJDIR_RELEASE)/__/__/engine/rtman.o: ../../engine/rtman.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/rtman.cpp -o $(OBJDIR_RELEASE)/__/__/engine/rtman.o

$(OBJDIR_RELEASE)/__/__/engine/module.o: ../../engine/module.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/module.cpp -o $(OBJDIR_RELEASE)/__/__/engine/module.o

$(OBJDIR_RELEASE)/__/__/engine/linux32/linx32routines.o: ../../engine/linux32/linx32routines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux32/linx32routines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux32/linx32routines.o

$(OBJDIR_RELEASE)/__/__/engine/linux32/elfhelper.o: ../../engine/linux32/elfhelper.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux32/elfhelper.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux32/elfhelper.o

$(OBJDIR_RELEASE)/__/__/common/altstrings.o: ../../common/altstrings.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/altstrings.cpp -o $(OBJDIR_RELEASE)/__/__/common/altstrings.o

$(OBJDIR_RELEASE)/__/__/engine/libman.o: ../../engine/libman.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/libman.cpp -o $(OBJDIR_RELEASE)/__/__/engine/libman.o

$(OBJDIR_RELEASE)/__/__/engine/elenamachine.o: ../../engine/elenamachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/elenamachine.cpp -o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o

$(OBJDIR_RELEASE)/__/__/engine/bytecode.o: ../../engine/bytecode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/bytecode.cpp -o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o

$(OBJDIR_RELEASE)/__/__/common/xmlreader.o: ../../common/xmlreader.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/xmlreader.cpp -o $(OBJDIR_RELEASE)/__/__/common/xmlreader.o

$(OBJDIR_RELEASE)/__/__/common/files.o: ../../common/files.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/files.cpp -o $(OBJDIR_RELEASE)/__/__/common/files.o

$(OBJDIR_RELEASE)/__/__/common/dump.o: ../../common/dump.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/dump.cpp -o $(OBJDIR_RELEASE)/__/__/common/dump.o

$(OBJDIR_RELEASE)/__/__/common/config.o: ../../common/config.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../common/config.cpp -o $(OBJDIR_RELEASE)/__/__/common/config.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm ../../../bin/elenart.so
	rm -rf $(OBJDIR_RELEASE)/__/linux32
	rm -rf $(OBJDIR_RELEASE)/__
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/x86
	rm -rf $(OBJDIR_RELEASE)/__/__/engine
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/linux32
	rm -rf $(OBJDIR_RELEASE)/__/__/common

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release
