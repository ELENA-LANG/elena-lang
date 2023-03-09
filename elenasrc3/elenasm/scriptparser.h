//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2023 by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

#include "elena.h"
#include "scriptreader.h"

namespace elena_lang
{
   typedef Map<pos_t, LineInfo> CoordMap;

   // --- InvalidOperationError ---
   class InvalidOperationError
   {
      ustr_t error;

   public:
      ustr_t Error() const { return error; }

      InvalidOperationError(ustr_t error)
      {
         this->error = error;
      }
   };

   // --- ScriptBookmark ---
   struct ScriptBookmark
   {
      pos_t    offset;
      int      state;
      LineInfo lineInfo;
   };

   // --- ScriptEngineReaderBase ---
   class ScriptEngineReaderBase
   {
   public:
      virtual ScriptBookmark read() = 0;

      virtual ustr_t lookup(ScriptBookmark& bm) = 0;

      virtual bool compare(ustr_t value) = 0;
   };

   // --- ScriptEngineReader ---
   class ScriptEngineReader : public ScriptEngineReaderBase
   {
   protected:
      ScriptReader sourceReader;
      MemoryDump   buffer;
      ScriptToken  token;
      bool         eof;

      CoordMap*    coordinates;

   public:
      ScriptBookmark read() override;

      ustr_t lookup(ScriptBookmark& bm) override;

      bool compare(ustr_t value) override;

      ScriptEngineReader(UStrReader* textReader);
   };

   // --- ScriptEngineParserBase ---
   class ScriptEngineParserBase
   {
   public:
      virtual bool parseGrammarRule(ScriptEngineReader& reader) = 0;

      virtual ~ScriptEngineParserBase() = default;
   };
}

#endif
