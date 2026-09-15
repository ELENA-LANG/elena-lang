//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Script Reader class declaration.
//
//                                             (C)2021-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SCRIPTREADER_H
#define SCRIPTREADER_H

#include "textparser.h"

namespace elena_lang
{
   // --- ScriptToken ---
   struct ScriptToken
   {
      LineInfo         lineInfo;
      IdentifierString token;
      char             state;
      char*            extraLongStr;

      bool compare(ustr_t s) const 
      {
         return (*token).compare(s);
      }

      void saveTo(MemoryWriter& writer)
      {
         if (!emptystr(extraLongStr)) {
            writer.writeString(extraLongStr);
            freestr(extraLongStr);
            extraLongStr = nullptr;
         }
         else writer.writeString(*token);
      }

      ScriptToken()
      {
         state = 0;
         extraLongStr = nullptr;
      }
      ~ScriptToken()
      {
         freestr(extraLongStr);
      }
   };

   // --- ScriptReader ---
   class ScriptReader : protected TextParser<char, LINE_LEN, dfaStart, dfaMaxChar, quote_matcher>
   {
      void copyToken(ScriptToken& tokenInfo)
      {
         pos_t len = _position - _startPosition;

         tokenInfo.token.copy(_line + _startPosition, len);
      }
      void copyQuote(ScriptToken& tokenInfo)
      {
         pos_t len = _position - _startPosition;

         QuoteString quote(_line + _startPosition, len);

         if (!tokenInfo.token.copy(quote.str())) {
            freestr(tokenInfo.extraLongStr);
            tokenInfo.extraLongStr = ustr_t(quote.str()).clone();
         }
      }

   public:
      enum class DFAMode
      {
         Normal = 0,
         Symbolic
      };

      void switchMode(DFAMode mode);

      bool read(ScriptToken& token);

      bool read(ScriptToken& token, ustr_t expected)
      {
         return read(token) && token.compare(expected);
      }

      void resetReader()
      {
         reset();
      }

      ScriptReader(int tabSize, UStrReader* reader);
      ScriptReader(const char** dfa, int tabSize, UStrReader* reader);
   };
}

#endif