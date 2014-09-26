//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		Asm2BinX main file
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// --------------------------------------------------------------------------
#include "elena.h"
#include "x86assembler.h"
#include "ecassembler.h"
#include "source.h"

#define BUILD_NUMBER 8

int main(int argc, char* argv[])
{
   printf("ELENA Assembler Compiler %d.%d.%d (C)2007-2014 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, BUILD_NUMBER);

   if (argc<2) {
      printf("asm2bin <file.asm> <output path>");
      return 0;
   }
   _ELENA_::Path target;

   bool esmMode = _ELENA_::Path::checkExtension(_ELENA_::Path(argv[1]), _T("esm"));

   if (argc==3) {
      _ELENA_::FileName name(argv[1]);

		target.copy(argv[2]);
		target.combine(name);
   }
   else target.copy(argv[1]);

   target.changeExtension(_T("bin"));

   _ELENA_::Path source(argv[1]);
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

      printf("Successfully compiled");
   }
   catch(_ELENA_::InvalidChar& e) {
      printf("(%d): Invalid char %c\n", e.row, e.ch);
   }
   catch(_ELENA_::AssemblerException& e) {
      printf(e.message, e.row);
   }
   return 0;
}
