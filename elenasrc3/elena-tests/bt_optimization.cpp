//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Optimization Fixture implementation
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "bt_optimization.h"
#include "serializer.h"

using namespace elena_lang;

// --- BTOptimization1 ---

constexpr auto Declaration1_1 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"Struct\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ()))) class ( nameattr (identifier \"TestReference\" ()) field (type (identifier \"Struct\" ()) nameattr (identifier \"_value\" ())) ) class (attribute -2147467263 ()attribute -2147479546 () nameattr (identifier \"Tester\" ()) method (type (identifier \"Struct\" ()) nameattr (identifier \"getValue\" ())code ())))";
constexpr auto SyntaxTree1_1 = "expression(assign_operation(object(type(identifier \"Struct\"())identifier \"r\"())expression(message_operation(object(identifier \"Tester\"())message(identifier \"getValue\" ())))))";
constexpr auto BuildTree1_1 = "byrefmark -8 () local_address -8 () saving_stack 1 () class_reference 59 () saving_stack () argument () direct_call_op 18434 (type 59 ()) local_address -8 () copying -4 (size 4 ())";

void BTOptimization1_1 :: SetUp()
{
   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   BuildTreeWriter buildWriter(buildTree);
   buildWriter.appendNode(BuildKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);
   exprNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 2);

   SyntaxTreeSerializer::load(Declaration1_1, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree1_1, exprNode);

   auto outputNode = buildTree.readRoot();
   BuildTreeSerializer::load(BuildTree1_1, outputNode);
}