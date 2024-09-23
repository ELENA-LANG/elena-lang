#include "pch.h"
// ------------------------------------------------
#include "serializer.h"
#include "bcwriter.h"

#include "compile_tests.h"
#include "scenario_consts.h"

using namespace elena_lang;

// ==== Tests Scenarios ===

constexpr auto PrivateField_Scenario1 = "class (nameattr (identifier \"A\" ())field (attribute -2147467262 ()nameattr (identifier \"_x\" ()))method (nameattr (identifier \"setX\" ())code (expression (assign_operation (object (identifier \"_x\" ())expression (object (integer \"2\" ())))))))class (nameattr 62 (identifier \"B\" ())parent (type (identifier \"A\" ()))method (nameattr (identifier \"setParentX\" ())code (expression (assign_operation (object (identifier \"_x\" ())expression (object (integer \"2\" ())))))))";

// --- CompileScenarioTest ---

void CompileScenarioTest :: SetUp()
{
   CompileTest::SetUp();
}

void CompileScenarioTest :: runTest(ref_t targetRef, int exptectedError)
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true);
   moduleScope->buildins.superReference = 1;
   moduleScope->buildins.intReference = intReference;
   moduleScope->buildins.constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE);

   moduleScope->aliases.add("int", intReference);

   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   Compiler::Namespace nsScope(compiler, moduleScope, TestErrorProcessor::getInstance(), nullptr, nullptr);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   Compiler::Class classHelper(nsScope, targetRef, Visibility::Public);
   classHelper.load();
   Compiler::Method methodHelper(classHelper);

   SyntaxNode methodNode = findTargetNode(targetRef);
   int catchedError = 0;
   try
   {
      methodHelper.compile(writer, methodNode);
   }
   catch (TestException& ex) {
      catchedError = ex.code;
   }

   // Assess
   EXPECT_TRUE(exptectedError == catchedError);
}

SyntaxNode CompileScenarioTest :: findTargetNode(ref_t targetRef)
{
   return findClassNode(targetRef).findChild(SyntaxKey::Method);
}

SyntaxNode CompileScenarioTest :: findClassNode(ref_t targetRef)
{
   return SyntaxTree::gotoChild(declarationNode.firstChild(), SyntaxKey::Class, targetRef);
}

// --- CallPrivateConstructorDirectly ---

void AccessPrivateField :: SetUp()
{
   CompileScenarioTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_3, S_IntNumber, PrivateField_Scenario1);
}

TEST_F(AccessPrivateField, AccessPrivateFieldTest)
{
   runTest(3, 0);
}

TEST_F(AccessPrivateField, AccessParentPrivateFieldTest)
{
   runTest(4, 106);
}
