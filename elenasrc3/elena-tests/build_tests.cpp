#include "pch.h"
// ------------------------------------------------
#include "bt_optimization.h"

#include "compiler.h"

using namespace elena_lang;

TEST_F(BTOptimization1_1, BuildTest) 
{
   runBuildTest(false);
}

TEST_F(BTOptimization1_2, BuildTest)
{
   runBuildTest(false);
}

TEST_F(BTOptimization1_3, BuildTest)
{
   runBuildTest(true);
}

TEST_F(BTOptimization2, BuildTest)
{
   runBuildTest(false);
}

TEST_F(BTOptimization4, BuildTest)
{
   runBuildTest(false);
}

TEST_F(StructAlignment, BuildTest)
{
   runTest();
}

TEST_F(PackedStructAlignment, BuildTest)
{
   runTest();
}

TEST_F(VariadicRuntimeSingleDispatch, BuildTest)
{
   runTest();
}

TEST_F(VariadicCompiletimeSingleDispatch, BuildTest)
{
   runTest();
}

TEST_F(CallMethodWithoutTarget, BuildTest)
{
   runTest(false);
}

TEST_F(CallVariadocMethodWithoutTarget, BuildTest)
{
   runTest(false);
}

// Test scenario : E.load(new C(), new D()); where: class E { constructor load(params B[] args) {}}, C:B and D:B
TEST_F(VariadicCompiletimeSingleDispatch_WithDifferentArgs, BuildTest)
{
   runTest();
}

TEST_F(Lambda_CallingPrivateMethod, BuildTest)
{
   runTest();
}

TEST_F(IntAssigningNil, BuildTest)
{
   runTest(true);
}

TEST_F(NillableIntAssigning, BuildTest)
{
   runTest(false);
}