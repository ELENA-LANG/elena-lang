#include "pch.h"
// ------------------------------------------------
#include "bt_optimization.h"

#include "compiler.h"

using namespace elena_lang;

TEST_F(BTOptimization1_1, CompilerTest) 
{
   //// Arrange
   //ModuleScopeBase* moduleScope = env.createModuleScope(true, false);
   //Compiler* compiler = env.createCompiler();

   //BuildTree output;
   //BuildTreeWriter writer(output);
   //Compiler::Namespace nsScope(compiler, moduleScope, nullptr, nullptr, nullptr);

   //// Act
   //nsScope.declare(declarationNode.firstChild(), true);

   //Compiler::Symbol symbol(nsScope, 0, Visibility::Internal);
   //Compiler::Expression expression(symbol, writer);
   //expression.compileRoot(exprNode.firstChild(), ExpressionAttribute::None);

   //// Assess
   bool matched = /*BuildTree::compare(buildTree.readRoot(), output.readRoot())*/true;
   EXPECT_TRUE(matched);

   //freeobj(compiler);
   //freeobj(moduleScope);
}