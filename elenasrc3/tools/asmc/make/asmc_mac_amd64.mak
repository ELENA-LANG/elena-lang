#------------------------------------------------------------------------------#
# ELENA make file                                                              #
#------------------------------------------------------------------------------#

WORKDIR = `pwd`

CC = clang
CXX = clang++
AR = ar
LD = clang++
WINDRES = windres

INC = -I.. -I../../../engine -I../../../common
RESINC = 
LIBDIR = 
LIB = 
CFLAGS = -Wall -std=c++20 -m64 -static-libgcc -static-libstdc++
LDFLAGS = -m64 -ldl -fuse-ld=lld

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = ../../../temp/asm64-cli/
DEP_RELEASE = 
OUT_RELEASE = ../../../../bin/asm64-cli

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/__/common/paths.o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/__/engine/module.o $(OBJDIR_RELEASE)/__/__/__/engine/scriptreader.o $(OBJDIR_RELEASE)/__/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/__/engine/section.o $(OBJDIR_RELEASE)/__/asmc.o $(OBJDIR_RELEASE)/__/assembler.o $(OBJDIR_RELEASE)/__/x86assembler.o $(OBJDIR_RELEASE)/__/ppc64assembler.o  $(OBJDIR_RELEASE)/__/armassembler.o $(OBJDIR_RELEASE)/__/bcassembler.o $(OBJDIR_RELEASE)/__/__/__/engine/x86helper.o

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

$(OBJDIR_RELEASE)/__/__/__/common/paths.o: ../../../common/paths.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/paths.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/paths.o

$(OBJDIR_RELEASE)/__/__/__/common/ustring.o: ../../../common/ustring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../common/ustring.cpp -o $(OBJDIR_RELEASE)/__/__/__/common/ustring.o

$(OBJDIR_RELEASE)/__/__/__/engine/module.o: ../../../engine/module.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/module.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/module.o

$(OBJDIR_RELEASE)/__/__/__/engine/scriptreader.o: ../../../engine/scriptreader.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/scriptreader.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/scriptreader.o

$(OBJDIR_RELEASE)/__/__/__/engine/bytecode.o: ../../../engine/bytecode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/bytecode.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/bytecode.o

$(OBJDIR_RELEASE)/__/__/__/engine/section.o: ../../../engine/section.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/section.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/section.o

$(OBJDIR_RELEASE)/__/asmc.o  : ../asmc.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../asmc.cpp -o $(OBJDIR_RELEASE)/__/asmc.o

$(OBJDIR_RELEASE)/__/assembler.o  : ../assembler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../assembler.cpp -o $(OBJDIR_RELEASE)/__/assembler.o

$(OBJDIR_RELEASE)/__/x86assembler.o  : ../x86assembler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../x86assembler.cpp -o $(OBJDIR_RELEASE)/__/x86assembler.o

$(OBJDIR_RELEASE)/__/__/__/engine/x86helper.o: ../../../engine/x86helper.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../engine/x86helper.cpp -o $(OBJDIR_RELEASE)/__/__/__/engine/x86helper.o

$(OBJDIR_RELEASE)/__/ppc64assembler.o  : ../ppc64assembler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../ppc64assembler.cpp -o $(OBJDIR_RELEASE)/__/ppc64assembler.o

$(OBJDIR_RELEASE)/__/armassembler.o  : ../armassembler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../armassembler.cpp -o $(OBJDIR_RELEASE)/__/armassembler.o

$(OBJDIR_RELEASE)/__/bcassembler.o  : ../bcassembler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../bcassembler.cpp -o $(OBJDIR_RELEASE)/__/bcassembler.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/__
	rm -rf $(OBJDIR_RELEASE)/__/__/__/engine
	rm -rf $(OBJDIR_RELEASE)/__/__/__/common

.PHONY: before_release after_release clean_release
