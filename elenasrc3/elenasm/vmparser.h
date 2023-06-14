//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef VMPARSER_H
#define VMPARSER_H

#include "scriptparser.h"

namespace elena_lang
{
   typedef Stack<ScriptBookmark> ScriptStack;

   class VMTapeParser : public ScriptEngineParserBase
   {
      TempString _postfix;

      bool parseBuildScript(ScriptEngineReaderBase& reader, TapeWriter& writer);
      void parseInline(ustr_t script, TapeWriter& writer);
      void parseInlineScript(ScriptEngineReaderBase& reader, TapeWriter& writer);
      void parseInlineStatement(ScriptEngineReaderBase& reader, ScriptBookmark& bm, TapeWriter& writer);
      int parseBuildScriptStatement(ScriptEngineReaderBase& reader, ScriptBookmark& bm, ScriptStack& callStack, 
         pos_t exprBookmark);

      void writeBuildScriptStatement(ScriptEngineReaderBase& reader, ScriptBookmark bm, ScriptStack& callStack,
         TapeWriter& writer);
      int parseBuildScriptArgumentList(ScriptEngineReaderBase& reader, ScriptStack& callStack);

      int writeBuildScriptArgumentList(ScriptEngineReaderBase& reader, ScriptBookmark terminator,
         ScriptStack& callStack, TapeWriter& writer);

   public:
      bool parseDirective(ScriptEngineReaderBase& reader, MemoryDump* output) override;
      void parse(ScriptEngineReaderBase& reader, MemoryDump* output) override;
      bool parseGrammarRule(ScriptEngineReaderBase& reader) override;

      VMTapeParser();
   };

}

#endif
