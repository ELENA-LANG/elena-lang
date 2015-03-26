//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Source Reader class declaration.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef sourceH
#define sourceH 1

#include "textparser.h"

namespace _ELENA_
{

// --- ELENA DFA Constants ---
const char dfaMaxChar        = 127;

const char dfaStart          = 'a';
const char dfaError          = '?';
const char dfaEOF            = '.';
const char dfaWhitespace     = '*';
const char dfaBack           = '!';
const char dfaDotLookahead   = '$';
const char dfaMinusLookahead = '-';  // indicates that if minus is preceeded by the operator it may be part of the digit
const char dfaKeyword        = 'b';
const char dfaIdentifier     = 'd';
const char dfaFullIdentifier = 'f';
const char dfaWildcard       = 'g';
const char dfaOperator       = 'h';
const char dfaDblOperator    = 'm';
const char dfaInteger        = 'n';
const char dfaDotStart       = 'o';
const char dfaReal           = 'q';
const char dfaLong           = 'r';
const char dfaHexInteger     = 't';
const char dfaSignStart      = 'u';
const char dfaQuote          = 'w';
const char dfaPrivate        = 'x';
const char dfaCharacter      = '}';

class SourceReader : public _TextParser<dfaMaxChar, dfaStart, dfaWhitespace, LINE_LEN>
{
   char _lastState;

   inline void resolveDotAmbiguity(LineInfo& info)
   {
      char state = info.state;
      size_t rollback = _position;
      nextColumn(_position);

      char endState = readLineInfo(dfaDotStart, info);
      // rollback if lookahead fails
      if (endState == dfaBack) {
         _position = rollback;
         info.state = state;
      }
   }

   inline bool IsOperator(char state)
   {
      return (state == dfaOperator || state == dfaDblOperator);
   }

   inline void resolveSignAmbiguity(LineInfo& info)
   {
       // if it is not preceeded by an operator
      if (!IsOperator(_lastState)) {
         info.state = dfaOperator;
      }
      // otherwise check if it could be part of numeric constant
      else {
         size_t rollback = _position;

         char terminalState = readLineInfo(dfaSignStart, info);
         // rollback if lookahead fails
         if (terminalState == dfaBack) {
            _position = rollback;
            info.state = dfaOperator;
         }
         // resolve dot ambiguity
         else if (terminalState == dfaDotLookahead) {
            resolveDotAmbiguity(info);
         }
      }
  }

   void copyToken(LineInfo& info, ident_c* token, size_t length)
   {
      info.length = _position - info.position;
      info.line = token;

      StringHelper::copy(token, _line + info.position, info.length, length);
      token[info.length] = 0;
   }

   void copyQuote(LineInfo& info)
   {
      info.length = _position - info.position;
      info.line = _line + info.position;
   }

public:
   LineInfo read(ident_c* token, size_t length);

   SourceReader(int tabSize, TextReader* source);
};

} // _ELENA_

#endif // sourceH
