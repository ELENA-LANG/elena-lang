//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA ScriptReader class implementation.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "scriptreader.h"
#include "dfa.h"

using namespace elena_lang;

// --- ScriptReader ---

ScriptReader::ScriptReader(int tabSize, UStrReader* reader)
   : TextParser(token_dfa, tabSize, reader)
{
   
}

ScriptReader :: ScriptReader(const char** dfa, int tabSize, UStrReader* reader)
   : TextParser(dfa, tabSize, reader)
{

}

bool ScriptReader::read(ScriptToken& tokenInfo)
{
   char terminalState = TextParser::read(dfaStart, tokenInfo.state, tokenInfo.lineInfo);

   switch (terminalState) {
      case dfaError:
         throw InvalidChar(tokenInfo.lineInfo, _line[_position]);
      case dfaEOF:
         tokenInfo.token.clear();
         tokenInfo.state = dfaEOF;
         return false;
      default:
         if (tokenInfo.state == dfaQuote || tokenInfo.state == dfaWideQuote) {
            copyQuote(tokenInfo);
         }
         else copyToken(tokenInfo);
         break;
   }

   return true;
}

void ScriptReader :: switchMode(DFAMode mode)
{
   switch (mode)  {
      case DFAMode::Symbolic:
         setDFA(symbolic_dfa);
         break;
      case DFAMode::Normal:
         setDFA(token_dfa);
      default:
         break;
   }
}
