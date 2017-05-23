//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "treeparser.h"
#include "bytecode.h"
#include "compilercommon.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

// --- TreeScriptParser ---

TreeScriptParser :: TreeScriptParser()
{
   _tokens.add("root", lxRoot);
   _tokens.add("class", lxClass);
   _tokens.add("singleton", lxClass);
   _tokens.add("nested", lxNestedClass);
   _tokens.add("method", lxClassMethod);
   _tokens.add("message", lxMessage);
   _tokens.add("code", lxCode);
   _tokens.add("expression", lxExpression);
   _tokens.add("returning", lxReturning);
   _tokens.add("symbol", lxSymbol);
   _tokens.add("preloaded_symbol", lxSymbol);
   _tokens.add("literal", lxLiteral);
   _tokens.add("identifier", lxIdentifier);
   _tokens.add("numeric", lxInteger);
   _tokens.add("parameter", lxMethodParameter);
   _tokens.add("include", lxInclude);
   _tokens.add("forward", lxForward);
   _tokens.add("reference", lxReference);
   _tokens.add("variable", lxVariable);
   _tokens.add("assign", lxAssign);

   _attributes.add("singleton", V_SINGLETON);
   _attributes.add("preloaded_symbol", V_PRELOADED);
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
      int attr = _attributes.get(reader.lookup(bm));

      bm = reader.read();
      if (reader.compare("(")) {
         writer.newNode((LexicalType)type);
         if (attr != 0)
            writer.appendNode(lxAttribute, attr);

         parseScope(reader, bm, writer);
      }
      else if (reader.compare("=")) {
         bm = reader.read();

         writer.newNode((LexicalType)type, reader.lookup(bm));

         writer.appendNode(lxRow, bm.row);
         writer.appendNode(lxCol, bm.column);
         //writer.appendNode(lxLength, getlength(reader.lookup(bm)));
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
