//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		Asm2BinX main file
//
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <cstdarg>

#include "elena.h"
#include "asmconst.h"

#include "x86assembler.h"
#include "ppc64assembler.h"
#include "armassembler.h"
#include "bcassembler.h"
#include "textparser.h"
#include "module.h"

using namespace elena_lang;

#ifdef _MSC_VER
void print(const wchar_t* wstr, ...)
{
   va_list argptr;
   va_start(argptr, wstr);

   vwprintf(wstr, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

void printLine(ustr_t mssg, path_t path)
{
   WideMessage wmssg(mssg);

   print(wmssg.str(), path.str());
}
#elif __GNUG__

void print(const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

void printLine(ustr_t mssg, path_t path)
{
   print(mssg.str(), path.str());
}

#endif

enum class CompileMode
{
   x86,
   amd64,
   ppc64le,
   arm64,
   bc32,
   bc64
};

template<class AssemblyT> void compileAssembly(path_t source, path_t target)
{
   Module         targetModule(BINARY_MODULE);

   TextFileReader reader(source, FileEncoding::UTF8, true);
   if (!reader.isOpen()) {
      printLine(ASM_CANNOTOPEN_INPUT, source);
      throw ExceptionBase();
   }

   AssemblyT      assembler(4, &reader, &targetModule);

   assembler.compile();

   FileWriter writer(target, FileEncoding::Raw, false);
   if (!targetModule.save(writer)) {
      printf(ASM_CANNOTCREATE_OUTPUT);
      throw ExceptionBase();
   }
}

void compileByteCode(path_t source, path_t target, bool mode64)
{
   FileNameString sourceName(source, true);
   ReferenceName  name;

   name.pathToName(sourceName.str());

   Module         targetModule(*name);

   TextFileReader reader(source, FileEncoding::UTF8, true);
   if (!reader.isOpen()) {
      printLine(ASM_CANNOTOPEN_INPUT, source);
      throw ExceptionBase();
   }

   ByteCodeAssembler assembler(4, &reader, &targetModule, mode64);

   assembler.compile();

   FileWriter writer(target, FileEncoding::Raw, false);
   if(!targetModule.save(writer)) {
      printf(ASM_CANNOTCREATE_OUTPUT);
      throw ExceptionBase();
   }
}

int main(int argc, char* argv[])
{
   printf(ASM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ASM_REVISION_NUMBER);
   if (argc < 2 || argc > 4) {
      printf(ASM_HELP);
      return  -1;
   }

   PathString source;
   PathString target;
   CompileMode mode = CompileMode::x86;
   for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
         ustr_t arg(argv[i] + 1);
         if (arg.compare(ASM_X86_MODE)) {
            mode = CompileMode::x86;
         }
         else if (arg.compare(ASM_AMD64_MODE)) {
            mode = CompileMode::amd64;
         }
         else if (arg.compare(ASM_PPC64le_MODE)) {
            mode = CompileMode::ppc64le;
         }
         else if (arg.compare(ASM_ARM64_MODE)) {
            mode = CompileMode::arm64;
         }
         else if (arg.compare(BC_32_MODE)) {
            mode = CompileMode::bc32;
         }
         else if (arg.compare(BC_64_MODE)) {
            mode = CompileMode::bc64;
         }
         else {
            printf(ASM_HELP);
            return  -1;
         }
      }
      else if (i == 2) {
         source.copy(argv[i]);
      }
      else if (i == 3) {
         target.copy(argv[i]);
      }
      else {
         printf(ASM_HELP);
         return  -1;
      }
   }

   if (target.empty()) {
      target.copy(*source);
   }
   else {
      FileNameString name(*source, true);
      target.combine(*name);
   }

   PathUtil::recreatePath(*target);

   try
   {
      switch (mode) {
         case CompileMode::x86:
         {
            printLine(ASM_COMPILE_X86, *source);

            target.changeExtension("bin");

            compileAssembly<X86Assembler>(*source, *target);
            break;
         }
         case CompileMode::amd64:
         {
            printLine(ASM_COMPILE_X86_64, *source);

            target.changeExtension("bin");

            compileAssembly<X86_64Assembler>(*source, *target);
            break;
         }
         case CompileMode::ppc64le:
         {
            printLine(ASM_COMPILE_PPC64le, *source);

            target.changeExtension("bin");

            compileAssembly<PPC64Assembler>(*source, *target);

            break;
         }
         case CompileMode::arm64:
         {
            printLine(ASM_COMPILE_ARM64, *source);

            target.changeExtension("bin");

            compileAssembly<Arm64Assembler>(*source, *target);

            break;
         }
         case CompileMode::bc32:
         {
            printLine(BC_COMPILE_32, *source);

            target.changeExtension("nl");

            compileByteCode(*source, *target, false);

            break;
         }
         case CompileMode::bc64:
         {
            printLine(BC_COMPILE_64, *source);

            target.changeExtension("nl");

            compileByteCode(*source, *target, true);

            break;
         }
         default:
            // to make compiler happy
            break;
      }

      printf(ASM_DONE);

      return 0;
   }
   catch (InvalidChar& e) {
      printf("(%d,%d): Invalid char %c\n", e.lineInfo.row, e.lineInfo.column, e.ch);
      return -1;
   }
   catch (SyntaxError& e) {
      printf(e.message, e.lineInfo.row, e.lineInfo.column);
      return -1;
   }
   catch (ProcedureError& e) {
      printf(e.message, *e.name, *e.arg);
      return -1;
   }
   catch(ExceptionBase&) {
      return -1;
   }
}
