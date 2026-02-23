//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Optimization Fixture declarations
//                                             (C)2024-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef BTOPTIMIZATION_H
#define BTOPTIMIZATION_H

#include "tests_common.h"

namespace elena_lang
{
   class BTOptimization : public ExprTest
   {
   protected:
      MemoryDump btRules;
      
      BuildNode  afterOptimization;

      void SetUp() override;

   public:
      void runBTTest();
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
         : offsets(0), expectedSize(0), targetRef(0)
      {
      }
   };

   // Optimization #2 (byRefOp) : "a := b.get()" => "b.get(ref a)"
   class BTOptimization2_1 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #2 (byRefOp) : "a := b.Value" => "b.prop:Value(ref a)"
   class BTOptimization2_2 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #2 (byRefOp) : "a := *b" => "b.prop:Value(ref a)"
   class BTOptimization2_3 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #2 (byRefOp + boxing / unboxing) : "a := b[0]"
   class BTOptimization2_4 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #3 (byRefOp + boxing / unboxing) : "a := b[0]"
   class BTOptimization3 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #4 (intOpWithConsts) : "int r := n + 2" => direct op with consts / #15 (optIntOpWithConsts)
   class BTOptimization4 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #5 (assignIntOpWithConsts) : "n += 2" => direct op with consts
   class BTOptimization5 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #6 (boxingInt) : "var o := n" => optimized boxing
   class BTOptimization6 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #7 : "native branching
   class BTOptimization7 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #8 : "native int branching with constant
   class BTOptimization8 : public BTOptimization
   {
   protected:
      void initModuleScope(ModuleScopeBase* moduleScope, bool declareDefaultMessages) override;

      void SetUp() override;
   };

   // Optimization #9 : double assigning on convertion (int n := b)
   class BTOptimization9 : public BTOptimization
   {
   protected:
      void initModuleScope(ModuleScopeBase* moduleScope, bool declareDefaultMessages) override;

      void SetUp() override;
   };

   // Optimization #10_1 : doubleAssigningIntRealOp (r := n + 2.0)
   class BTOptimization10_1 : public BTOptimization
   {
   protected:
      void initModuleScope(ModuleScopeBase* moduleScope, bool declareDefaultMessages) override;

      void SetUp() override;
   };

   // Optimization #10_1 : doubleAssigningIntRealOp (r := 2.0 + n)
   class BTOptimization10_2 : public BTOptimization10_1
   {
   protected:
      void SetUp() override;
   };

   // Optimization #12 (inplaceCallOp) : "struct X ( int x; ) ..  X x := new X() => call in-place constructor
   class BTOptimization12 : public BTOptimization
   {
   protected:
      void SetUp() override;
   };

   // Optimization #12 (inplaceCallOp) : "struct X ( int x; constructor(int){} ) ..  X x := 2 => call in-place constructor
   class BTOptimization12_2 : public BTOptimization
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

   class VariadicRuntimeSingleDispatch : public MethodScenarioTest
   {
   protected:
      SyntaxNode findTargetNode(int scenario) override;

      void SetUp() override;
   };

   class VariadicCompiletimeSingleDispatch : public MethodScenarioTest
   {
   protected:
      void SetUp() override;
   };

   class VariadicCompiletimeSingleDispatch_WithDifferentArgs : public MethodScenarioTest
   {
   protected:
      void SetUp() override;
   };

   class MethodCallTest : public ScenarioTest
   {
   protected:
      ref_t      argArrayRef;
      ref_t      genericVargRef;
      ref_t      targetVargRef;

      void SetUp() override;

   public:
      void runTest(bool withVariadic, int scenario = 0);
   };

   class ArrayOpTest : public ScenarioTest
   {
   protected:
      ref_t      intNumberRef;
      ref_t      arrayTemplateRef;
      ref_t      elementRef;
      ref_t      arrayRef;

      void SetUp() override;

   public:
      void runTest(int scenario = 0);
   };

   class CallMethodWithoutTarget : public MethodCallTest
   {
   protected:
      void SetUp() override;
   };

   class CallVariadocMethodWithoutTarget : public MethodCallTest
   {
   protected:
      void SetUp() override;
   };

   class CallMethodWithSignatureOfSuperClass : public MethodScenarioTest
   {
   protected:
      void SetUp() override;
   };

   class CallMethodWithNil : public MethodCallTest
   {
   protected:
      void SetUp() override;
   };

   class LambdaTest : public ScenarioTest
   {
   protected:
      ref_t  funcRef;
      ref_t  outputRef;

      BuildNode findOutput(BuildNode root);

      void SetUp() override;

   public:
      void runTest(int scenario = 0);
   };

   class Lambda_CallingPrivateMethod : public LambdaTest
   {
   protected:
      void SetUp() override;
   };

   class IntOperation : public ScenarioTest
   {
   protected:
      ref_t intReference;
      int   expectedError;

      void SetUp() override;

   public:
      void runTest(bool exceptionExpected = false, int scenario = 0);
   };

   class NillableIntAssigning : public IntOperation
   {
   protected:
      void SetUp() override;
   };

   class IntAssigningNil : public IntOperation
   {
   protected:
      void SetUp() override;
   };

   class ByteArrayOperation : public ArrayOpTest
   {
   protected:
      void SetUp() override;
   };

   class ByteArrayOperation2 : public ArrayOpTest
   {
   protected:
      void SetUp() override;
   }; 

   class IntermediateVar : public MethodScenarioTest
   {
   protected:
      void SetUp() override;
   };

   class StructFieldAssigning : public MethodScenarioTest
   {
   protected:
      void SetUp() override;
   };
}

#endif
