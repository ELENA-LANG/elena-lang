//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "treeparser.h"

using namespace elena_lang;

// --- TreeScriptParser ---

TreeScriptParser :: TreeScriptParser()
   : _tokens(SyntaxKey::None), _attributes(0)
{
   SyntaxTree::loadTokens(_tokens);
   LangHelper::loadAttributes(_attributes);
}

void TreeScriptParser :: parseScope(ScriptEngineReaderBase& reader, ScriptBookmark& bm,
   SyntaxTreeWriter& writer, SyntaxKey type)
{
   bm = reader.read();
   while (!reader.eof() && !reader.compare(")")) {
      if (reader.compare(";")) {
         writer.closeNode();
         writer.inject(type);
      }
      else parseStatement(reader, bm, writer);

      bm = reader.read();
   }
}

void TreeScriptParser :: parseStatement(ScriptEngineReaderBase& reader, ScriptBookmark& bm, 
   SyntaxTreeWriter& writer)
{
   SyntaxKey key = _tokens.get(reader.lookup(bm));
   if (key != SyntaxKey::None) {
      auto attr_it = _attributes.getIt(reader.lookup(bm));

      bm = reader.read();
      if (reader.compare("(")) {
         writer.newBookmark();
         writer.newNode(key);
         while (!attr_it.eof()) {
            writer.appendNode(SyntaxKey::Attribute, *attr_it);

            attr_it = _attributes.nextIt(attr_it.key(), attr_it);
         }

         parseScope(reader, bm, writer, key);
         writer.removeBookmark();
      }
      else if (reader.compare("=")) {
         while (!attr_it.eof()) {
            writer.appendNode(SyntaxKey::Attribute, *attr_it);

            attr_it = _attributes.nextIt(attr_it.key(), attr_it);
         }

         bm = reader.read();

         if (bm.state == dfaInteger && reader.compare("0")) {
            writer.newNode(key);
         }
         else writer.newNode(key, reader.lookup(bm));

         writer.appendNode(SyntaxKey::Row, bm.lineInfo.row);
         writer.appendNode(SyntaxKey::Column, bm.lineInfo.column);
         //writer.appendNode(lxLength, getlength(reader.lookup(bm)));
      }
      else throw SyntaxError("invalid grammar rule", bm.lineInfo);

      writer.closeNode();
   }
   else throw SyntaxError("invalid grammar rule", bm.lineInfo);
}

void TreeScriptParser :: parse(ScriptEngineReaderBase& reader, MemoryDump* output)
{
   MemoryWriter writer(output);
   writer.writeDWord(0); // HOTFIX : end of tape
   pos_t sizePos = writer.position();
   writer.writeDWord(0);

   SyntaxTree       tree;
   SyntaxTreeWriter treeWriter(tree);

   SyntaxTree       clipboard;
   SyntaxTreeWriter clipboardWriter(clipboard);
   clipboardWriter.newNode(SyntaxKey::Root);

   ScriptBookmark bm = reader.read();
   while (!reader.eof()) {
      if (reader.compare("clipboard")) {
         bm = reader.read();
         if (reader.compare("(")) {
            clipboardWriter.closeNode();

            SyntaxTree::copyNode(treeWriter, clipboard.readRoot());
            clipboardWriter.clear();
            clipboardWriter.newNode(SyntaxKey::Root);

            bm = reader.read();
            if (reader.compare(")"))
               throw SyntaxError("closing bracket is expected", bm.lineInfo);
         }
         else parseStatement(reader, bm, clipboardWriter);
      }
      else parseStatement(reader, bm, treeWriter);

      bm = reader.read();
   }

   if (tree.save(output)) {
      writer.seek(sizePos);
      writer.writeDWord(output->length() - sizePos);
   }
   else output->clear();
}

bool TreeScriptParser::parseDirective(ScriptEngineReaderBase& reader, MemoryDump* output)
{
   return false;
}

bool TreeScriptParser :: parseGrammarRule(ScriptEngineReaderBase& reader)
{
   return false;
}
