#include "pch.h"
// ------------------------------------------------
#include "bt_optimization.h"

#include "compiler.h"

using namespace elena_lang;

TEST_F(BTOptimization1_1, CompilerTest) 
{
   runCompilerTest(false);
}

TEST_F(BTOptimization1_2, CompilerTest)
{
   runCompilerTest(false);
}

TEST_F(BTOptimization1_3, CompilerTest)
{
   runCompilerTest(true);
}

TEST_F(BTOptimization2, CompilerTest)
{
   runCompilerTest(false);
}

TEST_F(BTOptimization4, CompilerTest)
{
   runCompilerTest(false);
}

TEST_F(StructAlignment, CompilerTest)
{
   runTest();
}

TEST_F(PackedStructAlignment, CompilerTest)
{
   runTest();
}

TEST_F(VariadicRuntimeSingleDispatch, CompilerTest)
{
   runTest();
}

TEST_F(VariadicCompiletimeSingleDispatch, CompilerTest)
{
   runTest();
}

TEST_F(CallMethodWithoutTarget, CompilerTest)
{
   runTest(false);
}

TEST_F(CallVariadocMethodWithoutTarget, CompilerTest)
{
   runTest(false);
}

// Test scenario : E.load(new C(), new D()); where: class E { constructor load(params B[] args) {}}, C:B and D:B
TEST_F(VariadicCompiletimeSingleDispatch_WithDifferentArgs, CompilerTest)
{
   runTest();
}

TEST_F(Lambda_CallingPrivateMethod, CompilerTest)
{
   runTest();
}

TEST_F(IntAssigningNil, CompilerTest)
{
   runTest(true);
}

TEST_F(NillableIntAssigning, CompilerTest)
{
   runTest(false);
}