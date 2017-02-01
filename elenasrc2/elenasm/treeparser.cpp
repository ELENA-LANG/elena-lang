//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "treeparser.h"
#include "bytecode.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

// --- TreeScriptParser ---

TreeScriptParser :: TreeScriptParser()
{
   _tokens.add("root", lxRoot);
   _tokens.add("class", lxClass);
   _tokens.add("symbol_decl", lxSymbol);
   _tokens.add("nested_decl", lxNestedClass);
   _tokens.add("method", lxClassMethod);
   _tokens.add("message", lxMessage);
   _tokens.add("code", lxCode);
   _tokens.add("expression", lxExpression);
   _tokens.add("returning", lxReturning);
   _tokens.add("symbol", lxReference);
   _tokens.add("literal", lxLiteral);
   _tokens.add("identifier", lxIdentifier);
   _tokens.add("numeric", lxInteger);
   _tokens.add("method_param", lxMethodParameter);
   _tokens.add("include", lxInclude);
   _tokens.add("forward", lxForward);
   _tokens.add("reference", lxReference);
}

void TreeScriptParser :: parseScope(_ScriptReader& reader, ScriptBookmark& bm, SyntaxWriter& writer)
{
   bm = reader.read();
   while (!reader.Eof() && !reader.compare(")")) {
      parseStatement(reader, bm, writer);

      bm = reader.read();
   }
}

void TreeScriptParser :: parseStatement(_ScriptReader& reader, ScriptBookmark& bm, SyntaxWriter& writer)
{
   int type = _tokens.get(reader.lookup(bm));
   if (type != 0) {
      writer.newNode((LexicalType)type);

      bm = reader.read();
      if (reader.compare("(")) {
         parseScope(reader, bm, writer);
      }
      else if (reader.compare("=")) {
         bm = reader.read();

         writer.appendNode(lxTerminal, reader.lookup(bm));
      }
      else throw EParseError(bm.column, bm.row);

      writer.closeNode();
   }
   else throw EParseError(bm.column, bm.row);
}

void TreeScriptParser :: parse(_ScriptReader& reader, MemoryDump* output)
{
   MemoryWriter writer(output);
   writer.writeDWord(0); // HOTFIX : end of tape
   int sizePos = writer.Position();
   writer.writeDWord(0);

   SyntaxTree tree;
   SyntaxWriter treeWriter(tree);

   ScriptBookmark bm = reader.read();
   while (!reader.Eof()) {      
      parseStatement(reader, bm, treeWriter);

      bm = reader.read();
   }   

   if (tree.save(output)) {
      writer.seek(sizePos);
      writer.writeDWord(output->Length() - sizePos);
   }
   else output->clear();
}
