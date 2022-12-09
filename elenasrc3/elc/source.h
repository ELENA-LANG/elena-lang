//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Source Reader class declaration.
//
//                                             (C)2021-2022, by Aleksey Rakov
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

   // --- SourceReader ---
   class SourceReader : protected TextParser<char, LINE_LEN>
   {
      bool _operatorMode;

      void copyToken(char* token, size_t length);

      bool IsOperator(char state)
      {
         return (state == dfaOperator || state == dfaAltOperator || state == dfaGrOperator);
      }

      void resolveSignAmbiguity(SourceInfo& info)
      {
         // if it is not preceeded by an operator
         if (IsOperator(info.state)) {
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
      SourceInfo read(char* line, size_t length);

      SourceReader(int tabSize, UStrReader* source);
   };

}

#endif
