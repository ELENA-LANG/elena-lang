//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Tree Serializer classes implementation.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "serializer.h"
#include "scriptreader.h"

using namespace elena_lang;

// --- SyntaxTreeReader ---

void syntaxTreeEncoder(int level, TextWriter<char>& writer, SyntaxKey key, ustr_t strArg, int arg, void* extraArg)
{
   if (key == SyntaxKey::None) {
      writer.writeText(")");

      return;
   }

   TokenMap* map = static_cast<TokenMap*>(extraArg);

   ustr_t keyName = map->retrieve<SyntaxKey>(nullptr, key, [](SyntaxKey arg, ustr_t value, SyntaxKey current)
      {
         return current == arg;
      });

   writer.writeTextLine(nullptr);

   for (int i = 0; i < level; i++)
      writer.writeChar(' ');

   if (keyName.empty()) {
      IdentifierString code;
      code.append('#');
      code.appendInt((int)key);

      writer.writeText(*code);
   }
   else writer.writeText(keyName);

   writer.writeChar(' ');
   if (!strArg.empty()) {
      writer.writeChar('"');
      writer.writeText(strArg);
      writer.writeChar('"');
      writer.writeChar(' ');
   }
   else if (arg) {
      String<char, 12> number;
      number.appendInt(arg);
      writer.writeText(number.str());
      writer.writeChar(' ');
   }

   writer.writeText("(");
}

void SyntaxTreeSerializer :: save(SyntaxNode node, DynamicUStr& target, List<SyntaxKey>* filters)
{
   TokenMap list(SyntaxKey::None);
   SyntaxTree::loadTokens(list);

   DynamicUStrWriter writer(&target);

   SyntaxTree::serialize(0, node, syntaxTreeEncoder, writer, &list, filters);
}

struct LoadScope
{
   ScriptReader scriptReader;
   TokenMap     list;

   LoadScope(UStrReader* reader)
      : scriptReader(4, reader), list(SyntaxKey::None)
   {
   }
};

bool syntaxTreeReader(SyntaxKey& key, IdentifierString& strValue, int& value, void* arg)
{
   LoadScope* scope = static_cast<LoadScope*>(arg);

   strValue.clear();

   ScriptToken token;
   scope->scriptReader.read(token);
   if (token.compare(")") || token.state == dfaEOF)
      return false;

   key = scope->list.get(*token.token);

   scope->scriptReader.read(token);
   if (token.state == dfaQuote) {
      strValue.copy(*token.token);

      scope->scriptReader.read(token);
   }
   else if (token.state == dfaInteger) {
      value = StrConvertor::toInt(*token.token, 10);

      scope->scriptReader.read(token);
   }
   else if (token.state == dfaOperator && token.compare("-")) {
      scope->scriptReader.read(token);
      value = -StrConvertor::toInt(*token.token, 10);

      scope->scriptReader.read(token);
   }
   else value = 0;

   return true;
}

void SyntaxTreeSerializer :: load(ustr_t source, SyntaxNode& target)
{
   StringTextReader<char> reader(source.str());
   LoadScope scope(&reader);

   SyntaxTree::loadTokens(scope.list);

   SyntaxTree::deserialize(target, syntaxTreeReader, &scope);
}

// --- BuildTreeSerializer ---

void buildTreeEncoder(int level, TextWriter<char>& writer, BuildKey key, ustr_t, int arg, void* extraArg)
{
   if (key == BuildKey::None) {
      writer.writeText(")");

      return;
   }

   BuildKeyMap* map = static_cast<BuildKeyMap*>(extraArg);

   ustr_t keyName = map->retrieve<BuildKey>(nullptr, key, [](BuildKey arg, ustr_t value, BuildKey current)
      {
         return current == arg;
      });

   writer.writeTextLine(nullptr);

   for (int i = 0; i < level; i++)
      writer.writeChar(' ');

   if (keyName.empty()) {
      IdentifierString code;
      code.append('#');
      code.appendInt((int)key);

      writer.writeText(*code);
   }
   else writer.writeText(keyName);

   writer.writeChar(' ');
   if (arg) {
      String<char, 12> number;
      number.appendInt(arg);
      writer.writeText(number.str());
      writer.writeChar(' ');
   }

   writer.writeText("(");
}

void BuildTreeSerializer :: save(BuildNode node, DynamicUStr& target, List<BuildKey>* filters)
{
   BuildKeyMap map(BuildKey::None);
   BuildTree::loadBuildKeyMap(map);

   DynamicUStrWriter writer(&target);

   BuildTree::serialize(0, node, buildTreeEncoder, writer, &map, filters);
}

struct BuildLoadScope
{
   ScriptReader scriptReader;
   BuildKeyMap  map;

   BuildLoadScope(UStrReader* reader)
      : scriptReader(4, reader), map(BuildKey::None)
   {
   }
};

bool buildTreeReader(BuildKey& key, IdentifierString&, int& value, void* arg)
{
   BuildLoadScope* scope = static_cast<BuildLoadScope*>(arg);

   ScriptToken token;
   scope->scriptReader.read(token);
   if (token.compare(")") || token.state == dfaEOF)
      return false;

   key = scope->map.get(*token.token);

   scope->scriptReader.read(token);
   if (token.state == dfaInteger) {
      value = StrConvertor::toInt(*token.token, 10);

      scope->scriptReader.read(token);
   }
   else if (token.state == dfaOperator && token.compare("-")) {
      scope->scriptReader.read(token);
      value = -StrConvertor::toInt(*token.token, 10);

      scope->scriptReader.read(token);
   }
   else value = 0;

   return true;
}

void BuildTreeSerializer :: load(ustr_t source, BuildNode& target)
{
   StringTextReader<char> reader(source.str());
   BuildLoadScope scope(&reader);

   BuildTree::loadBuildKeyMap(scope.map);

   BuildTree::deserialize(target, buildTreeReader, &scope);

}