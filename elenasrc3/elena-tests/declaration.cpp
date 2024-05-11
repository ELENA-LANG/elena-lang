//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Optimization Fixture implementation
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "declaration.h"
#include "serializer.h"
#include "bcwriter.h"

#include "parser.h"
#include "cliconst.h"
#include "derivation.h"

using namespace elena_lang;

constexpr auto Field_TemplateBasedArray = "class (nameattr (identifier \"A\" ())field (array_type (type (template_type (identifier \"VarTuple\" ()template_arg (identifier \"object\" ())template_arg (identifier \"object\" ()))))nameattr (identifier \"_array\" ())))";

constexpr auto Src_Field_TemplateBasedArray = "A { VarTuple<object, object>[] _array; }";

// --- DeclarationFixture ---

void DeclarationFixture :: SetUp()
{
   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);
}

void DeclarationFixture :: runTest()
{
   // Arrange
   TerminalMap terminals(
      SyntaxTree::toParseKey(SyntaxKey::eof),
      SyntaxTree::toParseKey(SyntaxKey::identifier),
      SyntaxTree::toParseKey(SyntaxKey::reference),
      SyntaxTree::toParseKey(SyntaxKey::globalreference),
      SyntaxTree::toParseKey(SyntaxKey::string),
      SyntaxTree::toParseKey(SyntaxKey::character),
      SyntaxTree::toParseKey(SyntaxKey::wide),
      SyntaxTree::toParseKey(SyntaxKey::integer),
      SyntaxTree::toParseKey(SyntaxKey::hexinteger),
      SyntaxTree::toParseKey(SyntaxKey::longinteger),
      SyntaxTree::toParseKey(SyntaxKey::real),
      SyntaxTree::toParseKey(SyntaxKey::constant));

   PathString appPath;
   getAppPath(appPath);
   StringTextReader<char> reader(_src);

   PathString syntaxPath(*appPath, SYNTAX_FILE);
   FileReader syntax(*syntaxPath, FileRBMode, FileEncoding::Raw, false);

   auto parser = new Parser(&syntax, terminals, nullptr);

   ModuleScopeBase* moduleScope = env.createModuleScope(true, false);

   SyntaxTreeBuilder builder(&syntaxTree, nullptr, moduleScope, nullptr);

   // Act
   builder.newNode(SyntaxTree::toParseKey(SyntaxKey::Namespace));
   parser->parse(&reader, &builder);
   builder.closeNode();

   // Assess
   bool matched = SyntaxTree::compare(syntaxTree.readRoot().findChild(SyntaxKey::Namespace), declarationNode, true);
   EXPECT_TRUE(matched);
}

// --- TemplateArrayFixture ---

void TemplateArrayFixture :: SetUp()
{
   DeclarationFixture::SetUp();

   LoadDeclarationScenario("$1", Field_TemplateBasedArray);

   _src = Src_Field_TemplateBasedArray;
}