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
   class BTOptimization : public testing::Test
   {
   protected:
      MemoryDump btRules;

      SyntaxTree syntaxTree;
      BuildTree  buildTree;

      SyntaxNode declarationNode;
      SyntaxNode exprNode;

      BuildNode  beforeOptimization;
      BuildNode  afterOptimization;

      CompilerEnvironment env;

      void SetUp() override;

   public:
      void runBTTest();
      void runCompilerTest(bool declareOperators);
   };

   class StructTest : public testing::Test
   {
   protected:
      SyntaxTree syntaxTree;

      SyntaxNode declarationNode;

      List<int>  offsets;
      pos_t      expectedSize;

      ref_t      targetRef;

      CompilerEnvironment env;

      bool validateStructInfo(ClassInfo& structInfo);

      void SetUp() override;

   public:
      void runTest();

      StructTest()
         : offsets(0)
      {
      }
   };

   // Optimization #2 (byRefOp) : "a := b.get()" => "b.get(ref a)"
   class BTOptimization1_1 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #2 (byRefOp) : "a := b.Value" => "b.prop:Value(ref a)"
   class BTOptimization1_2 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #2 (byRefOp) : "a := *b" => "b.prop:Value(ref a)"
   class BTOptimization1_3 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #3 (intCopying) : "int n := 2" => direct assigning
   class BTOptimization2 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // --- StructAlignment ---
   class StructAlignment : public StructTest
   {
   protected:
      void SetUp() override;
   };

   // --- PackedStructAlignment ---
   class PackedStructAlignment : public StructTest
   {
   protected:
      void SetUp() override;
   };

   // --- ComplexStructAlignment ---
   class ComplexStructAlignment : public StructTest
   {
   protected:
      void SetUp() override;
   };

   // --- PrimitiveStructAlignment ---
   class PrimitiveStructAlignment : public StructTest
   {
   protected:
      void SetUp() override;
   };
}

#endif
