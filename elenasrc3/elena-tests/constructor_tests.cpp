#include "pch.h"
// ------------------------------------------------
#include "serializer.h"
#include "bcwriter.h"

#include "constructor_tests.h"
#include "scenario_consts.h"

using namespace elena_lang;

// ==== Tests Scenarios ===

constexpr auto S1_VariadicConstructorSingleDispatch_1 = "class (attribute -2147479546 ()nameattr (identifier \"Tester\" ())method (nameattr (identifier \"test\" ())parameter (attribute -2147475445 ()array_type (type (identifier \"Object\" ()))nameattr (identifier \"args\" ()))code (returning (expression (message_operation (object (identifier \"X\" ())message (identifier \"load\" ())expression (object (attribute -2147475445 ()identifier \"args\" ())))))))) class (nameattr (identifier \"X\" ()) method (attribute -2147479548 ()nameattr (identifier \"load\" ())parameter (attribute -2147475445 ()array_type (type (identifier \"B\" ()))nameattr (identifier \"args\" ()))code ()))";

constexpr auto S_PrivateConstructorTest = "class (nameattr (identifier \"X\" ())field (type (identifier \"int\" ())nameattr (identifier \"_value\" ()))method (attribute -2147467262 ()nameattr (identifier \"constructor\" ())code (EOP (eop \"}\" ())))method (attribute -2147479548 ()nameattr (identifier \"load\" ())parameter (type (identifier \"int\" ())nameattr (identifier \"v\" ()))code (expression (assign_operation (object (identifier \"_value\" ())expression (object (identifier \"v\" ()))))EOP ())))";

#ifdef _M_IX86

constexpr auto BuildTree_VariadicSingleDispatch_3 = "tape(open_frame()assigning 1 ()local_reference - 2 ()saving_stack()varg_sop 6 (index - 4 ())unbox_call_message - 2 (index 1 ()length - 4 ()temp_var - 8 ()message 1601 ())class_reference 6 ()saving_stack()argument()direct_call_op 5250 (type 12 ())loading_index() free_varstack() going_to_eop() close_frame()exit())reserved 3 ()reserved_n 8 ())";

#elif _M_X64

constexpr auto BuildTree_VariadicSingleDispatch_3 = "tape(open_frame()assigning 1 ()local_reference -2 ()saving_stack()varg_sop 6 (index -8 ())unbox_call_message -2 (index 1 ()length -8 ()temp_var -24 ()message 1601 ())class_reference 6 ()saving_stack()argument()direct_call_op 5250 (type 12 ())loading_index() free_varstack() going_to_eop() close_frame()exit())reserved 4 ()reserved_n 32 ())";

#endif

// --- VariadicCompiletimeConstructorSingleDispatch ---

void VariadicCompiletimeConstructorSingleDispatch :: SetUp()
{
   MethodScenarioTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_1, S1_VariadicTemplates, S1_VariadicConstructorSingleDispatch_1, S_IntNumber);

   BuildTreeSerializer::load(BuildTree_VariadicSingleDispatch_3, controlOutputNode);

   argArrayRef = 0x80;
   genericVargRef = 3;
   targetVargRef = 4;
   targetRef = 5;
   intNumberRef = 7;
}

// ==== Tests ===

TEST_F(VariadicCompiletimeConstructorSingleDispatch, ConstructorTest)
{
   runTest(true);
}

/*TEST_F(CallPrivateConstructorDirectly, ConstructorTest)
{
   runTest(true);
}*/
