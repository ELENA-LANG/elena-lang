//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		Asm2BinX main file
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// --------------------------------------------------------------------------
#include "elena.h"
#include "x86assembler.h"
#include "amd64assembler.h"
#include "ppc64assembler.h"
#include "ecassembler.h"
#include "source.h"
#include "asm2binx.h"
#include "assemblerException.h"
#include "preprocessor.h"
#include "preProcessorException.h"

enum CPUType
{
   cpuX86 = 0,
   cpuAMD64 = 1,
   cpuPower64Le = 2
};

bool defineType(_ELENA_::ident_t arg, CPUType& type)
{
   if (arg.compare("-amd64")) {
      type = cpuAMD64;
   }
   else if (arg.compare("-ppc64le")) {
      type = cpuPower64Le;
   }
   else return false;

   return true;
}

int main(int argc, char* argv[])
{
   printf("ELENA Assembler Compiler %d.%d.%d (C)2011-2021 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, REVISION_NUMBER);

   if (argc<2) {
      printf("asm2binx [-amd64] <file.asm> <output path> <postfix>");
      return 0;
   }
   _ELENA_::Path target;
   _ELENA_::IdentifierString postfix("32");

   CPUType type = cpuX86;

   int fileIndex = 1;
   if (argc == 4) {
      fileIndex = 2;
      _ELENA_::FileName name(argv[2], true);

      target.copy(argv[3]);
      target.combine(name.c_str());

      if (defineType(_ELENA_::ident_t(argv[1]), type)) {

      }
      else if (_ELENA_::ident_t(argv[1]).startsWith("-p")) {
         postfix.copy(argv[1] + 2);
      }
      else {
         printf("Invalid argument list");
         return -1;
      }
   }
   else if (argc==3) {
      if (defineType(_ELENA_::ident_t(argv[1]), type)) {
         fileIndex = 2;

         target.copy(argv[2]);
      }
      else {
         _ELENA_::FileName name(argv[1], true);

         target.copy(argv[2]);
         target.combine(name.c_str());
      }
   }
   else target.copy(argv[1]);

   bool esmMode = _ELENA_::Path::checkExtension(argv[fileIndex], "esm");
   if (esmMode) {
		target.changeExtension("nl");
   }
   else target.changeExtension("bin");

   //_ELENA_::PreProcessor pp(amd64Mode ? argv[2] : argv[1]);
   //if (!pp.isOpened()) {
   //   printf("Cannot open the file %s", amd64Mode ? argv[2] : argv[1]);
   //   return -1;
   //}
   //try
   //{
	  // pp.preProcess();
   //}
   //catch (_ELENA_::PreProcessorException& e)
   //{
	  // if (e.macroName == NULL)
		 //  printf(e.message);
	  // else if (e.macroName == NULL && e.row >= 0)
		 //  printf(e.message, e.row);
	  // else if (e.row == -1)
		 //  printf(e.message, e.macroName);
	  // else
		 //  printf(e.message, e.macroName, e.row);
	  // return -1;
   //}

   _ELENA_::Path source(argv[fileIndex]);
   _ELENA_::TextFileReader reader(source.c_str(), _ELENA_::feUTF8, true);
   if (!reader.isOpened()) {
      printf("Cannot open the file %s", source.c_str());
      return -1;
   }

   _ELENA_::Path::create(NULL, target.c_str());

   try {
      if (esmMode) {
	      _ELENA_::ECodesAssembler assembler(postfix.c_str());
		   assembler.compile(&reader, target.c_str());
      }
      else if (type == cpuAMD64) {
         _ELENA_::AMD64Assembler	assembler;
         assembler.compile(&reader, target.c_str());
      }
      else if (type == cpuPower64Le) {
         _ELENA_::PPC64Assembler	assembler;
         assembler.compile(&reader, target.c_str());
      }
      else {
	      _ELENA_::x86Assembler	assembler;
		   assembler.compile(&reader, target.c_str());
      }

      printf("Successfully compiled\n");
   }
   catch(_ELENA_::InvalidChar& e) {
      printf("(%d): Invalid char %c\n", e.row, e.ch);
	  return -1;
   }
   catch(_ELENA_::AssemblerException& e) {
	   if (e.messageArguments == NULL)
		   printf(e.message, e.row);
	   else if (e.procedureName == NULL)
		   printf(e.message, e.messageArguments.c_str(), e.procedureNumber);
	   else
		   printf(e.message, e.messageArguments.c_str(), e.procedureName.c_str());
	   return -1;
   }
   return 0;
}
