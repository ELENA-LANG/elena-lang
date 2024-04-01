//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Optimization Fixture declarations
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef BTOPTIMIZATION_H
#define BTOPTIMIZATION_H

#include "pch.h"
#include "tests_common.h"

namespace elena_lang
{
   // Optimization 1 (byRefOp) : "a := b.get()" => "b.get(ref a)"
   class BTOptimization1_1 : public testing::Test
   {
   protected:
      SyntaxTree syntaxTree;
      BuildTree  buildTree;

      SyntaxNode declarationNode;
      SyntaxNode exprNode;

      CompilerEnvironment env;

      void SetUp() override;
   };
}

#endif
