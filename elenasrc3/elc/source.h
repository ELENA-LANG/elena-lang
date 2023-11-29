//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Source Reader class declaration.
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SOURCE_H
#define SOURCE_H

#include "textparser.h"

namespace elena_lang
{
   struct SourceInfo
   {
      LineInfo lineInfo;
      char     state;
      ustr_t   symbol;

      SourceInfo()
      {
         state = 0;
      }
   };

   inline bool isQuote(char state)
   {
      return state == dfaQuoteStart;
   }

   // --- SourceReader ---
   class SourceReader : protected TextParser<char, LINE_LEN, dfaStart, dfaMaxChar, isQuote>
   {
      bool _operatorMode;

      ustr_t copyToken(char* token, size_t length);
      ustr_t copyQuote(char* token, size_t length, List<char*, freestr>& dynamicStrings);

      bool IsOperator(char state)
      {
         return (state == dfaOperator || state == dfaAltOperator 
            || state == dfaGrOperator || state == dfaIncOperator
            || state == dfaIfOperator || state == dfaElseOperator || state == dfaAltOpOperator);
      }
      bool IsQuoteToken(char state)
      {
         return (state == dfaQuote || state == dfaWideQuote || state == dfaCharacter);
      }

      void resolveSignAmbiguity(SourceInfo& info)
      {
         // if it is not preceeded by an operator
         if (!_operatorMode) {
            info.state = dfaOperator;
         }
         // otherwise check if it could be part of numeric constant
         else {
            pos_t rollback = _position;
            pos_t rollbackStart = _startPosition;

            char terminalState = TextParser::read(dfaSignStart, info.state, info.lineInfo);
            // rollback if lookahead fails
            if (terminalState == dfaBack) {
               _position = rollback;
               info.state = dfaOperator;
            }
            // resolve dot ambiguity
            else if (terminalState == dfaDotLookahead) {
               resolveDotAmbiguity(info);
            }

            _startPosition = rollbackStart;
         }
      }

      void resolveDotAmbiguity(SourceInfo& info)
      {
         char state = info.state;
         pos_t rollback = _position;
         pos_t rollbackStart = _startPosition;

         nextColumn();

         char endState = TextParser::read(dfaDotStart, info.state, info.lineInfo);
         // rollback if lookahead fails
         if (endState == dfaBack) {
            _position = rollback;
            info.state = state;
         }

         _startPosition = rollbackStart;
      }

   public:
      SourceInfo read(char* line, size_t length, List<char*, freestr>& dynamicStrings);

      SourceReader(int tabSize, UStrReader* source);
   };

}

#endif
