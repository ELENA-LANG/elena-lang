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

   class ScriptEngineLog
   {
      MemoryDump _log;
      CoordMap   _coordinates;

   public:
      void writeCoord(LineInfo lineInfo)
      {
         _coordinates.add(_log.length(), lineInfo);
      }

      pos_t write(ustr_t token)
      {
         pos_t length = token.length_pos();
         if (length > 0) {
            MemoryWriter writer(&_log);

            writer.writeString(token, length);
         }

         return length;
      }

      static void writeQuote(MemoryWriter& writer, ustr_t token)
      {
         writer.writeChar('\"');
         size_t start = 0;
         while (true) {
            size_t quote_index = token.findSub(start, '"', NOTFOUND_POS);
            if (quote_index != NOTFOUND_POS) {
               writer.writeString(token.str() + start, (pos_t)(quote_index - start));
               writer.writeChar('\"');
               writer.writeChar('\"');
               start = quote_index + 1;
            }
            else {
               writer.writeString(token.str() + start, getlength(token) - (pos_t)start);

               break;
            }
         }

         writer.writeChar('\"');
      }

      void writeQuote(ustr_t token)
      {
         MemoryWriter writer(&_log);

         writeQuote(writer, token);
      }

      ScriptEngineLog()
         : _coordinates({ INVALID_POS })
      {
         
      }
   };

   // --- ScriptEngineParserBase ---
   class ScriptEngineParserBase
   {
   public:
      virtual bool parseGrammarRule(ScriptEngineReaderBase& reader) = 0;

      virtual ~ScriptEngineParserBase() = default;
   };
}

#endif
