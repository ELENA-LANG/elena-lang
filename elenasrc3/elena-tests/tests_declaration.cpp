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

constexpr auto Field_TemplateBasedArray = "namespace (class (nameattr (identifier \"A\" ())field (array_type (template_type (identifier \"VarTuple\" () template_arg (type(identifier \"object\" ())) template_arg (type(identifier \"object\" ()))))nameattr (identifier \"_array\" ())))";
constexpr auto New_TemplateBasedArray = "namespace (class (attribute -2147467263 (identifier \"public\" ())nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (expression (assign_operation (object (attribute -2147479539 (identifier \"var\" ())identifier \"array\" ())expression (message_operation (object (array_type (template_type (attribute -2147479534 (identifier \"new\" ())identifier \"VarTuple\" ()template_arg (type (identifier \"object\" ()))template_arg (type (identifier \"object\" ())))))expression (object (integer \"10\" ()))))))EOP (eop \"}\" ())))))";

constexpr auto Src_Field_TemplateBasedArray = "A { VarTuple<object, object>[] _array; }";
constexpr auto Src_New_TemplateBasedArray = "public program() { var array := new VarTuple<object, object>[](10); }";

// --- DeclarationFixture ---

void DeclarationFixture :: SetUp()
{
   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);
   sourceNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);
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
      SyntaxTree::toParseKey(SyntaxKey::constant),
      SyntaxTree::toParseKey(SyntaxKey::interpolate));

   PathString appPath;
   getAppPath(appPath);
   StringTextReader<char> reader(_src);

   PathString syntaxPath(*appPath, SYNTAX60_FILE);
   FileReader syntax(*syntaxPath, FileRBMode, FileEncoding::Raw, false);

   auto parser = new Parser(&syntax, terminals, nullptr);

   ModuleScopeBase* moduleScope = env.createModuleScope(true, true);

   SyntaxTreeBuilder builder(sourceNode, nullptr, moduleScope, nullptr, true);

   // Act
   builder.newNode(SyntaxTree::toParseKey(SyntaxKey::Namespace));
   parser->parse(&reader, &builder);
   builder.closeNode();

   // Assess
   bool matched = SyntaxTree::compare(sourceNode, declarationNode, true);
   EXPECT_TRUE(matched);
}

// --- TemplateArrayFixture ---

void TemplateArrayFixture :: SetUp()
{
   DeclarationFixture::SetUp();

   LoadDeclarationScenario("$1", Field_TemplateBasedArray);

   _src = Src_Field_TemplateBasedArray;
}

// --- NewTemplateArrayFixture ---

void NewTemplateArrayFixture :: SetUp()
{
   DeclarationFixture::SetUp();

   LoadDeclarationScenario("$1", New_TemplateBasedArray);

   _src = Src_New_TemplateBasedArray;
}