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

void syntaxTreeEncoder(TextWriter<char>& writer, SyntaxKey key, ustr_t strArg, int arg, void* extraArg)
{
   if (key == SyntaxKey::None) {
      writer.writeTextLine(")");

      return;
   }

   TokenMap* map = static_cast<TokenMap*>(extraArg);

   ustr_t keyName = map->retrieve<SyntaxKey>(nullptr, key, [](SyntaxKey arg, ustr_t value, SyntaxKey current)
      {
         return current == arg;
      });

   writer.writeText(keyName);
   if (!strArg.empty()) {
      writer.writeChar('"');
      writer.writeText(strArg);
      writer.writeChar('"');
      writer.writeChar(' ');
   }
   else if (arg) {
      String<char, 4> number;
      number.appendInt(arg);
      writer.writeText(number.str());
      writer.writeChar(' ');
   }

   writer.writeTextLine("(");
}

void SyntaxTreeSerializer :: save(SyntaxNode node, DynamicUStr& target)
{
   TokenMap list(SyntaxKey::None);
   SyntaxTree::loadTokens(list);

   DynamicUStrWriter writer(&target);

   SyntaxTree::serialize(node, syntaxTreeEncoder, writer, &list);
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

   ScriptToken token;
   scope->scriptReader.read(token);
   if (token.compare(")"))
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

   return true;
}

void SyntaxTreeSerializer :: load(ustr_t source, SyntaxNode& target)
{
   StringTextReader<char> reader(source.str());
   LoadScope scope(&reader);

   SyntaxTree::loadTokens(scope.list);

   SyntaxTree::deserialize(target, syntaxTreeReader, &scope);
}
