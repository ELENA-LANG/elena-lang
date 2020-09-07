//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2020, by Alexei Rakov
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
   loadSyntaxTokens(_tokens);

   _attributes.add("singleton", V_SINGLETON);
   _attributes.add("preloaded_symbol", V_PRELOADED);
   _attributes.add("function", V_FUNCTION);
   _attributes.add("get_method", V_GETACCESSOR);
   _attributes.add("script_method", V_SCRIPTSELFMODE);
   _attributes.add("public_namespace", V_PUBLIC);
   _attributes.add("script_function", V_SCRIPTSELFMODE);
   _attributes.add("script_function", V_FUNCTION);
   _attributes.add("variable_identifier", V_VARIABLE);
   _attributes.add("new_reference", V_NEWOP);
   _attributes.add("new_identifier", V_NEWOP);
   _attributes.add("prev_identifier", V_PREVIOUS);
   _attributes.add("loop_expression", V_LOOP);
}

void TreeScriptParser :: parseScope(_ScriptReader& reader, ScriptBookmark& bm, SyntaxWriter& writer, LexicalType type)
{
   bm = reader.read();
   while (!reader.Eof() && !reader.compare(")")) {
      if (reader.compare(";")) {
         writer.closeNode();  
         writer.inject(type);
      }
      else parseStatement(reader, bm, writer);

      bm = reader.read();
   }
}

void TreeScriptParser :: parseStatement(_ScriptReader& reader, ScriptBookmark& bm, SyntaxWriter& writer)
{
   int type = _tokens.get(reader.lookup(bm));
   if (type != 0) {
      auto attr_it = _attributes.getIt(reader.lookup(bm));

      bm = reader.read();
      if (reader.compare("(")) {
         writer.newBookmark();
         writer.newNode((LexicalType)type);
         while (!attr_it.Eof()) {
            writer.appendNode(lxAttribute, *attr_it);

            attr_it = _attributes.nextIt(attr_it.key(), attr_it);
         }            

         parseScope(reader, bm, writer, (LexicalType)type);
         writer.removeBookmark();
      }
      else if (reader.compare("=")) {
         while (!attr_it.Eof()) {
            writer.appendNode(lxAttribute, *attr_it);

            attr_it = _attributes.nextIt(attr_it.key(), attr_it);
         }

         bm = reader.read();

         if ((LexicalType)type == lxCharacter) {
            // HOTFIX : to support character
            QuoteTemplate<IdentifierString> quote(reader.lookup(bm));

            writer.newNode((LexicalType)type, quote.ident());

         }
         else if (bm.state == dfaInteger && reader.compare("0")) {
            writer.newNode((LexicalType)type);
         }
         else writer.newNode((LexicalType)type, reader.lookup(bm));

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
