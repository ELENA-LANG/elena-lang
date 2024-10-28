//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Optimization Fixture implementation
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "bt_optimization.h"
#include "serializer.h"
#include "bcwriter.h"

#include "scenario_consts.h"
#include "tape_consts.h"

using namespace elena_lang;

constexpr auto BT_RULES_FILE = "bt_rules60.dat";

// --- BTOptimization1 ---

constexpr auto Declaration1_1 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"Struct\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ()))) class ( attribute -2147471359 () nameattr (identifier \"TestReference\" ()) field (attribute -2147475454 () type (identifier \"Struct\" ()) nameattr (identifier \"_value\" ())) ) class (attribute -2147467263 ()attribute -2147479546 () nameattr (identifier \"Tester\" ()) method (type (identifier \"Struct\" ()) nameattr (identifier \"getValue\" ())code ())))";
constexpr auto SyntaxTree1_1 = "expression(assign_operation(object(type(identifier \"Struct\"())identifier \"r\"())expression(message_operation(object(identifier \"Tester\"())message(identifier \"getValue\" ())))))";

constexpr auto Declaration1_2 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"Struct\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ()))) class ( attribute -2147471359 () nameattr (identifier \"TestReference\" ()) field (attribute -2147475454 () type (identifier \"Struct\" ()) nameattr (identifier \"_value\" ())) ) class (attribute -2147467263 ()attribute -2147479546 () nameattr (identifier \"Tester\" ()) method (attribute -2147463167 () type (identifier \"Struct\" ()) nameattr (identifier \"Value\" ())code ())))";
constexpr auto SyntaxTree1_2 = "expression(assign_operation(object(type(identifier \"Struct\"())identifier \"r\"())expression(property_operation(object(identifier \"Tester\"())message(identifier \"Value\" ())))))";

constexpr auto S_Declaration1_4 = "class (attribute -2147467263 ()attribute -2147479546 () nameattr (identifier \"Tester\" ()) method (type (identifier \"IntNumber\" ()) nameattr (identifier \"at\" ()) parameter (type (identifier \"IntNumber\" ()) nameattr (identifier \"o\" ())) code ())";

constexpr auto SyntaxTree1_3 = "expression(assign_operation(object(type(identifier \"Struct\" ())identifier \"r\"())expression(value_operation(expression(object(identifier \"Tester\"()))))))";
constexpr auto SyntaxTree1_4 = "expression(assign_operation(object(type(identifier \"IntNumber\" ())identifier \"r\"())expression(index_operation(expression(object(identifier \"Tester\"())) expression(object(integer \"0\")))))))";

constexpr auto Declaration2 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"IntNumber\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ()))) class ( attribute -2147471359 () nameattr (identifier \"IntNumberReference\" ()) field (attribute -2147475454 () type (identifier \"IntNumber\" ()) nameattr (identifier \"_value\" ())) ))";
constexpr auto SyntaxTree2 = "expression (assign_operation (object (type (identifier \"IntNumber\" ()) identifier \"n\" ()) expression (object (integer \"2\"))))";
constexpr auto SyntaxTree4 = "expression ( code( expression (assign_operation (object (type (identifier \"IntNumber\" ())identifier \"n\" ())expression (object (integer \"3\" ()))))expression (assign_operation (object (type (identifier \"IntNumber\" ())identifier \"r\" ())expression (add_operation (object (identifier \"n\" ())expression (object (integer \"2\" ()))))))))";

constexpr auto Struct_Declaration1 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147479550 () nameattr (identifier \"IntNumber\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr ( identifier \"_value\" ()) dimension ( integer \"4\" ()))) class (attribute -2147479550 () nameattr (identifier \"ByteNumber\" ()) field (attribute -2147475454 () attribute -2147481596 () nameattr (identifier \"_value\" ()) dimension (integer \"1\" ())))class (attribute -2147479550 () nameattr 60 (identifier \"ShortNumber\" ()) field (attribute -2147475454 () attribute -2147481597 ()nameattr ( identifier \"_value\" ()) dimension ( integer \"2\" ()))) class (attribute -2147479550 ()nameattr (identifier \"LongNumber\" ())field (attribute -2147475454 () attribute -2147481597 () nameattr ( identifier \"_value\" ()) dimension (integer \"8\" ()))) class (attribute -2147479550 () nameattr (identifier \"Aligned\" ())field (type (identifier \"byte\" ())nameattr (identifier \"b\" ()) ) field (type (identifier \"short\" ())nameattr (identifier \"w\" ())) field (type (identifier \"byte\" ())nameattr (identifier \"b2\" ())) field (type (identifier \"int\" ())nameattr (identifier \"n\" ())) field (type (identifier \"byte\" ())nameattr (identifier \"b3\" ()))field (type (identifier \"long\" ()) nameattr (identifier \"l\" ()))))";
constexpr auto Struct_Declaration2 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147479550 () nameattr (identifier \"IntNumber\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr ( identifier \"_value\" ()) dimension ( integer \"4\" ()))) class (attribute -2147479550 () nameattr (identifier \"ByteNumber\" ()) field (attribute -2147475454 () attribute -2147481596 () nameattr (identifier \"_value\" ()) dimension (integer \"1\" ())))class (attribute -2147479550 () nameattr 60 (identifier \"ShortNumber\" ()) field (attribute -2147475454 () attribute -2147481597 ()nameattr ( identifier \"_value\" ()) dimension ( integer \"2\" ()))) class (attribute -2147479550 ()nameattr (identifier \"LongNumber\" ())field (attribute -2147475454 () attribute -2147481597 () nameattr ( identifier \"_value\" ()) dimension (integer \"8\" ()))) class (attribute -2147479508 () nameattr (identifier \"Aligned\" ())field (type (identifier \"byte\" ())nameattr (identifier \"b\" ()) ) field (type (identifier \"short\" ())nameattr (identifier \"w\" ())) field (type (identifier \"byte\" ())nameattr (identifier \"b2\" ())) field (type (identifier \"int\" ())nameattr (identifier \"n\" ())) field (type (identifier \"byte\" ())nameattr (identifier \"b3\" ()))field (type (identifier \"long\" ()) nameattr (identifier \"l\" ()))))";
constexpr auto Struct_Declaration3 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147479550 () nameattr (identifier \"IntNumber\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr ( identifier \"_value\" ()) dimension ( integer \"4\" ()))) class (attribute -2147479550 () nameattr (identifier \"ByteNumber\" ()) field (attribute -2147475454 () attribute -2147481596 () nameattr (identifier \"_value\" ()) dimension (integer \"1\" ())))class (attribute -2147479550 () nameattr 60 (identifier \"ShortNumber\" ()) field (attribute -2147475454 () attribute -2147481597 ()nameattr ( identifier \"_value\" ()) dimension ( integer \"2\" ()))) class (attribute -2147479550 ()nameattr (identifier \"LongNumber\" ())field (attribute -2147475454 () attribute -2147481597 () nameattr ( identifier \"_value\" ()) dimension (integer \"8\" ()))) class (attribute -2147479550 () nameattr (identifier \"Aligned\" ())field (type (identifier \"byte\" ())nameattr (identifier \"b\" ()) ) field (type (identifier \"short\" ())nameattr (identifier \"w\" ())) field (type (identifier \"byte\" ())nameattr (identifier \"b2\" ())) field (type (identifier \"int\" ())nameattr (identifier \"n\" ()))) class (attribute -2147479550 () nameattr (identifier \"Complex\" ())field (type (identifier \"byte\" ())nameattr (identifier \"f1\" ()) ) field (type (identifier \"Aligned\" ())nameattr (identifier \"f2\" ()))))";

constexpr auto S1_VariadicSingleDispatch_1 = "class (attribute -2147479546 () nameattr (identifier \"E\" ()) method (nameattr (identifier \"load\" ()) parameter (attribute -2147475445 () array_type (type (identifier \"B\" ())) nameattr (identifier \"o\" ())) code ()))";
constexpr auto S1_VariadicSingleDispatch_2 = "class (attribute -2147467263 () nameattr (identifier \"program\" ()) attribute -2147479546 () method 2592 (attribute -2147479540 ()code ( expression (assign_operation (object (attribute -2147479539 () identifier \"b\" ())expression (message_operation (object (attribute -2147479534 () identifier \"B\" ()))))) expression (assign_operation (object (attribute -2147479539 () identifier \"c\" ()) expression (message_operation (object (attribute -2147479534 ()identifier \"C\" ()))))) expression (message_operation (object (identifier \"E\" ())message (identifier \"load\" ())expression (object (identifier \"b\" ()))expression (object (identifier \"c\" ()))))))) class (nameattr (identifier \"C\" ()) method (type (identifier \"B\" ())attribute -2147479535 () returning (expression (message_operation (object (attribute -2147479534 () identifier \"B\" ())))))";

constexpr auto S1_VariadicSingleDispatch_3a = "class (nameattr(identifier \"C\" ())parent (type (identifier \"B\" ())))class (nameattr (identifier \"D\" ())parent (type (identifier \"B\" ())))";
constexpr auto S1_VariadicSingleDispatch_3b = "class (attribute -2147467263 ()nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (expression (message_operation (object (identifier \"E\" ())message (identifier \"load\" ())expression (message_operation (object (attribute -2147479534 ()identifier \"C\" ())))expression (message_operation (object (attribute -2147479534 ()identifier \"D\" ()))))))))";

constexpr auto S1_DirectCall_1 = "class (nameattr (identifier \"myMethod\" ())attribute -2147479546 ()method (attribute -2147479540 ()parameter (type (identifier \"B\" ())nameattr (identifier \"arg\" ()))code ()))";
constexpr auto S1_DirectCall_2 = "class (attribute -2147479546 ()nameattr (identifier \"TestHelper\" ())method (nameattr (identifier \"myMethodInvoker\" ())code (expression (message_operation (object (identifier \"myMethod\" ())expression (message_operation (object (attribute -2147479534 ()identifier \"B\" ())))))))method (nameattr (identifier \"myMethod\" ())parameter (type (identifier \"B\" ())nameattr (identifier \"arg\" ()))code ()))";

constexpr auto S1_DirectCallWithNil_1 = "class (attribute -2147479546 ()nameattr (identifier \"X\" ())method (nameattr (identifier \"printMe\" ())parameter (type (nullable (identifier \"B\" ()))nameattr (identifier \"arg\" ()))code ()))";
constexpr auto S1_DirectCallWithNil_2 = "class (attribute -2147467263 ()nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (expression (message_operation (object (identifier \"X\" ())message (identifier \"printMe\" ())expression (object (identifier \"nil\" ())))))))";

constexpr auto S1_DirectCall_3 = "class (attribute -2147479546 ()nameattr (identifier \"TestHelper\" ())method (nameattr (identifier \"myMethodInvoker\" ())code (expression (assign_operation (object (attribute -2147479539 ()identifier \"b1\" ())expression (message_operation (object (attribute -2147479534 ()identifier \"B\" ())))))expression (assign_operation (object (attribute -2147479539 ()identifier \"b2\" ())expression (message_operation (object (attribute -2147479534 ()identifier \"B\" ())))))expression (message_operation (object (identifier \"myMethod\" ())expression (object (identifier \"b1\" ()))expression (object (identifier \"b2\" ()))))))method (nameattr (identifier \"myMethod\" ())parameter (attribute -2147475445 ()array_type (type (identifier \"B\" ()))nameattr (identifier \"arg\" ()))code ()))";

constexpr auto S1_MethodWithSignatureOfObject = "class (attribute -2147479546 ()nameattr (identifier \"Helper\" ())method (nameattr (identifier \"test\" ())parameter (type (identifier \"Object\" ())nameattr (identifier \"arg\" ()))code ())) class (attribute -2147467263 ()nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (expression (assign_operation (object (type (identifier \"Object\" ())identifier \"arg\" ())expression (object (integer \"2\" ()))))expression (message_operation (object (identifier \"Helper\" ()) message(identifier \"test\" ())expression (object (identifier \"arg\" ())))))))";

// S2 Scenarion : lmbda

constexpr auto S2_Func = "class (attribute -2147467263 ()attribute -2147479545 ()nameattr (identifier \"Func\" ())method (attribute -2147471358 ()nameattr (identifier \"function\" ())no_body ()))";
constexpr auto S2_Scenario1 = "class (attribute -2147479546 ()nameattr (identifier \"E\" ())method (nameattr (identifier \"callFromLambda\" ())code (expression (assign_operation (object (attribute -2147479539 ()identifier \"f\" ())expression (closure (code (expression (message_operation (object (identifier \"callMe\" ()))))))))expression (message_operation (object (identifier \"f\" ())))))method (attribute -2147467262 ()nameattr (identifier \"callMe\" ())code ()))";

#ifdef _M_IX86

constexpr auto BuildTree1_1 = "byrefmark -8 () local_address -8 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2306 (type 4 ()) local_address -8 () copying -4 (size 4 ())";
constexpr auto BuildTree1_2 = "byrefmark -8 () local_address -8 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2434 (type 4 ()) local_address -8 () copying -4 (size 4 ())";
constexpr auto BuildTree1_4 = "byrefmark -8 () local_address -8 () saving_stack 2 () int_literal 2 (value 0 ()) saving_stack 1() class_reference 4 () saving_stack () argument () direct_call_op 3331 (type 4 ()) local_address -8 () copying -4 (size 4 ())";
constexpr auto BuildTree2 = "int_literal 2 (value 2 ()) copying -4 ( size 4 ())";
constexpr auto BuildTree4 = "int_literal 2 (value 3 ())copying -4 (size 4 ())local_address -4 ()saving_stack ()int_literal 3 (value 2 ())saving_stack 1 ()intop 4 (index -12 ())local_address -12 ()copying -8 (size 4 ())";

constexpr auto OptimizedBuildTree1_1 = "local_address -4 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2306 (type 4 ()) local_address -4 ()";
constexpr auto OptimizedBuildTree1_2 = "local_address -4 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2434 (type 4 ()) local_address -4 ()";
constexpr auto OptimizedBuildTree1_4 = "local_address -4 () saving_stack 2 () int_literal 2 (value 0 ()) saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 3331 (type 4 ()) local_address -4 ()";
constexpr auto OptimizedBuildTree2 = "saving_int - 4 (size 4 ()value 2 ())";
constexpr auto OptimizedBuildTree4 = "saving_int -4 (size 4 ()value 3 ())local_address -4 ()copying -8 (size 4 ())addingint -8 (value 2 ())";

constexpr auto BuildTree_VariadicSingleDispatch_1 = "tape(sealed_dispatching 11 (message 3138 ()) open_frame() assigning 1 () local_reference -2 () saving_stack() varg_sop 6 (index -4 ()) unbox_call_message -2 (index 1 () length -4 () temp_var -8 () message 1217 ()) local 1 () saving_stack() argument() direct_call_op 2626 (type 5 ()) loading_index() free_varstack() close_frame() exit()) reserved 3 ()reserved_n 8 ())";
constexpr auto BuildTree_VariadicSingleDispatch_2 = "tape(open_frame() assigning 1 () class_reference 2 () direct_call_op 544 (type 10 ()) assigning 2 () class_reference 8 () direct_call_op 544 (type 15 ()) assigning 3 () local 2 () saving_stack() argument() call_op 1217 () assigning 4 () local 3 () saving_stack() argument() call_op 1217 () assigning 5 () terminator() saving_stack 3 () local 5 () saving_stack 2 () local 4 () saving_stack 1 () class_reference 5 () saving_stack() argument() direct_call_op 2626 (type 5 ()) local 1 () close_frame() exit()) reserved 9 ()";

constexpr auto BuildTree_VariadicSingleDispatch_4 = "tape (open_frame ()assigning 1 ()class_reference 8 ()direct_call_op 544 (type 16 ())assigning 2 ()class_reference 9 ()direct_call_op 544 (type 17 ())assigning 3 ()terminator ()saving_stack 3 ()local 3 ()saving_stack 2 ()local 2 ()saving_stack 1 ()class_reference 7 ()saving_stack ()argument ()direct_call_op 3650 (type 7 ())local 1 ()close_frame ()exit ())reserved 7 ())";

constexpr auto BuildTree_CallMethodWithoutTarget = "tape (open_frame ()assigning 1 ()class_reference 2 ()direct_call_op 544 (type 6 ())assigning 2 ()local 2 ()saving_stack 1 ()local 1 ()saving_stack ()argument ()direct_call_op 3074 (type 4 ())local 1 ()close_frame ()exit ())reserved 4 ()";
constexpr auto BuildTree_CallVariadicMethodWithoutTarget = "tape(open_frame()assigning 1 ()class_reference 2 ()direct_call_op 544 (type 8 ())assigning 2 ()class_reference 2 ()direct_call_op 544 (type 8 ())assigning 3 ()local 2 ()saving_stack()argument()call_op 1217 ()assigning 4 ()local 3 ()saving_stack()argument()call_op 1217 ()assigning 5 ()terminator()saving_stack 3 ()local 5 ()saving_stack 2 ()local 4 ()saving_stack 1 ()local 1 ()saving_stack()argument()direct_call_op 3138 (type 4 ())local 1 ()close_frame()exit())reserved 9 ()";
constexpr auto BuildTree_CallMethodWithNil = "tape (open_frame ()assigning 1 ()nil_reference ()saving_stack 1 ()class_reference 3 ()saving_stack ()argument ()direct_call_op 1538 (type 3 ())local 1 ()close_frame ()exit ())reserved 3 ()";

constexpr auto BuildTree_LambdaCallPrivate = "tape (open_frame ()assigning 1 ()local 1 ()field ()saving_stack ()argument ()direct_call_op 3329 (type 3 ())close_frame ()exit ())reserved 2 ()";

constexpr auto BuildTree_CallMethodWithSignatureOfObject = "tape (open_frame ()assigning 1 ()int_literal 2 (value 2 ())assigning 2 ()local 2 ()saving_stack 1 ()class_reference 3 ()saving_stack ()argument ()direct_call_op 1538 (type 3 ())local 1 ()close_frame ()exit ())reserved 4 ())";

constexpr auto PackedStructSize = 20;

constexpr auto ComplexStructOffset2 = 4;
constexpr auto ComplexStructSize = 16;

#elif _M_X64

constexpr auto BuildTree1_1 = "byrefmark -24 () local_address -24 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2306 (type 4 ()) local_address -24 () copying -8 (size 4 ())";
constexpr auto BuildTree1_2 = "byrefmark -24 () local_address -24 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2434 (type 4 ()) local_address -24 () copying -8 (size 4 ())";
constexpr auto BuildTree2 = "int_literal 2 (value 2 ()) copying -8 ( size 4 ())";
constexpr auto BuildTree4 = "int_literal 2 (value 3 ())copying -8 (size 4 ())local_address -8 ()saving_stack ()int_literal 3 (value 2 ())saving_stack 1 ()intop 4 (index -40 ())local_address -40 ()copying -24 (size 4 ())";

constexpr auto OptimizedBuildTree1_1 = "local_address -8 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2306 (type 4 ()) local_address -8 ()";
constexpr auto OptimizedBuildTree1_2 = "local_address -8 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2434 (type 4 ()) local_address -8 ()";
constexpr auto OptimizedBuildTree2 = "saving_int - 8 (size 4 ()value 2 ())";
constexpr auto OptimizedBuildTree4 = "saving_int -8 (size 4 ()value 3 ())local_address -8 ()copying -24 (size 4 ())addingint -24 (value 2 ())";

constexpr auto BuildTree_VariadicSingleDispatch_1 = "tape(sealed_dispatching 11 (message 3138 ()) open_frame() assigning 1 () local_reference -2 () saving_stack() varg_sop 6 (index -8 ()) unbox_call_message -2 (index 1 () length -8 () temp_var -24 () message 1217 ()) local 1 () saving_stack() argument() direct_call_op 2626 (type 5 ()) loading_index() free_varstack() close_frame() exit()) reserved 4 ()reserved_n 32 ())";
constexpr auto BuildTree_VariadicSingleDispatch_2 = "tape(open_frame() assigning 1 () class_reference 2 () direct_call_op 544 (type 10 ()) assigning 2 () class_reference 8 () direct_call_op 544 (type 15 ()) assigning 3 () local 2 () saving_stack() argument() call_op 1217 () assigning 4 () local 3 () saving_stack() argument() call_op 1217 () assigning 5 () terminator() saving_stack 3 () local 5 () saving_stack 2 () local 4 () saving_stack 1 () class_reference 5 () saving_stack() argument() direct_call_op 2626 (type 5 ()) local 1 () close_frame() exit()) reserved 10 ()";

constexpr auto BuildTree_VariadicSingleDispatch_4 = "tape (open_frame ()assigning 1 ()class_reference 8 ()direct_call_op 544 (type 16 ())assigning 2 ()class_reference 9 ()direct_call_op 544 (type 17 ())assigning 3 ()terminator ()saving_stack 3 ()local 3 ()saving_stack 2 ()local 2 ()saving_stack 1 ()class_reference 7 ()saving_stack ()argument ()direct_call_op 3650 (type 7 ())local 1 ()close_frame ()exit ())reserved 8 ())";

constexpr auto BuildTree_CallMethodWithoutTarget = "tape (open_frame ()assigning 1 ()class_reference 2 ()direct_call_op 544 (type 6 ())assigning 2 ()local 2 ()saving_stack 1 ()local 1 ()saving_stack ()argument ()direct_call_op 3074 (type 4 ())local 1 ()close_frame ()exit ())reserved 4 ()";
constexpr auto BuildTree_CallVariadicMethodWithoutTarget = "tape(open_frame()assigning 1 ()class_reference 2 ()direct_call_op 544 (type 8 ())assigning 2 ()class_reference 2 ()direct_call_op 544 (type 8 ())assigning 3 ()local 2 ()saving_stack()argument()call_op 1217 ()assigning 4 ()local 3 ()saving_stack()argument()call_op 1217 ()assigning 5 ()terminator()saving_stack 3 ()local 5 ()saving_stack 2 ()local 4 ()saving_stack 1 ()local 1 ()saving_stack()argument()direct_call_op 3138 (type 4 ())local 1 ()close_frame()exit())reserved 10 ()";
constexpr auto BuildTree_CallMethodWithNil = "tape (open_frame ()assigning 1 ()nil_reference ()saving_stack 1 ()class_reference 3 ()saving_stack ()argument ()direct_call_op 1538 (type 3 ())local 1 ()close_frame ()exit ())reserved 4 ()";

constexpr auto BuildTree_LambdaCallPrivate = "tape (open_frame ()assigning 1 ()local 1 ()field ()saving_stack ()argument ()direct_call_op 3329 (type 3 ())close_frame ()exit ())reserved 4 ()";

constexpr auto BuildTree_CallMethodWithSignatureOfObject = "tape (open_frame ()assigning 1 ()int_literal 2 (value 2 ())assigning 2 ()local 2 ()saving_stack 1 ()class_reference 3 ()saving_stack ()argument ()direct_call_op 1538 (type 3 ())local 1 ()close_frame ()exit ())reserved 4 ())";

constexpr auto PackedStructSize = 24;

constexpr auto ComplexStructOffset2 = 8;
constexpr auto ComplexStructSize = 24;

#endif

// --- BTOptimization ---

void BTOptimization :: SetUp()
{
   PathString appPath;
   getAppPath(appPath);

   PathString btRulesPath(*appPath, BT_RULES_FILE);
   FileReader btRuleReader(*btRulesPath, FileRBMode, FileEncoding::Raw, false);
   if (btRuleReader.isOpen()) {
      btRules.load(btRuleReader, btRuleReader.length());
   }

   ExprTest::SetUp();

   afterOptimization = buildTree.readRoot().appendChild(BuildKey::Tape);
}

void BTOptimization :: runBTTest()
{
   // Arrange
   ByteCodeWriter::BuildTreeOptimizer buildTreeOptimizer;
   MemoryReader reader(&btRules);
   buildTreeOptimizer.load(reader);

   BuildTree output;
   BuildTreeWriter writer(output);
   BuildTree::copyNode(writer, buildNode, true);

   // Act
   buildTreeOptimizer.proceed(output.readRoot());

   // Assess
   bool matched = BuildTree::compare(output.readRoot(), afterOptimization, true);
   EXPECT_TRUE(matched);
}

// --- StructTest ---

void StructTest :: SetUp()
{
   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);
}

bool StructTest :: validateStructInfo(ClassInfo& structInfo)
{
   int index = 1;
   for (auto it = structInfo.fields.start(); !it.eof(); ++it) {
      auto field = *it;
      
      int expectedOffset = -1;
      if (offsets.count_int() >= index)
         expectedOffset = offsets.get(index);

      if (field.offset != expectedOffset)
         return false;

      index++;
   }

   return structInfo.size == expectedSize;
}

void StructTest :: runTest()
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true);
   moduleScope->buildins.superReference = 1;
   moduleScope->buildins.intReference = 2;
   moduleScope->buildins.uint8Reference = 3;
   moduleScope->buildins.shortReference = 4;
   moduleScope->buildins.longReference = 5;

   moduleScope->aliases.add("int", 2);
   moduleScope->aliases.add("byte", 3);
   moduleScope->aliases.add("short", 4);
   moduleScope->aliases.add("long", 5);

   Compiler* compiler = env.createCompiler();

   Compiler::Namespace nsScope(compiler, moduleScope, nullptr, nullptr, nullptr);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   // Assess
   ClassInfo structInfo = {};
   moduleScope->loadClassInfo(structInfo, targetRef);
   bool valid = validateStructInfo(structInfo);
   EXPECT_TRUE(valid);

   freeobj(compiler);
   freeobj(moduleScope);
}

// --- BTOptimization1_1 ---

void BTOptimization1_1 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration1_1, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree1_1, exprNode);

   BuildTreeSerializer::load(BuildTree1_1, buildNode);
   BuildTreeSerializer::load(OptimizedBuildTree1_1, afterOptimization);
}

// --- BTOptimization1_2 ---

void BTOptimization1_2 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration1_2, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree1_2, exprNode);

   BuildTreeSerializer::load(BuildTree1_2, buildNode);
   BuildTreeSerializer::load(OptimizedBuildTree1_2, afterOptimization);
}

// --- BTOptimization1_3 ---

void BTOptimization1_3 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration1_2, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree1_3, exprNode);

   BuildTreeSerializer::load(BuildTree1_2, buildNode);
   BuildTreeSerializer::load(OptimizedBuildTree1_2, afterOptimization);
}

// --- BTOptimization1_4 ---

void BTOptimization1_4 :: SetUp()
{
   BTOptimization::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_2, S_IntNumber, S_IntRefeference, S_Declaration1_4);
   SyntaxTreeSerializer::load(SyntaxTree1_4, exprNode);

   BuildTreeSerializer::load(BuildTree1_4, buildNode);
   BuildTreeSerializer::load(OptimizedBuildTree1_4, afterOptimization);
}

// --- BTOptimization2 ---

void BTOptimization2 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration2, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree2, exprNode);

   BuildTreeSerializer::load(BuildTree2, buildNode);
   BuildTreeSerializer::load(OptimizedBuildTree2, afterOptimization);
}

// --- BTOptimization4 ---

void BTOptimization4 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration2, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree4, exprNode);

   BuildTreeSerializer::load(BuildTree4, buildNode);
   BuildTreeSerializer::load(OptimizedBuildTree4, afterOptimization);
}

// --- StructAlignment ---

void StructAlignment :: SetUp()
{
   StructTest::SetUp();

   offsets.add(0);
   offsets.add(2);
   offsets.add(4);
   offsets.add(8);
   offsets.add(12);
   offsets.add(16);

   SyntaxTreeSerializer::load(Struct_Declaration1, declarationNode);

   targetRef = 6;

   expectedSize = 24;
}

// --- StructAlignment ---

void PackedStructAlignment::SetUp()
{
   StructTest::SetUp();

   offsets.add(0);
   offsets.add(1);
   offsets.add(3);
   offsets.add(4);
   offsets.add(8);
   offsets.add(9);

   SyntaxTreeSerializer::load(Struct_Declaration2, declarationNode);

   targetRef = 6;

   expectedSize = PackedStructSize;
}

// --- ComplexStructAlignment ---

void ComplexStructAlignment ::SetUp()
{
   StructTest::SetUp();

   offsets.add(0);
   offsets.add(ComplexStructOffset2);

   SyntaxTreeSerializer::load(Struct_Declaration3, declarationNode);

   targetRef = 7;

   expectedSize = ComplexStructSize;
}

// --- ComplexStructAlignment ---

void PrimitiveStructAlignment:: SetUp()
{
   StructTest::SetUp();

   offsets.add(0);

   SyntaxTreeSerializer::load(Struct_Declaration1, declarationNode);

   targetRef = 4;

   expectedSize = 2;
}

// --- VariadicRuntimeSingleDispatch ---

SyntaxNode VariadicRuntimeSingleDispatch :: findTargetNode()
{
   return findAutoGenerated(findClassNode());
}

void VariadicRuntimeSingleDispatch :: SetUp()
{
   MethodScenarioTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_1, S1_VariadicTemplates, S1_VariadicSingleDispatch_1, S_IntNumber);

   BuildTreeSerializer::load(BuildTree_VariadicSingleDispatch_1, controlOutputNode);

   argArrayRef = 0x80;
   genericVargRef = 3;
   targetVargRef = 4;
   targetRef = 5;
   intNumberRef = 6;
}

// --- VariadicCompiletimeSingleDispatch ---

void VariadicCompiletimeSingleDispatch :: SetUp()
{
   MethodScenarioTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_1, S1_VariadicTemplates, S1_VariadicSingleDispatch_1, S_IntNumber, S1_VariadicSingleDispatch_2);

   BuildTreeSerializer::load(BuildTree_VariadicSingleDispatch_2, controlOutputNode);

   argArrayRef = 0x80;
   genericVargRef = 3;
   targetVargRef = 4;
   targetRef = 7;
   intNumberRef = 6;
}

// --- VariadicCompiletimeSingleDispatch_WithDifferentArgs ---

void VariadicCompiletimeSingleDispatch_WithDifferentArgs :: SetUp()
{
   MethodScenarioTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_1, S1_VariadicTemplates, S1_VariadicSingleDispatch_3b, S_IntNumber, S1_VariadicSingleDispatch_1, S1_VariadicSingleDispatch_3a);

   BuildTreeSerializer::load(BuildTree_VariadicSingleDispatch_4, controlOutputNode);

   argArrayRef = 0x80;
   genericVargRef = 3;
   targetVargRef = 4;
   targetRef = 5;
   intNumberRef = 6;
}

// --- MethodCallTest ---

void MethodCallTest :: SetUp()
{
   ScenarioTest::SetUp();
}

void MethodCallTest :: runTest(bool withVariadic)
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true);
   moduleScope->buildins.superReference = 1;
   moduleScope->buildins.constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE);

   if (withVariadic) {
      moduleScope->buildins.argArrayTemplateReference = argArrayRef;

      env.setUpTemplateMockup(argArrayRef, 1, genericVargRef);
      env.setUpTemplateMockup(argArrayRef, 2, targetVargRef);
   }

   moduleScope->predefined.add("nil", V_NIL);

   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   Compiler::Namespace nsScope(compiler, moduleScope, TestErrorProcessor::getInstance(), nullptr, nullptr);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   Compiler::Class classHelper(nsScope, targetRef, Visibility::Public);
   classHelper.load();
   Compiler::Method methodHelper(classHelper);

   SyntaxNode methodNode = findTargetNode();
   if (methodNode != SyntaxKey::None)
      methodHelper.compile(writer, methodNode);

   // Assess
   bool matched = BuildTree::compare(output.readRoot(), controlOutputNode, true);
   EXPECT_TRUE(matched);

   freeobj(compiler);
   freeobj(moduleScope);
}

// --- CallMethodWithoutTarget ---

void CallMethodWithoutTarget :: SetUp()
{
   MethodCallTest::SetUp();

   targetRef = 4;

   LoadDeclarationScenario(S_DefaultNamespace_1, S1_DirectCall_1, S1_DirectCall_2);

   BuildTreeSerializer::load(BuildTree_CallMethodWithoutTarget, controlOutputNode);
}

// --- CallVariadocMethodWithoutTarget ---

void CallVariadocMethodWithoutTarget :: SetUp()
{
   MethodCallTest::SetUp();

   argArrayRef = 0x80;
   targetRef = 4;
   genericVargRef = 5;
   targetVargRef = 6;

   LoadDeclarationScenario(S_DefaultNamespace_1, S1_DirectCall_1, S1_DirectCall_3, S1_VariadicTemplates);

   BuildTreeSerializer::load(BuildTree_CallVariadicMethodWithoutTarget, controlOutputNode);
}

// --- CallMethodWithSignatureOfSuperClass ---

void CallMethodWithSignatureOfSuperClass :: SetUp()
{
   MethodScenarioTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_3, S_IntNumber, S1_MethodWithSignatureOfObject);

   BuildTreeSerializer::load(BuildTree_CallMethodWithSignatureOfObject, controlOutputNode);

   targetRef = 4;
   intNumberRef = 2;
}

// --- CallMethodWithNil ---

void CallMethodWithNil :: SetUp()
{
   MethodCallTest::SetUp();

   targetRef = 4;

   LoadDeclarationScenario(S_DefaultNamespace_1, S1_DirectCallWithNil_1, S1_DirectCallWithNil_2);

   BuildTreeSerializer::load(BuildTree_CallMethodWithNil, controlOutputNode);
}

// --- LambdaTest ---

BuildNode LambdaTest :: findOutput(BuildNode root)
{
   return BuildTree::gotoChild(root, BuildKey::Class, outputRef).findChild(BuildKey::Method);
}

void LambdaTest :: SetUp()
{
   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   BuildTreeWriter buildWriter(buildTree);
   buildWriter.appendNode(BuildKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);

   controlOutputNode = buildTree.readRoot().appendChild(BuildKey::Idle);
}

void LambdaTest :: runTest()
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true, true);
   moduleScope->buildins.superReference = 1;
   moduleScope->buildins.dispatch_message = encodeMessage(
      moduleScope->module->mapAction(DISPATCH_MESSAGE, 0, false), 1, 0);
   moduleScope->buildins.constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE);

   moduleScope->buildins.closureTemplateReference = funcRef;

   //env.setUpTemplateMockup(argArrayRef, 1, genericVargRef);

   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   Compiler::Namespace nsScope(compiler, moduleScope, TestErrorProcessor::getInstance(), nullptr, nullptr);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   Compiler::Class classHelper(nsScope, targetRef, Visibility::Public);
   classHelper.load();
   Compiler::Method methodHelper(classHelper);

   SyntaxNode methodNode = findTargetNode();
   if (methodNode != SyntaxKey::None) {
      writer.newNode(BuildKey::Root);
      writer.newNode(BuildKey::Class);
      methodHelper.compile(writer, methodNode);
      writer.closeNode();
      writer.closeNode();
   }

   // Assess
   bool matched = BuildTree::compare(findOutput(output.readRoot()), controlOutputNode, true);
   EXPECT_TRUE(matched);

   freeobj(compiler);
   freeobj(moduleScope);
}

// --- Lambda_CallingPrivateMethod ---

void Lambda_CallingPrivateMethod :: SetUp()
{
   LambdaTest::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_2, S2_Func, S2_Scenario1);

   BuildTreeSerializer::load(BuildTree_LambdaCallPrivate, controlOutputNode);

   funcRef = 2;
   targetRef = 3;
   outputRef = 6;
}

// --- IntOperation ---

void IntOperation :: SetUp()
{
   ScenarioTest::SetUp();
}

void IntOperation :: runTest(bool exceptionExpected)
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true);
   moduleScope->buildins.superReference = 1;
   moduleScope->buildins.intReference = intReference;
   moduleScope->buildins.constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE);

   moduleScope->aliases.add("int", intReference);

   moduleScope->predefined.add("nil", V_NIL);

   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   Compiler::Namespace nsScope(compiler, moduleScope, TestErrorProcessor::getInstance(), nullptr, nullptr);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   Compiler::Class classHelper(nsScope, targetRef, Visibility::Public);
   classHelper.load();
   Compiler::Method methodHelper(classHelper);

   SyntaxNode methodNode = findTargetNode();
   int catchedError = 0;
   if (methodNode != SyntaxKey::None) {
      try
      {
         methodHelper.compile(writer, methodNode);
      }
      catch (TestException& ex) {
         catchedError = ex.code;
      }
   }      

   // Assess
   bool matched = false;
   if (!exceptionExpected) {
      matched = BuildTree::compare(output.readRoot(), controlOutputNode, true);
   }
   else matched = catchedError == expectedError;
   
   EXPECT_TRUE(matched);

   freeobj(compiler);
   freeobj(moduleScope);
}

// --- NillableIntAssigning ---

void NillableIntAssigning :: SetUp()
{
   IntOperation::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_3, S_IntNumber, S_NillableIntAssigning);

   BuildTreeSerializer::load(B_NillableIntAssigning, controlOutputNode);

   intReference = 2;
   targetRef = 3;
}

// --- IntAssigningNil ---

void IntAssigningNil :: SetUp()
{
   IntOperation::SetUp();

   LoadDeclarationScenario(S_DefaultNamespace_3, S_IntNumber, S_IntAssigningNil);

   intReference = 2;
   targetRef = 3;
   expectedError = 0x6b;
}
