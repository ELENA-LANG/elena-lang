//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Text Reader class declaration.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef textsourceH
#define textsourceH 1

#include "textparser.h"

namespace _ELENA_TOOL_
{

// --- ELENA DFA Constants ---
const char dfaMaxChar        = 127;

const char dfaStart          = 'a';
const char dfaError          = '?';
const char dfaEOF            = '.';
const char dfaWhitespace     = '*';
const char dfaKeyword        = 'b';
const char dfaIdentifier     = 'd';
const char dfaInteger        = 'e';
const char dfaHexInteger     = 'g';
const char dfaLong           = 'h';
const char dfaReal           = 'i';
const char dfaFullIdentifier = 'p';
const char dfaPrivate        = 'q';
const char dfaQuote          = 's';
const char dfaMinusLA        = '-';
const char dfaDotLA          = '$';

class TextSourceReader : public _ELENA_::_TextParser<dfaMaxChar, dfaStart, dfaWhitespace, _ELENA_::LINE_LEN>
{
protected:
   void copyToken(_ELENA_::LineInfo& info, char* token, size_t length)
   {
      info.length = _position - info.position;
      info.line = token;

      size_t len = info.length;
      _ELENA_::Convertor::copy(token, _line + info.position, len, length);
      token[length] = 0;
   }

   void copyQuote(_ELENA_::LineInfo& info)
   {
      info.length = _position - info.position;
      info.line = _line + info.position;
   }

public:
   void switchDFA(const char** dfa)
   {
      _dfa = dfa;
   }

   _ELENA_::LineInfo read(char* token, size_t length);

   TextSourceReader(const char** dfa, int tabSize, _ELENA_::TextReader* source);
   TextSourceReader(int tabSize, _ELENA_::TextReader* source);
};

} // _ELENA_TOOL_

#endif // textsourceH
