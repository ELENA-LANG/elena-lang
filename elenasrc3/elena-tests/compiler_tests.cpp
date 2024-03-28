#include "pch.h"
// ------------------------------------------------
#include "bt_optimization.h"

#include "compiler.h"

using namespace elena_lang;

TEST_F(BTOptimization1_1, CompilerTest) 
{
   // Arrange
   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   Compiler::SymbolScope rootScope(nullptr, 0, Visibility::Internal);
   Compiler::Expression expression(compiler, rootScope, writer);
   
   // Act
   expression.compileRoot(syntaxTree.readRoot(), ExpressionAttribute::None);

   // Assess
   bool matched = BuildTree::compare(buildTree.readRoot(), output.readRoot());
   EXPECT_TRUE(matched);
}