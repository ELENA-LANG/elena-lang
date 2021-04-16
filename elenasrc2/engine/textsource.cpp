//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Text Reader class implementation.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "textsource.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

// --- DFA Table ---

const char* DFA_table[21] =
{
        ".????????cc??c??????????????????clrbqmlpmmllmtmjeeeeeeeeeellllllmddddddddddddddddddddddddddululd?ddddddddddddddddddddddddddmlmmd",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbaaaababbbbbbbbbbbbbbbbbbbbbbbbbbaaaab",
        "*********cc*********************c***********************************************************************************************",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaapaaaaaaaaddddddddddaaaaaaaddddddddddddddddddddddddddaaaadaddddddddddddddddddddddddddaaaad",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa$aeeeeeeeeeeaaaaaaaffffffaaaaaaaaaaaaaaaaaaaaaaaaaaffffffagaaahaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaffffffffffaaaaaaaffffffaaaaaaaaaaaaaaaaaaaaaaaaaaffffffagaaahaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaiiiiiiiiiiaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaanaaaakaaaaaaaaaaaaamaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "*kkkkkkkkk*kk*kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaalaaaaaaaaaaaaaaaaaaalallllaaaaaaaaaaaaaaaaaaaaaaaaaaaalalaaaaaaaaaaaaaaaaaaaaaaaaaaaaalaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "?nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnonnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn",
        "?nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn*nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaapaaaaaaaappppppppppaaaaaaappppppppppppppppppppppppppaaaapappppppppppppppppppppppppppaaaap",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqqqqqqqqqqaaaaaaaqqqqqqqqqqqqqqqqqqqqqqqqqqaaaaqaqqqqqqqqqqqqqqqqqqqqqqqqqqaaaaq",
        "?rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrsrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaraaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaalaa----------lalllaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaalaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaamamaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
};

// --- TextSourceReader ---

TextSourceReader :: TextSourceReader(int tabSize, TextReader* source)
   : _TextParser(DFA_table, tabSize, source)
{
}

TextSourceReader :: TextSourceReader(const char** table, int tabSize, TextReader* source)
   : _TextParser(table ? table : DFA_table, tabSize, source)
{
}

inline bool isDigit(wchar_t ch)
{
   return (ch >='0' && ch <= '9');
}

LineInfo TextSourceReader :: read(char* token, size_t length)
{
   LineInfo info(_position, _column, _row, _sourcePos);
   char terminalState = readLineInfo(dfaStart, info);

   switch (terminalState) {
      case dfaError:
         throw InvalidChar(info.column, info.row, _line[_position]);
      case dfaEOF:
         info.state = dfaEOF;
         info.length = 0;
         break;
      case dfaDotLA:
         if (isDigit(_line[_position]))
            readLineInfo(dfaReal, info);
         break;
      case dfaMinusLA:
         if (_position < 2 || !isDigit(_line[_position - 2]))
            readLineInfo(dfaInteger, info);
         break;
   }

   if (info.state == dfaQuote) {
      copyQuote(info);
   }
   else if(token != NULL) 
      copyToken(info, token, length);

   return info;
}
