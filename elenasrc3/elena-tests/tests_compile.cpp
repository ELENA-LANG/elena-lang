#include "pch.h"
// ------------------------------------------------
#include "serializer.h"
#include "bcwriter.h"

#include "compile_tests.h"
#include "scenario_consts.h"

using namespace elena_lang;

// ==== Tests Scenarios ===

constexpr auto PrivateField_Scenario1 = "class (nameattr (identifier \"A\" ())field (attribute -2147467262 ()nameattr (identifier \"_x\" ()))method (nameattr (identifier \"setX\" ())code (expression (assign_operation (object (identifier \"_x\" ())expression (object (integer \"2\" ())))))))class (nameattr 62 (identifier \"B\" ())parent (type (identifier \"A\" ()))method (nameattr (identifier \"setParentX\" ())code (expression (assign_operation (object (identifier \"_x\" ())expression (object (integer \"2\" ())))))))";

constexpr auto CallingIndexedethodFromSealed_Scenario1 = "expression (assign_operation (object (type (identifier \"IntNumber\" ())identifier \"ret\" ())expression (message_operation (object (identifier \"Y\" ())message (identifier \"calc\" ())expression (object (integer \"3\" ())))))";

#ifdef _M_IX86

constexpr auto Build_CallingIndexedethodFromSealed_Scenario1 = "byrefmark -8 () local_address -8 () saving_stack 2() int_literal 2 (value 3 ())saving_stack 1 ()class_reference 5 ()saving_stack ()argument ()semi_direct_call_op 4355 (type 5 ()index_table_mode ())local_address -8 ()copying -4 (size 4 ())";

#elif _M_X64

constexpr auto Build_CallingIndexedethodFromSealed_Scenario1 = "byrefmark -24 () local_address -24 () saving_stack 2() int_literal 2 (value 3 ())saving_stack 1 ()class_reference 5 ()saving_stack ()argument ()semi_direct_call_op 4355 (type 5 ()index_table_mode ())local_address -24 ()copying -8 (size 4 ())";

#endif

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


// --- CallingIndexedethodFromSealed ---
/*
interface X
{
   int calc(int arg)
      = arg + 2;
}

singleton Y : X {}

{
   int ret := Y.calc(3); // Must be indexed call
}
*/
void CallingIndexedMethodFromSealed::SetUp()
{
   ExprTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_2, S_IntNumber, S_IntRefeference, IndexedClass_Scenario1);
   SyntaxTreeSerializer::load(CallingIndexedethodFromSealed_Scenario1, exprNode);

   BuildTreeSerializer::load(Build_CallingIndexedethodFromSealed_Scenario1, buildNode);
}

TEST_F(CallingIndexedMethodFromSealed, CallingIndexedMethodTest)
{
   runBuildTest(true, false);
}
