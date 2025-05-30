//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Source Reader class implementation.
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "source.h"

using namespace elena_lang;

const char* source_dfa[42] =
{
        ".????????BB??B??????????????????BdFIRCDLQQDbDVQHEEEEEEEEEEDDDDYc`CCCCCCCCCCCCCCCCCCCCCCCCCCDeQDC?CCCCCCCCCCCCCCCCCCCCCCCCCCDDDDC",
        "*********BB*********************B***********************************************************************************************",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALAAAAAAAACCCCCCCCCCAAAAAAACCCCCCCCCCCCCCCCCCCCCCCCCCAAAACACCCCCCCCCCCCCCCCCCCCCCCCCCAAAAC",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAADAADQDAAAAAAAAAAAAAAAAAAAAAAAAAAAAQQQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAA",
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
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAfAAAAAAAAAAAAASSSSSSSSSSAAAAAAARRRRRRRRRRRRRRRRRRRRRRRRRRAAAAAARRRRRRRRRRRRRRRRRRRRRRRRRRAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFATAAAAAAAAAAASSSSSSSSSSAAAAAAASSSSSSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAASAAAAAAAAAAAAAAAAAAAAAAA",
        "????????????????????????????????????????????????UUUUUUUUUU??????????????????????????????????????????????????????????????????????",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFATAAAAAAAAAAAUUUUUUUUUUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
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
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "ffffffffffffffffffffffffffffffffffhffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffgffff",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAfAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAfAiAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "????????????????????????????????????????????????jjjjjjjjjj??????????????????????????????????????????????????????????????????????",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAfAiAAAAAAAAAAAjjjjjjjjjjAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
};

// --- SourceReader ---

SourceReader::SourceReader(int tabSize, UStrReader* reader)
   : TextParser(source_dfa, tabSize, reader), _operatorMode(false), _interpolating(false), _bracketLevel(0)
{
   _startState = dfaStart;
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

inline char* copyInterpolQuote(char* dest, const char* src, pos_t start, size_t tokenLen, bool startingWithChar)
{
   if (startingWithChar) {
      if (src[start + tokenLen - 1] == '{')
         tokenLen--;

      if (src[start + tokenLen - 1] == '"')
         tokenLen--;

      StrConvertor::copy(dest, src + start, tokenLen, tokenLen);
   }
   else {
      if (src[start] != '"') {
         dest[0] = '"';
         StrConvertor::copy(dest + 1, src + start, tokenLen, tokenLen);
      }
      else StrConvertor::copy(dest, src + start, tokenLen, tokenLen);

      dest[tokenLen - 1] = '"';
   }

   dest[tokenLen] = 0;

   return dest;
}

ustr_t SourceReader :: copyInterpolQuote(char* token, size_t length, bool nextInterpolToken, List<char*, freestr>& dynamicStrings)
{
   pos_t start = _startPosition;

   bool startingWithChar = false;
   if (nextInterpolToken && _line[start] == '"' && _line[start + 1] == '$') {
      startingWithChar = true;
      start++;
   }
   else if (_line[start] == '$' || _line[start] == '{')
      start++;

   size_t tokenLen = _position - start;
   if (_line[start] != '"' && !startingWithChar)
      tokenLen++;

   if (tokenLen > length) {
      char* str = StrFactory::allocate(tokenLen + 1, DEFAULT_STR);
      ::copyInterpolQuote(str, _line, start, tokenLen, startingWithChar);

      dynamicStrings.add(str);

      return str;
   }
   else return ::copyInterpolQuote(token, _line, start, tokenLen, startingWithChar);
}

SourceInfo SourceReader :: read(char* line, size_t length, List<char*, freestr>& dynamicStrings)
{
   SourceInfo info;

   char terminalState = TextParser::read(_startState, info.state, info.lineInfo);
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

   switch (info.state) {
      case dfaQuote: 
      case dfaWideQuote:
      case dfaCharacter:
         info.symbol = copyQuote(line, length, dynamicStrings);
         break;
      case dfaAltQuote2:
      case dfaAltQuote:
         info.symbol = copyInterpolQuote(line, length, _startState == dfaNextInterpol, dynamicStrings);
         info.state = dfaStartInterpol;

         _startState = dfaStart;
         break;
      case dfaStartInterpol:
         _interpolating = true;
         _bracketLevel = 1;         
         info.symbol = copyInterpolQuote(line, length, _startState == dfaNextInterpol, dynamicStrings);

         _startState = dfaStart;
         break;
      default:
         info.symbol = copyToken(line, length);
         break;
   }

   _operatorMode = IsOperator(info.state);
   if (_operatorMode && _interpolating) {
      ustr_t op(line);

      if (op.compare("{")) {
         _bracketLevel++;
      }
      else if (op.compare("}")) {
         _bracketLevel--;
         if (!_bracketLevel) {
            _startState = dfaNextInterpol;
            _interpolating = false;

            info = read(line, length, dynamicStrings);
         }
      }
   }

   return info;
}

