//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Source Reader class declaration.
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef sourceH
#define sourceH 1

#include "textparser.h"

namespace _ELENA_
{

// --- ELENA DFA Constants ---
constexpr char dfaMaxChar        = 127;

constexpr char dfaStart          = 'A';
constexpr char dfaError          = '?';
constexpr char dfaEOF            = '.';
constexpr char dfaWhitespace     = '*';
constexpr char dfaBack           = '!';
constexpr char dfaDotLookahead   = '$';
constexpr char dfaMinusLookahead = '-';  // indicates that if minus is preceeded by the operator it may be part of the digit
constexpr char dfaAttribute      = 'B';
constexpr char dfaIdentifier     = 'D';
constexpr char dfaFullIdentifier = 'F';
constexpr char dfaWildcard       = 'G';
constexpr char dfaOperator       = 'H';
constexpr char dfaDblOperator    = 'M';
constexpr char dfaInteger        = 'N';
constexpr char dfaDotStart       = 'O';
constexpr char dfaExplicitConst  = 'S';
constexpr char dfaLong           = 'R';
constexpr char dfaHexInteger     = 'T'; // should be kept for compatibility
constexpr char dfaGenericReal    = 'P';
constexpr char dfaReal           = 'Q';
constexpr char dfaSignStart      = 'U';
constexpr char dfaQuoteStart     = 'V';
constexpr char dfaQuote          = 'W';
constexpr char dfaCharacter      = ']';
constexpr char dfaWideQuote      = '^';
constexpr char dfaMetaConstant   = 'a';
constexpr char dfaGlobal         = 'b';
constexpr char dfaAltOperator    = 'c';

inline bool isQuote(char state)
{
   return state == dfaQuoteStart;
}

class SourceReader : public _TextParser<dfaMaxChar, dfaStart, dfaWhitespace, LINE_LEN, isQuote>
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
      return (state == dfaOperator || state == dfaDblOperator || state == dfaAltOperator);
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

   void copyToken(LineInfo& info, char* token, size_t length)
   {
      info.length = _position - info.position;
      info.line = token;

      Convertor::copy(token, _line + info.position, info.length, length);
      token[info.length] = 0;
   }

   void copyQuote(LineInfo& info)
   {
      info.length = _position - info.position;
      info.line = _line + info.position;
   }

public:
   LineInfo read(char* token, size_t length);

   SourceReader(int tabSize, TextReader* source);
};

} // _ELENA_

#endif // sourceH
