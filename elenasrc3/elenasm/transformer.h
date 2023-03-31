//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include "scriptparser.h"

namespace elena_lang
{
   // --- ScriptEngineTextParser ---
   class ScriptEngineTextParser : public ScriptEngineParserBase
   {
      int _width;

   public:
      bool parseGrammarRule(ScriptEngineReaderBase& reader) override;

      void parse(ScriptEngineReaderBase& reader, MemoryDump* output) override;

      ScriptEngineTextParser()
      {
         _width = 0x50;
      }
   };

   // --- ScriptEngineBuilder ---
   class ScriptEngineBuilder : public ScriptEngineParserBase
   {
      int _width;

      void saveToken(MemoryWriter& writer, ScriptEngineReaderBase& reader, ScriptBookmark bm);

      void saveClass(MemoryWriter& writer, ScriptEngineReaderBase& reader, Stack<ScriptBookmark>& stack, int allocated,
         int& maxAllocated, int& maxStackSize);
      void flush(MemoryWriter& writer, ScriptEngineReaderBase& reader, Stack<ScriptBookmark>& stack);

   public:
      bool parseGrammarRule(ScriptEngineReaderBase& reader) override;

      void parse(ScriptEngineReaderBase& reader, MemoryDump* output) override;

      ScriptEngineBuilder()
      {
         _width = 0x50;
      }
   };

}

#endif
