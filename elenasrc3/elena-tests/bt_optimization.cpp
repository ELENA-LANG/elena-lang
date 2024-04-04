//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Optimization Fixture implementation
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "bt_optimization.h"
#include "serializer.h"
#include "bcwriter.h"

#include <windows.h>

using namespace elena_lang;

constexpr auto BT_RULES_FILE = "bt_rules60.dat";

inline void getAppPath(PathString& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(nullptr, path, MAX_PATH);

   appPath.copySubPath(path, false);
   appPath.lower();
}

// --- BTOptimization1 ---

constexpr auto Declaration1_1 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"Struct\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ()))) class ( attribute -2147471359 () nameattr (identifier \"TestReference\" ()) field (attribute -2147475454 () type (identifier \"Struct\" ()) nameattr (identifier \"_value\" ())) ) class (attribute -2147467263 ()attribute -2147479546 () nameattr (identifier \"Tester\" ()) method (type (identifier \"Struct\" ()) nameattr (identifier \"getValue\" ())code ())))";
constexpr auto SyntaxTree1_1 = "expression(assign_operation(object(type(identifier \"Struct\"())identifier \"r\"())expression(message_operation(object(identifier \"Tester\"())message(identifier \"getValue\" ())))))";
constexpr auto BuildTree1_1 = "byrefmark -8 () local_address -8 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2050 (type 4 ()) local_address -8 () copying -4 (size 4 ())";
constexpr auto OptimizedBuildTree1_1 = "local_address -4 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2050 (type 4 ()) local_address -4 ()";

constexpr auto Declaration1_2 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"Struct\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ()))) class ( attribute -2147471359 () nameattr (identifier \"TestReference\" ()) field (attribute -2147475454 () type (identifier \"Struct\" ()) nameattr (identifier \"_value\" ())) ) class (attribute -2147467263 ()attribute -2147479546 () nameattr (identifier \"Tester\" ()) method (attribute -2147463167 () type (identifier \"Struct\" ()) nameattr (identifier \"Value\" ())code ())))";
constexpr auto SyntaxTree1_2 = "expression(assign_operation(object(type(identifier \"Struct\"())identifier \"r\"())expression(property_operation(object(identifier \"Tester\"())message(identifier \"Value\" ())))))";
constexpr auto BuildTree1_2 = "byrefmark -8 () local_address -8 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2242 (type 4 ()) local_address -8 () copying -4 (size 4 ())";
constexpr auto OptimizedBuildTree1_2 = "local_address -4 () saving_stack 1 () class_reference 4 () saving_stack () argument () direct_call_op 2242 (type 4 ()) local_address -4 ()";

constexpr auto SyntaxTree1_3 = "expression(assign_operation(object(type(identifier \"Struct\" ())identifier \"r\"())expression(value_operation(expression(object(identifier \"Tester\"()))))))";

constexpr auto Declaration2 = "namespace (class ( nameattr (identifier \"Object\" ())) class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"IntNumber\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ()))) class ( attribute -2147471359 () nameattr (identifier \"IntNumberReference\" ()) field (attribute -2147475454 () type (identifier \"IntNumber\" ()) nameattr (identifier \"_value\" ())) ))";
constexpr auto SyntaxTree2 = "expression (assign_operation (object (type (identifier \"IntNumber\" ()) identifier \"n\" ()) expression (object (integer \"2\"))))";
constexpr auto BuildTree2 = "int_literal 2 (value 2 ()) copying -4 ( size 4 ())";
constexpr auto OptimizedBuildTree2 = "saving_int - 4 (size 4 ()value 2 ())";

void BTOptimization :: SetUp()
{
   PathString appPath;
   getAppPath(appPath);

   PathString btRulesPath(*appPath, BT_RULES_FILE);
   FileReader btRuleReader(*btRulesPath, FileRBMode, FileEncoding::Raw, false);
   if (btRuleReader.isOpen()) {
      btRules.load(btRuleReader, btRuleReader.length());
   }

   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   BuildTreeWriter buildWriter(buildTree);
   buildWriter.appendNode(BuildKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);
   exprNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 2);

   beforeOptimization = buildTree.readRoot().appendChild(BuildKey::Tape);
   afterOptimization = buildTree.readRoot().appendChild(BuildKey::Tape);
}

void BTOptimization :: runCompilerTest(bool declareOperators)
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true, false);
   moduleScope->buildins.superReference = 1;
   moduleScope->buildins.intReference = 2;
   moduleScope->buildins.wrapperTemplateReference = 3;

   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   Compiler::Namespace nsScope(compiler, moduleScope, nullptr, nullptr, nullptr);

   Compiler::Class cls(nsScope, 0, Visibility::Internal);
   Compiler::Method method(cls);
   Compiler::Code code(method);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   if (declareOperators)
      env.initializeOperators(moduleScope);

   writer.newNode(BuildKey::Tape);
   Compiler::Expression expression(code, writer);
   expression.compileRoot(exprNode.firstChild(), ExpressionAttribute::NoDebugInfo);
   writer.closeNode();

   // Assess
   bool matched = BuildTree::compare(beforeOptimization, output.readRoot(), true);
   EXPECT_TRUE(matched);

   freeobj(compiler);
   freeobj(moduleScope);
}

void BTOptimization :: runBTTest()
{
   // Arrange
   ByteCodeWriter::BuildTreeOptimizer buildTreeOptimizer;
   MemoryReader reader(&btRules);
   buildTreeOptimizer.load(reader);

   BuildTree output;
   BuildTreeWriter writer(output);
   BuildTree::copyNode(writer, beforeOptimization, true);

   // Act
   buildTreeOptimizer.proceed(output.readRoot());

   // Assess
   bool matched = BuildTree::compare(output.readRoot(), afterOptimization, true);
   EXPECT_TRUE(matched);
}

// --- BTOptimization1_1 ---

void BTOptimization1_1 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration1_1, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree1_1, exprNode);

   BuildTreeSerializer::load(BuildTree1_1, beforeOptimization);
   BuildTreeSerializer::load(OptimizedBuildTree1_1, afterOptimization);
}

// --- BTOptimization1_2 ---

void BTOptimization1_2 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration1_2, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree1_2, exprNode);

   BuildTreeSerializer::load(BuildTree1_2, beforeOptimization);
   BuildTreeSerializer::load(OptimizedBuildTree1_2, afterOptimization);
}

// --- BTOptimization1_3 ---

void BTOptimization1_3 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration1_2, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree1_3, exprNode);

   BuildTreeSerializer::load(BuildTree1_2, beforeOptimization);
   BuildTreeSerializer::load(OptimizedBuildTree1_2, afterOptimization);
}

// --- BTOptimization2 ---

void BTOptimization2 :: SetUp()
{
   BTOptimization::SetUp();

   SyntaxTreeSerializer::load(Declaration2, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree2, exprNode);

   BuildTreeSerializer::load(BuildTree2, beforeOptimization);
   BuildTreeSerializer::load(OptimizedBuildTree2, afterOptimization);
}
