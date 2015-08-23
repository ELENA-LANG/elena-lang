//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		Asm2BinX main file
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// --------------------------------------------------------------------------
#include "elena.h"
#include "x86assembler.h"
#include "ecassembler.h"
#include "source.h"

#define REVISION_NUMBER   1

int main(int argc, char* argv[])
{
   printf("ELENA Assembler Compiler %d.%d.%d (C)2007-2015 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, REVISION_NUMBER);

   if (argc<2) {
      printf("asm2bin <file.asm> <output path>");
      return 0;
   }
   _ELENA_::Path target;

   bool esmMode = _ELENA_::Path::checkExtension(argv[1], "esm");

   if (argc==3) {
      _ELENA_::FileName name;
      _ELENA_::FileName::load(name, argv[1]);

      _ELENA_::Path::loadPath(target, argv[2]);
		target.combine(name);
   }
   else _ELENA_::Path::loadPath(target, argv[1]);

   if (esmMode) {
		target.changeExtension("nl");
   }
   else target.changeExtension("bin");

   _ELENA_::Path source;
   _ELENA_::Path::loadPath(source, argv[1]);

   _ELENA_::TextFileReader reader(source, _ELENA_::feUTF8, true);
   if (!reader.isOpened()) {
      printf("Cannot open the file");
      return -1;
   }

   _ELENA_::Path::create(NULL, target);

   try {
      if (esmMode) {
	      _ELENA_::ECodesAssembler	assembler;
		   assembler.compile(&reader, target);
      }
      else {
	      _ELENA_::x86Assembler	assembler;
		   assembler.compile(&reader, target);
      }

      printf("Successfully compiled\n");
   }
   catch(_ELENA_::InvalidChar& e) {
      printf("(%d): Invalid char %c\n", e.row, e.ch);
   }
   catch(_ELENA_::AssemblerException& e) {
      printf(e.message, e.row);
   }
   return 0;
}
