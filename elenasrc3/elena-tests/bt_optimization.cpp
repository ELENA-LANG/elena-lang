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

void BTOptimization1 :: SetUp()
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

void BTOptimization1 :: runCompilerTest()
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true, false);
   moduleScope->buildins.superReference = 1;
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

void BTOptimization1 :: runBTTest()
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
   BTOptimization1::SetUp();

   SyntaxTreeSerializer::load(Declaration1_1, declarationNode);
   SyntaxTreeSerializer::load(SyntaxTree1_1, exprNode);

   BuildTreeSerializer::load(BuildTree1_1, beforeOptimization);
   BuildTreeSerializer::load(OptimizedBuildTree1_1, afterOptimization);
}