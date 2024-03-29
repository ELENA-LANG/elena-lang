//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Source Reader class implementation.
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "source.h"

using namespace elena_lang;

const char* source_dfa[37] =
{
        ".????????BB??B??????????????????BdFIRCDLDQDbDVQHEEEEEEEEEEDDDDYc`CCCCCCCCCCCCCCCCCCCCCCCCCCDeQDC?CCCCCCCCCCCCCCCCCCCCCCCCCCDDDDC",
        "*********BB*********************B***********************************************************************************************",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALAAAAAAAACCCCCCCCCCAAAAAAACCCCCCCCCCCCCCCCCCCCCCCCCCAAAACACCCCCCCCCCCCCCCCCCCCCCCCCCAAAAC",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAQAADQDAAAAAAAAAAAAAAAAAAAAAAAAAAAAQQQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA$AEEEEEEEEEEAAAAAAAKKKKKKJJJJJJJJJJJJJJJJJJJJAAAAAAJJJJ$JJJJJJJJJJJJJJJJJJJJJJJAJJ",
        "?FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFGFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAXAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOAAAANAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIIIIIIIIIIIIIIIIIIIIIIIIIIAAAAIAIIIIIIIIIIIIIIIIIIIIIIIIIIAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "????????????????????????????????????????????????KKKKKKKKKK???????KKKKKKJJJJJJJJJJJJJJJJJJJJ?????????????J???????????????????????",
        "?????????????????????????????????????????????????????????????????MMMMMMMMMMMMMMMMMMMMMMMMMM????M?MMMMMMMMMMMMMMMMMMMMMMMMMM????M",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALAAAAAAAAMMMMMMMMMMAAAAAAAMMMMMMMMMMMMMMMMMMMMMMMMMMAAAAMAMMMMMMMMMMMMMMMMMMMMMMMMMMAAAAM",
        "*NNNNNNNNN*NN*NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN",
        "?OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOPOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO",
        "?OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOBOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSAAAAAAARRRRRRRRRRRRRRRRRRRRRRRRRRAAAAAARRRRRRRRRRRRRRRRRRRRRRRRRRAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFATAAAAAAAAAAASSSSSSSSSSAAAAAAASSSSSSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAASAAAAAAAAAAAAAAAAAAAAAAA",
        "????????????????????????????????????????????????UUUUUUUUUU??????????????????????????????????????????????????????????????????????",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFAGAAAAAAAAAAAUUUUUUUUUUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "---------------------------------------------Q---------------Q------------------------------------------------------------------",
        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!EEEEEEEEEE!!!D!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!^!![[[[[[[[[[!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA[[[[[[[[[[AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA^AAAAAAAAAAAA_AAAAAAAAAAAAA",
        "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA]]]]]]]]]]AAAA?A???????????????????????????AAAA???????????????????_????????AAAA?",
        "???????????????????????????????????????????]?]??]]]]]]]]]]??????????????????????????????????????????????????????????????????????",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "?????????????????????????????????????????????????????????????????aaaaaaaaaaaaaaaaaaaaaaaaaa????a?aaaaaaaaaaaaaaaaaaaaaaaaaa????a",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA`AAAAAAAAaaaaaaaaaaAAAAAAAaaaaaaaaaaaaaaaaaaaaaaaaaaAAAAaAaaaaaaaaaaaaaaaaaaaaaaaaaaAAAAa",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
};

// --- SourceReader ---

SourceReader::SourceReader(int tabSize, UStrReader* reader)
   : TextParser(source_dfa, tabSize, reader), _operatorMode(false)
{

}

ustr_t SourceReader :: copyToken(char* token, size_t length)
{
   size_t tokenLen = _position - _startPosition;

   StrConvertor::copy(token, _line + _startPosition, tokenLen, length);
   token[length] = 0;

   return token;
}

ustr_t SourceReader :: copyQuote(char* token, size_t length, List<char*, freestr>& dynamicStrings)
{
   size_t tokenLen = _position - _startPosition;
   if (tokenLen > length) {
      char* str = StrFactory::allocate(tokenLen + 1, DEFAULT_STR);

      StrConvertor::copy(str, _line + _startPosition, tokenLen, tokenLen);
      str[tokenLen] = 0;

      dynamicStrings.add(str);

      return str;
   }
   else return copyToken(token, length);
}

SourceInfo SourceReader :: read(char* line, size_t length, List<char*, freestr>& dynamicStrings)
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
      case dfaMinusLookahead:
         resolveSignAmbiguity(info);
         break;
      case dfaDotLookahead:
         resolveDotAmbiguity(info);
         break;
      default:
         // to make compiler happy
         break;
   }

   if (IsQuoteToken(info.state)) {
      info.symbol = copyQuote(line, length, dynamicStrings);
   }
   else info.symbol = copyToken(line, length);

   _operatorMode = IsOperator(info.state);

   return info;
}

