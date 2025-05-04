//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Optimization Fixture declarations
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CONSTRUCTOR_TESTS_H
#define CONSTRUCTOR_TESTS_H

#include "tests_common.h"

namespace elena_lang
{
   class VariadicCompiletimeConstructorSingleDispatch : public MethodScenarioTest
   {
   protected:
      void SetUp() override;
   };

   class CallPrivateConstructorDirectly : public MethodScenarioTest
   {
   protected:
      SyntaxNode findTargetNode(int scenario) override;

      void SetUp() override;
   };
}

#endif
