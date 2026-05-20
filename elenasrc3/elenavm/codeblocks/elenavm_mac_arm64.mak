#------------------------------------------------------------------------------#
# ELENA make file                                                              #
#------------------------------------------------------------------------------#

WORKDIR = `pwd`

CC = clang
CXX = clang++
AR = ar
LD = clang++
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
OUT_RELEASE = ../../../bin/libelenavm60_64.dylib
INSTALL_NAME_RELEASE = /usr/local/lib/elena/libelenavm60_64.dylib

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/common/dump.o $(OBJDIR_RELEASE)/__/__/common/files.o $(OBJDIR_RELEASE)/__/__/common/paths.o $(OBJDIR_RELEASE)/__/__/common/ustring.o $(OBJDIR_RELEASE)/__/__/engine/elenamachine.o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o $(OBJDIR_RELEASE)/__/__/engine/gcroutines.o $(OBJDIR_RELEASE)/__/__/engine/libman.o $(OBJDIR_RELEASE)/__/__/engine/section.o $(OBJDIR_RELEASE)/__/__/engine/module.o $(OBJDIR_RELEASE)/__/__/engine/rtmanager.o $(OBJDIR_RELEASE)/__/__/engine/macos/macroutines.o $(OBJDIR_RELEASE)/__/elenavmmachine.o  $(OBJDIR_RELEASE)/__/__/engine/aarch64/aarch64routines.o $(OBJDIR_RELEASE)/__/macos/macsection.o $(OBJDIR_RELEASE)/__/macos/elenamacvmachine.o $(OBJDIR_RELEASE)/__/__/engine/arm64compiler.o $(OBJDIR_RELEASE)/__/macos/main.o $(OBJDIR_RELEASE)/__/__/engine/xmlprojectbase.o $(OBJDIR_RELEASE)/__/__/engine/jitcompiler.o $(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o $(OBJDIR_RELEASE)/__/__/engine/codescope.o $(OBJDIR_RELEASE)/__/__/common/xmltree.o $(OBJDIR_RELEASE)/__/__/common/config.o $(OBJDIR_RELEASE)/__/__/engine/jitlinker.o

all: release

clean: clean_release

before_release:
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_RELEASE)/__ || mkdir -p $(OBJDIR_RELEASE)/__
	test -d $(OBJDIR_RELEASE)/__/__/engine || mkdir -p $(OBJDIR_RELEASE)/__/__/engine
	test -d $(OBJDIR_RELEASE)/__/__/engine/linux || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/linux
	test -d $(OBJDIR_RELEASE)/__/__/engine/macos || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/macos
	test -d $(OBJDIR_RELEASE)/__/__/engine/aarch64 || mkdir -p $(OBJDIR_RELEASE)/__/__/engine/aarch64
	test -d $(OBJDIR_RELEASE)/__/macos || mkdir -p $(OBJDIR_RELEASE)/__/macos
	test -d $(OBJDIR_RELEASE)/__/linux || mkdir -p $(OBJDIR_RELEASE)/__/linux
	test -d $(OBJDIR_RELEASE)/__/__/common || mkdir -p $(OBJDIR_RELEASE)/__/__/common

after_release:

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -dynamiclib -Wl,-install_name,$(INSTALL_NAME_RELEASE) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

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

$(OBJDIR_RELEASE)/__/__/engine/bytecode.o: ../../engine/bytecode.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/bytecode.cpp -o $(OBJDIR_RELEASE)/__/__/engine/bytecode.o

$(OBJDIR_RELEASE)/__/__/engine/gcroutines.o: ../../engine/gcroutines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/gcroutines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/gcroutines.o

$(OBJDIR_RELEASE)/__/__/engine/libman.o: ../../engine/libman.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/libman.cpp -o $(OBJDIR_RELEASE)/__/__/engine/libman.o

$(OBJDIR_RELEASE)/__/__/engine/section.o: ../../engine/section.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/section.cpp -o $(OBJDIR_RELEASE)/__/__/engine/section.o

$(OBJDIR_RELEASE)/__/__/engine/module.o: ../../engine/module.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/module.cpp -o $(OBJDIR_RELEASE)/__/__/engine/module.o

$(OBJDIR_RELEASE)/__/__/engine/rtmanager.o: ../../engine/rtmanager.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/rtmanager.cpp -o $(OBJDIR_RELEASE)/__/__/engine/rtmanager.o

$(OBJDIR_RELEASE)/__/__/engine/macos/macroutines.o: ../../engine/macos/macroutines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/macos/macroutines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/macos/macroutines.o

$(OBJDIR_RELEASE)/__/elenavmmachine.o: ../elenavmmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../elenavmmachine.cpp -o $(OBJDIR_RELEASE)/__/elenavmmachine.o

$(OBJDIR_RELEASE)/__/__/engine/aarch64/aarch64routines.o: ../../engine/aarch64/aarch64routines.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/aarch64/aarch64routines.cpp -o $(OBJDIR_RELEASE)/__/__/engine/aarch64/aarch64routines.o

$(OBJDIR_RELEASE)/__/macos/macsection.o: ../macos/macsection.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../macos/macsection.cpp -o $(OBJDIR_RELEASE)/__/macos/macsection.o

$(OBJDIR_RELEASE)/__/macos/elenamacvmachine.o: ../macos/elenamacvmachine.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../macos/elenamacvmachine.cpp -o $(OBJDIR_RELEASE)/__/macos/elenamacvmachine.o

$(OBJDIR_RELEASE)/__/macos/main.o: ../macos/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../macos/main.cpp -o $(OBJDIR_RELEASE)/__/macos/main.o

$(OBJDIR_RELEASE)/__/__/engine/arm64compiler.o: ../../engine/arm64compiler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/arm64compiler.cpp -o $(OBJDIR_RELEASE)/__/__/engine/arm64compiler.o

$(OBJDIR_RELEASE)/__/__/engine/xmlprojectbase.o: ../../engine/xmlprojectbase.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/xmlprojectbase.cpp -o $(OBJDIR_RELEASE)/__/__/engine/xmlprojectbase.o

$(OBJDIR_RELEASE)/__/__/engine/jitcompiler.o: ../../engine/jitcompiler.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/jitcompiler.cpp -o $(OBJDIR_RELEASE)/__/__/engine/jitcompiler.o

$(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o: ../../engine/linux/presenter.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/linux/presenter.cpp -o $(OBJDIR_RELEASE)/__/__/engine/linux/presenter.o

$(OBJDIR_RELEASE)/__/__/engine/codescope.o: ../../engine/codescope.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../engine/codescope.cpp -o $(OBJDIR_RELEASE)/__/__/engine/codescope.o

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
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/linux
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/macos
	rm -rf $(OBJDIR_RELEASE)/__/__/engine/aarch64
	rm -rf $(OBJDIR_RELEASE)/__/macos
	rm -rf $(OBJDIR_RELEASE)/__/linux
	rm -rf $(OBJDIR_RELEASE)/__/__/common

.PHONY: before_release after_release clean_release
