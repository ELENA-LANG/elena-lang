//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Source Reader class implementation.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "source.h"

using namespace elena_lang;

const char* source_dfa[21] =
{
        ".????????bb??b??????????????????b?firc?ldd?ddddheeeeeeeeeeddddd??ccccccccccccccccccccccccccd?ddc?ccccccccccccccccccccccccccd?d??",
        "*********bb*********************b***********************************************************************************************",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaalaaaaaaaaccccccccccaaaaaaaccccccccccccccccccccccccccaaaacaccccccccccccccccccccccccccaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadaadaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaeeeeeeeeeeaaaaaaakkkkkkaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaajaaaaaaaaaaaaaaaaaaaaaaa",
        "?fffffffffffffffffffffffffffffffffgfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaataaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaoaaaanaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaiiiiiiiiiiiiiiiiiiiiiiiiiiaaaaiaiiiiiiiiiiiiiiiiiiiiiiiiiiaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "????????????????????????????????????????????????kkkkkkkkkk???????kkkkkk?????????????????????????????????j???????????????????????",
        "?????????????????????????????????????????????????????????????????mmmmmmmmmmmmmmmmmmmmmmmmmm????m?mmmmmmmmmmmmmmmmmmmmmmmmmm????m",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaalaaaaaaaammmmmmmmmmaaaaaaammmmmmmmmmmmmmmmmmmmmmmmmmaaaamammmmmmmmmmmmmmmmmmmmmmmmmmaaaam",
        "*nnnnnnnnn*nn*nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn",
        "?ooooooooooooooooooooooooooooooooooooooooopooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo",
        "?ooooooooooooooooooooooooooooooooooooooooooooooboooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaassssssssssaaaaaaarrrrrrrrrrrrrrrrrrrrrrrrrraaaaaarrrrrrrrrrrrrrrrrrrrrrrrrraaaaa",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaassssssssssaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "????????????????????????????????????????????????uuuuuuuuuu??????????????????????????????????????????????????????????????????????",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaagaaaaaaaaaaauuuuuuuuuuaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
};

// --- SourceReader ---

SourceReader::SourceReader(int tabSize, UStrReader* reader)
   : TextParser(source_dfa, tabSize, reader)
{

}

void SourceReader :: copyToken(char* token, size_t length)
{
   size_t tokenLen = _position - _startPosition;

   StrConvertor::copy(token, _line + _startPosition, tokenLen, length);
   token[length] = 0;
}

SourceInfo SourceReader :: read(char* line, size_t length)
{
   SourceInfo info;

   char terminalState = TextParser::read(dfaStart, info.state, info.lineInfo);
   switch (terminalState) {
      case dfaError:
         throw InvalidChar(info.lineInfo, _line[_position]);
      case dfaEOF:
         info.state = dfaEOF;
         info.symbol = nullptr;
         return info;
      default:
         // to make compiler happy
         break;
   }

   copyToken(line, length);
   info.symbol = line;

   return info;
}

