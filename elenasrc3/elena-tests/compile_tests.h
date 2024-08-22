//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Compile Test Fixture declarations
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef COMPILE_TESTS_H
#define COMPILE_TESTS_H

#include "tests_common.h"

namespace elena_lang
{
   class CompileScenarioTest : public CompileTest
   {
   protected:
      ref_t intReference;

      virtual SyntaxNode findTargetNode(ref_t targetRef);
      virtual SyntaxNode findClassNode(ref_t targetRef);

      void SetUp() override;

   public:
      void runTest(ref_t targetRef, int exptectedError);
   };

   class AccessPrivateField : public CompileScenarioTest
   {
   protected:
      void SetUp() override;
   };
}

#endif
