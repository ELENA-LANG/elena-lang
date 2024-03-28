//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Common declarations
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include "compiler.h"

namespace elena_lang 
{
   // --- CompilerEnvironment ---
   class CompilerEnvironment
   {
   public:
      Compiler* createCompiler();

      CompilerEnvironment();
   };
}

#endif // TESTS_COMMON_H