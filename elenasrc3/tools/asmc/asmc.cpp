//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		Asm2BinX main file
//
//                                              (C)2021-2025, by Aleksey Rakov
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

#if (defined(_WIN32) || defined(__WIN32__))
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

#elif defined(__unix__)

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

#ifdef __FreeBSD__

constexpr auto targetPlatform = ASM_FREEBSD_TARGET;

#elif __unix__

constexpr auto targetPlatform = ASM_LNX_TARGET;

#elif defined(_WIN32) || defined(_WIN64) 

constexpr auto targetPlatform = ASM_WIN_TARGET;

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

template<class AssemblyT> void compileAssembly(path_t source, path_t target, ustr_t platform)
{
   Module         targetModule(BINARY_MODULE);

   TextFileReader reader(source, FileEncoding::UTF8, true);
   if (!reader.isOpen()) {
      printLine(ASM_CANNOTOPEN_INPUT, source);
      throw ExceptionBase();
   }

   AssemblyT      assembler(4, &reader, &targetModule);

   assembler.defineMacro(platform, true);

   assembler.compile();

   FileWriter writer(target, FileEncoding::Raw, false);
   if (!targetModule.save(writer)) {
      printLine(ASM_CANNOTCREATE_OUTPUT, target);
      throw ExceptionBase();
   }
}

void compileByteCode(path_t source, path_t target, bool mode64, int rawDataAlignment, bool supportStdMode)
{
   FileNameString sourceName(source, true);
   NamespaceString  name;

   name.pathToName(sourceName.str());

   Module         targetModule(*name);

   TextFileReader reader(source, FileEncoding::UTF8, true);
   if (!reader.isOpen()) {
      printLine(ASM_CANNOTOPEN_INPUT, source);
      throw ExceptionBase();
   }

   ByteCodeAssembler assembler(4, &reader, &targetModule, mode64, rawDataAlignment, supportStdMode);

   assembler.compile();

   FileWriter writer(target, FileEncoding::Raw, false);
   if(!targetModule.save(writer)) {
      printLine(ASM_CANNOTCREATE_OUTPUT, target);

      throw ExceptionBase();
   }
}

int main(int argc, char* argv[])
{
   printf(ASM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ASM_REVISION_NUMBER);
   if (argc < 2 || argc > 6) {
      printf(ASM_HELP);
      return  EXIT_FAILURE;
   }

   IdentifierString platform(targetPlatform);
   PathString source;
   PathString target;
   PathString targetName;
   CompileMode mode = CompileMode::x86;
   bool supportStdMode = false;
   int optionIndex = 0;
   for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
         optionIndex = i;

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
         else if (arg.compare(ASM_AARCH64_MODE)) {
            mode = CompileMode::arm64;
         }
         else if (arg.compare(ASM_WIN_TARGET_MODE)) {
            platform.copy(ASM_WIN_TARGET);
         }
         else if (arg.compare(ASM_LNX_TARGET_MODE)) {
            platform.copy(ASM_LNX_TARGET);
         }
         else if (arg.compare(ASM_FREEBSD_TARGET_MODE)) {
            platform.copy(ASM_FREEBSD_TARGET);
         }
         else if (arg.compare(BC_32_MODE)) {
            mode = CompileMode::bc32;
#if (defined(_WIN32) || defined(__WIN32__))
            supportStdMode = true;
#endif
         }
         else if (arg.compare(BC_64_MODE)) {
            mode = CompileMode::bc64;
         }
         else {
            printf(ASM_HELP);
            return  EXIT_FAILURE;
         }
      }
      else if (i == optionIndex + 1) {
         source.copy(argv[i]);
      }
      else if (i == optionIndex + 2) {
         target.copy(argv[i]);
      }
      else if (i == optionIndex + 3) {
         targetName.copy(argv[i]);
      }
      else {
         printf(ASM_HELP);
         return  EXIT_FAILURE;
      }
   }

   if (target.empty()) {
      target.copy(*source);

      if (!targetName.empty()) {
         PathString temp;
         temp.copySubPath(*target, true);
         target.combine(*targetName);
      }
   }
   else if (!targetName.empty()) {
      target.combine(*targetName);
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

            compileAssembly<X86Assembler>(*source, *target, *platform);
            break;
         }
         case CompileMode::amd64:
         {
            printLine(ASM_COMPILE_X86_64, *source);

            target.changeExtension("bin");

            compileAssembly<X86_64Assembler>(*source, *target, *platform);
            break;
         }
         case CompileMode::ppc64le:
         {
            printLine(ASM_COMPILE_PPC64le, *source);

            target.changeExtension("bin");

            compileAssembly<PPC64Assembler>(*source, *target, *platform);

            break;
         }
         case CompileMode::arm64:
         {
            printLine(ASM_COMPILE_ARM64, *source);

            target.changeExtension("bin");

            compileAssembly<Arm64Assembler>(*source, *target, *platform);

            break;
         }
         case CompileMode::bc32:
         {
            printLine(BC_COMPILE_32, *source);

            target.changeExtension("nl");

            compileByteCode(*source, *target, false, 4, supportStdMode);

            break;
         }
         case CompileMode::bc64:
         {
            printLine(BC_COMPILE_64, *source);

            target.changeExtension("nl");

            compileByteCode(*source, *target, true, 16, supportStdMode);

            break;
         }
         default:
            // to make compiler happy
            break;
      }

      printf(ASM_DONE);

      return EXIT_SUCCESS;
   }
   catch (InvalidChar& e) {
      printf("(%d,%d): Invalid char %c\n", e.lineInfo.row, e.lineInfo.column, e.ch);
      return EXIT_FAILURE;
   }
   catch (SyntaxError& e) {
      printf(e.message, e.lineInfo.row, e.lineInfo.column);
      return EXIT_FAILURE;
   }
   catch (ProcedureError& e) {
      printf(e.message, *e.arg, *e.name);
      return EXIT_FAILURE;
   }
   catch(ExceptionBase&) {
      return EXIT_FAILURE;
   }
}
