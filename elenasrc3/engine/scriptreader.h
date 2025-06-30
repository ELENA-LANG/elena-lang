//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Script Reader class declaration.
//
//                                             (C)2021-2024, by Aleksey Rakov
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

      bool compare(ustr_t s) const 
      {
         return (*token).compare(s);
      }

      ScriptToken()
      {
         state = 0;
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

         tokenInfo.token.copy(quote.str());
      }

   public:
      enum class DFAMode
      {
         Normal = 0,
         Symbolic
      };

      void switchMode(DFAMode mode);

      bool read(ScriptToken& token);

      void resetReader()
      {
         reset();
      }

      ScriptReader(int tabSize, UStrReader* reader);
      ScriptReader(const char** dfa, int tabSize, UStrReader* reader);
   };
}

#endif