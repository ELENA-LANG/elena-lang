//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2023 by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TREEPARSER_H
#define TREEPARSER_H

#include "scriptparser.h"
#include "syntaxtree.h"
#include "langcommon.h"

namespace elena_lang
{
   // --- TreeScriptParser ---
   class TreeScriptParser : public ScriptEngineParserBase
   {
      TokenMap       _tokens;
      AttributeMap   _attributes;

      void parseScope(ScriptEngineReaderBase& reader, ScriptBookmark& bm,
         SyntaxTreeWriter& writer, SyntaxKey type);

      void parseStatement(ScriptEngineReaderBase& reader, ScriptBookmark& bm, 
         SyntaxTreeWriter& writer);

   public:
      void parse(ScriptEngineReaderBase& reader, MemoryDump* output) override;
      bool parseDirective(ScriptEngineReaderBase& reader, MemoryDump* output) override;
      bool parseGrammarRule(ScriptEngineReaderBase& reader) override;

      TreeScriptParser();
   };
}

#endif
