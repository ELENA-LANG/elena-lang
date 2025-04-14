/*
* BTOptimization1_1
* -------------
*
* BTOptimization1_2
* -----------------
*
* BTOptimization1_3
* ----------------*
*
* BTOptimization1_4
* -----------------
*
* BTOptimization2
* ---------------
*
* BTOptimization4
* ---------------
*
* StructAlignment
* ---------------
* 
* PackedStructAlignment
* ---------------------
* 
* VariadicRuntimeSingleDispatch
* -----------------------------
* 
* VariadicCompiletimeSingleDispatch
* ---------------------------------
* 
* CallMethodWithoutTarget
* -----------------------
* 
* CallVariadocMethodWithoutTarget
* -------------------------------
* 
* CallMethodWithSignatureOfSuperClass
* -----------------------------------
* 
* CallMethodWithNil
* -----------------
* 
* VariadicCompiletimeSingleDispatch_WithDifferentArgs
* ---------------------------------------------------
* 
* Lambda_CallingPrivateMethod
* ---------------------------
* 
* IntAssigningNil
* ---------------
* 
* NillableIntAssigning
* --------------------
* 
*/


#include "pch.h"
// ------------------------------------------------
#include "bt_optimization.h"

#include "compiler.h"

using namespace elena_lang;

TEST_F(BTOptimization1_1, BuildTest) 
{
   runBuildTest();
}

TEST_F(BTOptimization1_2, BuildTest)
{
   runBuildTest();
}

TEST_F(BTOptimization1_3, BuildTest)
{
   runBuildTest(false, true);
}

TEST_F(BTOptimization1_4, BuildTest)
{
   runBuildTest(false, true);
}

TEST_F(BTOptimization2, BuildTest)
{
   runBuildTest();
}

TEST_F(BTOptimization4, BuildTest)
{
   runBuildTest();
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

TEST_F(CallMethodWithSignatureOfSuperClass, BuildTest)
{
   runTest();
}

TEST_F(CallMethodWithNil, BuildTest)
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