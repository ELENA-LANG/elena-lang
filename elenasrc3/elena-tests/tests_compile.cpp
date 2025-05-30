/*
* AccessPrivateFieldTest:
* ----------------------
* 
* AccessParentPrivateFieldTest:
* -----------------------------
* 
* CallingIndexedMethodTest:
* -------------------------
*
* interface X
* {
*    int calc(int arg)
*     = arg + 2;
* }
* 
* singleton Y : X {}
* {
*   int ret := Y.calc(3); // Must be indexed call
* }
* 
* DuplicateBoxingTest:
* --------------------
*   int x : = 2;
*   Helper.testArg2({ ^ x }, { ^ x });
* 
* GeneratingByRefHandler
* ----------------------
* check if ByRefHandler was properly generated
* 
* class A
* {
*   int incSum(int arg1) 
*      = arg1 + 1;
* }
* 
* GeneratingByRefHandlerInvoker
* -----------------------------
* check if ByRefHandlerInvoker was properly generated
*
* class A
* {
*   int incSum(int arg1)
*      = arg1 + 1;
* }
*
*
*/

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

constexpr auto DuplicateBoxing_Scenario1 = "expression (code (expression (assign_operation (object (type (identifier \"IntNumber\" ())identifier \"x\" ())expression (object (integer \"2\" ()))))expression (message_operation (object (identifier \"Tester\" ())message (identifier \"testArg2\" ())expression (closure (code (returning (expression (object (identifier \"x\" ()))))))expression (closure (code (returning (expression (object (identifier \"x\" ()))))))))))";

#ifdef _M_IX86

constexpr auto Build_CallingIndexedethodFromSealed_Scenario1 = "byrefmark -8 () local_address -8 () saving_stack 2() int_literal 2 (value 3 ())saving_stack 1 ()class_reference 5 ()saving_stack ()argument ()semi_direct_call_op 4355 (type 5 ()index_table_mode ())local_address -8 ()copying -4 (size 4 ())";
constexpr auto Build_DuplicateBoxing_Scenario1 = "int_literal 2 (value 2 ()) copying -4 (size 4 ())local_address -4 ()saving_stack ()create_struct 4 (type 2 ())copying_to_acc 1 (size 4 ())assigning 1 ()create_class 1 (type 6 ())assign_local_to_stack 1 ()set_imm_field ()assigning 2 ()create_class 1 (type 7 ())assign_local_to_stack 1 ()set_imm_field ()assigning 3 ()local 3 ()saving_stack 2 ()local 2 ()saving_stack 1 ()class_reference 3 ()saving_stack ()argument ()direct_call_op 2051 (type 3 ())";

constexpr auto Build_S_ByRefHandlerTest_Class = "class 4 (method 4867 (tape ( open_frame () assigning 1 () local -2 () saving_stack () int_literal 2 (value 1 ())saving_stack 1 ()intop -4 (operator_id 4 ())local_address -4 ()saving_stack ()local -3 ()copying_to_acc -3 (size 4 ()) going_to_eop ()close_frame ()exit ())reserved 3 ()reserved_n 4 ())method 3074 (tape (open_frame ()assigning 1 ()local_address -4 ()saving_stack 2 ()local -2 ()saving_stack 1 ()local 1 ()saving_stack ()argument ()semi_direct_call_op 4867 (type 4 ()index_table_mode ())local_address -4 ()saving_stack ()create_struct 4 (type 2 ())copying_to_acc 2 (size 4 ())assigning 2 ()local 2 ()close_frame ()exit ())reserved 5 ()reserved_n 4 ()))";

#elif _M_X64

constexpr auto Build_CallingIndexedethodFromSealed_Scenario1 = "byrefmark -24 () local_address -24 () saving_stack 2() int_literal 2 (value 3 ())saving_stack 1 ()class_reference 5 ()saving_stack ()argument ()semi_direct_call_op 4355 (type 5 ()index_table_mode ())local_address -24 ()copying -8 (size 4 ())";
constexpr auto Build_DuplicateBoxing_Scenario1 = "int_literal 2 (value 2 ()) copying -8 (size 4 ())local_address -8 ()saving_stack ()create_struct 4 (type 2 ())copying_to_acc 1 (size 4 ())assigning 1 ()create_class 1 (type 6 ())assign_local_to_stack 1 ()set_imm_field ()assigning 2 ()create_class 1 (type 7 ())assign_local_to_stack 1 ()set_imm_field ()assigning 3 ()local 3 ()saving_stack 2 ()local 2 ()saving_stack 1 ()class_reference 3 ()saving_stack ()argument ()direct_call_op 2051 (type 3 ())";

constexpr auto Build_S_ByRefHandlerTest_Class = "class 4 (method 4867 (tape ( open_frame () assigning 1 () local -2 () saving_stack () int_literal 2 (value 1 ())saving_stack 1 ()intop -8 (operator_id 4 ())local_address -8 ()saving_stack ()local -3 ()copying_to_acc -3 (size 4 ()) going_to_eop ()close_frame ()exit ())reserved 4 ()reserved_n 16 ())method 3074 (tape (open_frame ()assigning 1 ()local_address -8 ()saving_stack 2 ()local -2 ()saving_stack 1 ()local 1 ()saving_stack ()argument ()semi_direct_call_op 4867 (type 4 ()index_table_mode ())local_address -8 ()saving_stack ()create_struct 4 (type 2 ())copying_to_acc 2 (size 4 ())assigning 2 ()local 2 ()close_frame ()exit ())reserved 6 ()reserved_n 16 ()))";

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

// --- DuplicateBoxingTest ---

void DuplicateBoxingTest :: SetUp()
{
   ExprTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_2, S_IntNumber, Tester_2Args);
   SyntaxTreeSerializer::load(DuplicateBoxing_Scenario1, exprNode);

   BuildTreeSerializer::load(Build_DuplicateBoxing_Scenario1, buildNode);
}

TEST_F(DuplicateBoxingTest, DuplicateBoxingTest)
{
   runBuildTest(true, false, 1);
}

// --- GeneratingByRefHandler ---

void ByRefHandlerTest :: SetUp()
{
   checkTargetMessage = true;
   byRefTemplateRef = 0x80;

   intNumberRef = 2;
   intByRefRef = 3;

   targetRef = 4;

   MethodScenarioTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_2, S_IntNumber, S_IntRefeference, S_ByRefHandlerTest_Class);

   BuildTreeSerializer::load(Build_S_ByRefHandlerTest_Class, controlOutputNode);
}

BuildNode ByRefHandlerTest::getExpectedOutput(BuildNode node, int scenario)
{
   if (scenario <= 0)
      return {};

   BuildNode current = node != BuildKey::Method ? node.firstChild().firstChild() : node;
   while (current != BuildKey::None) {
      scenario--;
      if (!scenario)
         return current;

      current = current.nextNode();
   }

   return {};
}

SyntaxNode ByRefHandlerTest :: findTargetNode(int scenario)
{
   if (scenario <= 0)
      return {};
      
   SyntaxNode node = findClassNode();
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Method) {
         scenario--;
         if (!scenario)
            return current;
      }
      current = current.nextNode();
   }

   return {};
}

TEST_F(ByRefHandlerTest, GeneratingByRefHandler)
{
   runTest(true, false, 1, 1);
}

// --- GeneratingByRefHandlerInvoker ---

TEST_F(ByRefHandlerTest, GeneratingByRefHandlerInvoker)
{
   runTest(true, false, 1, 2);
}
