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