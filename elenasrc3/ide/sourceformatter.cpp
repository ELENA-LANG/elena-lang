//---------------------------------------------------------------------------
//                E L E N A P r o j e c t: ELENA IDE
//                      ELENA Document formatter implementations
//                                             (C)2021-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "sourceformatter.h"
#include "view.h"

#ifdef _MSC_VER

#include <tchar.h>

#endif

using namespace elena_lang;

// --- Lexical DFA Table ---

const text_c lexStart = 'a';
const text_c lexKeyword = 'b';
const text_c lexSpace = 'c';
const text_c lexOperator = 'd';
const text_c lexDigit = 'e';
const text_c lexString = 'i';

const text_c lexResolvedDefault = 'A';
const text_c lexTerminal = 'Z';

const int CODE_QUERY_MODE = 1;
const int CODE_MODE = 2;
const int CODE_OP_MODE = 4;

//const text_c lexKeyword = 'd';
//const text_c lexObject = 'e';
//const text_c lexOperator = 'g';
//const text_c lexComment = 'j';
//const text_c lexComment2 = 'n';
//const text_c lexDigit = 'p';
//const text_c lexQuote = 's';
//const text_c lexChar = 'u';

/*
const text_c* lexDFA[] =
{
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaeqataaaeeeeeeehooooooooooeeeeeeebbbbbbbbbbbbbbbbbbbbbbbbbbeaeebabbbbbbbbbbbbbbbbbbbbbbbbbbeeeab"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaceaaaaaaeeeeeeeabbbbbbbbbbeeeeeeebbbbbbbbbbbbbbbbbbbbbbbbbbeaeebabbbbbbbbbbbbbbbbbbbbbbbbbbeeeab"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaceaaaaaaeeeeeeeaddddddddddeeeeeeeddddddddddddddddddddddddddeaeedaddddddddddddddddddddddddddeeead"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaeeeeeeeaaaaaaaaaaaeeeeeeebbbbbbbbbbbbbbbbbbbbbbbbbbeaeebabbbbbbbbbbbbbbbbbbbbbbbbbbaaaaa"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafaaaaaafffffffaaaaaaaaaaafffffffaaaaaaaaaaaaaaaaaaaaaaaaaafaffaaaaaaaaaaaaaaaaaaaaaaaaaaaafffaa"),
     _T("gggggggggggggggggggggggggggggggggfggggggfffffffgggggggggggfffffffggggggggggggggggggggggggggfgfgggggggggggggggggggggggggggggfffgg"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqataaaaaaaaaahooooooooooaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaagaaaaaaaaakaaaaigggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg"),
     _T("jiiiiiiiiijiijiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
     _T("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkklkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk"),
     _T("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkmkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk"),
     _T("nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
     _T("ppppppppppppppppppppppppppppppppppppppppppppppppoooooooooopppppppoooooopppppppppppppppppppppppppppppppppopppopppppoppppppppppppp"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafaaaaaafffffffaaaaaaaaaaaffffffaaaaaaaaaaaaaaaaaaaaaaaaaaafafaaaaaaaaaaaaaaaaaaaaaaaaaaaaafffaa"),
     _T("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqrqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"),
     _T("ssssssssssssssssssssssssssssssssssqstsssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafaaaaaafffffffhaaaaaaaaaaffffffaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafaaa"),
     _T("uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuqutuuuuuuuuuuuttttttttttuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
};
*/

const text_c* lexDFA[] =
{
     _T("AAAAAAAAAaaAAaAAAAAAAAAAAAAAAAAAaAhAAAAAddAddddfeeeeeeeeeedCdddAAbbbbbbbbbbbbbbbbbbbbbbbbbbdAdAbAGGGGGGGGGGGGGGGGGGGGGGGGGGEAFAb"),
     _T("AAAAAAAAAJJAAJAAAAAAAAAAAAAAAAAAJAAAAAAAIHAKKKKbbbbbbbbbbbKCKKKAAbbbbbbbbbbbbbbbbbbbbbbbbbbKAKAbAbbbbbbbbbbbbbbbbbbbbbbbbbbEAFAb"),
     _T("AAAAAAAAAccAAAAAAAAAAAAAAAAAAAAAcAAAAAAAIHAKKKKAAAAAAAAAAAKAKKKAABBBBBBBBBBBBBBBBBBBBBBBBBBKAKABABBBBBBBBBBBBBBBBBBBBBBBBBBEAFAB"),
     _T("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDIHDddddDDDDDDDDDDDdCdddDDDDDDDDDDDDDDDDDDDDDDDDDDDDdDdDDDDDDDDDDDDDDDDDDDDDDDDDDDDDEDFDD"),
     _T("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLHLKKKKLeeeeeeeeeeLCLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLKLKLLLLLLLLLLLLLLLLLLLLLLLLLLLLLELFLL"),
     _T("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDjDDDDgDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"),
     _T("MgggggggggMggMgggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg"),
     _T("hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhihhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"),
     _T("NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNhNNNNNNNNNNNNNNNNNNNNNNNNCNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNENFNN"),
     _T("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjkjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"),
     _T("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjljjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"),
     _T("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"),
};

const size_t operation_keywords_len = 2;
const text_c* operation_keywords[] = { _T("if"), _T("while") };

static bool binarySearchKeywords(text_str buffer) {
   int low = 0, high = operation_keywords_len - 1;
   while (low <= high) {
      int mid = low + (high - low) / 2;

      // Check if x is present at mid
      if (buffer.compare(operation_keywords[mid]))
         return true;

      // If x is smaller, ignore right half
      if (buffer.less(operation_keywords[mid]))         
         high = mid - 1;
      // If x greater, ignore left half
      else
         low = mid + 1;
   }

   // If we reach here, then element was
   // not present
   return false;
}

typedef bool(*Resolver)(FormatterInfo& info, text_c ch);
typedef text_c(*StepMaker)(text_c ch, FormatterInfo& info);

pos_t defineStyle(text_c state)
{
   pos_t retVal = state == lexOperator ? STYLE_OPERATOR : STYLE_DEFAULT;
   retVal = state == lexString ? STYLE_STRING : retVal;

   return state == lexDigit ? STYLE_NUMBER : retVal;
}

inline static text_c makeStep(text_c ch, FormatterInfo& info)
{
   return (unsigned)ch < 128 ? lexDFA[info.state - lexStart][ch] : lexDFA[info.state - lexStart][127];
}

inline static text_c makeStepWithStoring(text_c ch, FormatterInfo& info)
{
   if (info.context.bufLen < 10) {
      info.context.buffer[info.context.bufLen++] = ch;
   }

   return (unsigned)ch < 128 ? lexDFA[info.state - lexStart][ch] : lexDFA[info.state - lexStart][127];
}
//
//inline static text_c defineNextStep(text_c ch)
//{
//   return (unsigned)ch < 128 ? lexDFA[lexNextStep - lexStart][ch] : lexDFA[lexNextStep - lexStart][127];
//}

inline static bool startLineScope(FormatterInfo& info, text_c)
{
   info.state = lexKeyword;

   return false;
}

inline static bool startLineCode(FormatterInfo& info, text_c ch)
{
   info.state = lexKeyword;

   if (!test(info.context.mode, CODE_OP_MODE)) {
      info.context.buffer[0] = ch;
      info.context.bufLen = 1;
      info.context.argument = makeStepWithStoring;
      info.context.mode |= CODE_OP_MODE;
   }
   else info.context.argument = makeStep;

   return false;
}

inline static bool defaultStyle(FormatterInfo& info, text_c)
{
   info.style = STYLE_DEFAULT;
   info.state = lexStart;

   return true;
}

inline static bool keywordStyle(FormatterInfo& info, text_c)
{
   info.style = STYLE_KEYWORD;
   info.state = lexStart;

   info.context.argument = makeStep;

   return true;
}

inline static bool digitStyle(FormatterInfo& info, text_c)
{
   info.style = STYLE_NUMBER;
   info.state = lexStart;

   info.context.argument = makeStep;

   return true;
}

//inline static bool digitStyleOperator(FormatterInfo& info)
//{
//   info.style = STYLE_NUMBER;
//   info.state = lexOperator;
//
//   info.context.argument = makeStep;
//
//   return true;
//}

inline static bool operatorStyle(FormatterInfo& info, text_c ch)
{
   info.style = /*defineStyle(info.state)*/STYLE_OPERATOR;
   info.state = lexStart;
   SourceFormatter::repeat(ch, info);

   return true;
}

inline static bool operatorState(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.style = defineStyle(info.state);
   info.state = lexOperator;

   return retVal;
}

//inline static bool operatorDefaultStyle(FormatterInfo& info, text_c)
//{
//   info.style = /*defineStyle(info.state)*/STYLE_DEFAULT;
//   info.state = lexOperator;
//
//   return true;
//}

//inline static bool operatorKeywordStyle(FormatterInfo& info)
//{
//   info.style = STYLE_OPERATOR;
//   info.state = lexKeyword;
//
//   return true;
//}

inline static bool semicolonScope(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.context.mode &= ~CODE_QUERY_MODE;
   info.style = STYLE_DEFAULT;
   info.state = lexOperator;

   return retVal;
}

//inline static bool semicolonCode2(FormatterInfo& info)
//{
//   info.context.mode &= ~(CODE_QUERY_MODE | CODE_OP_MODE);
//   info.state = lexOperator;
//
//   return false;
//}

inline static bool semicolonCode(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.context.mode &= ~(CODE_QUERY_MODE | CODE_OP_MODE);
   info.style = defineStyle(info.state);
   info.state = lexOperator;

   return retVal;
}

inline static bool closingBracketScope(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.state = lexOperator;
   info.style = defineStyle(info.state);
   info.context.mode |= CODE_QUERY_MODE;

   return retVal;
}

inline static bool closingBracket(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.style = defineStyle(info.state);
   info.state = lexOperator;

   return retVal;
}

inline static bool openingBracket(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   if (test(info.context.mode, CODE_OP_MODE) && info.context.bufLen > 1) {
      if (info.context.buffer[info.context.bufLen - 1] == '(')
         info.context.bufLen--;

      info.context.buffer[info.context.bufLen] = 0;
      
      info.style = binarySearchKeywords(info.context.buffer) ? STYLE_KEYWORD : STYLE_DEFAULT;
      info.context.bufLen = 0;
   }
   else info.style = STYLE_DEFAULT;

   info.state = lexOperator;

   return retVal;
}

//inline static bool openingBracket2(FormatterInfo& info)
//{
//   if (test(info.context.mode, CODE_OP_MODE) && info.context.bufLen > 2) {
//      info.context.buffer[info.context.bufLen - 1] = 0;
//
//      info.style = binarySearchKeywords(info.context.buffer) ? STYLE_KEYWORD : STYLE_DEFAULT;
//      info.context.bufLen = 0;
//   }
//   else info.style = STYLE_DEFAULT;
//
//   info.state = lexOperator;
//
//   return true;
//}

inline static bool curlyBracketsOpeningScope(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   if (test(info.context.mode, CODE_QUERY_MODE)) {
      info.context.mode &= ~CODE_QUERY_MODE;
      info.context.mode |= CODE_MODE;
      info.context.level = 1;
   }

   info.state = lexOperator;
   info.style = STYLE_DEFAULT;

   return retVal;
}

inline static bool curlyBracketsOpening(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.context.argument = makeStep;
   info.state = lexOperator;
   info.style = /*defineStyle(info.state)*/STYLE_DEFAULT;
   info.context.mode &= ~CODE_OP_MODE;

   info.context.level++;

   return retVal;
}

inline static bool curlyBracketsClosingScope(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.state = lexOperator;
   info.style = defineStyle(info.state);

   info.context.level = 0;

   return retVal;
}

inline static bool curlyBracketsClosing(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.style = defineStyle(info.state);
   info.state = lexOperator;

   info.context.mode &= ~CODE_OP_MODE;

   info.context.level--;
   if (info.context.level <= 0) {
      info.context.mode &= ~CODE_MODE;
      info.context.level = 0;
   }

   return retVal;
}

inline static bool spaceState(FormatterInfo& info, text_c)
{
   info.state = lexSpace;
   info.context.argument = makeStep;
   info.context.bufLen--;

   return false;
}

inline static bool lineCommentStyle(FormatterInfo& info, text_c ch)
{
   info.style = STYLE_COMMENT;
   info.state = lexStart;
   SourceFormatter::repeat(ch, info);

   return true;
}

//inline static bool lineCommentNumberStyle(FormatterInfo& info)
//{
//   info.state = lexDigit;
//   info.style = STYLE_COMMENT;
//
//   return true;
//}

inline static bool stringStyle(FormatterInfo& info, text_c ch)
{
   info.style = STYLE_STRING;
   info.state = lexStart;
   SourceFormatter::repeat(ch, info);

   return true;
}

Resolver scopeResolver[] = { 
   defaultStyle, keywordStyle, semicolonScope, operatorStyle, curlyBracketsOpeningScope, curlyBracketsClosingScope, startLineScope,
/*   operatorDefaultStyle,*/ closingBracketScope, operatorState, spaceState, operatorState, //operatorDefaultStyle,
   digitStyle, /*digitStyleOperator, operatorKeywordStyle, */lineCommentStyle, stringStyle, //lineCommentNumberStyle,
};

Resolver codeResolver[] = { 
   defaultStyle, keywordStyle, semicolonCode, operatorStyle, curlyBracketsOpening, curlyBracketsClosing, startLineCode, 
/*   operatorDefaultStyle, */closingBracket, openingBracket, spaceState, operatorState, //openingBracket2,
   digitStyle, /*digitStyleOperator, operatorKeywordStyle,*/ lineCommentStyle, stringStyle, //lineCommentNumberStyle,
};

//pos_t defineStyle(text_c state, pos_t style)
//{
//   switch (state) {
//      case lexStart:
//      //case lexObject:
//         return STYLE_DEFAULT;
//      //case lexKeyword:
//      //   return STYLE_KEYWORD;
//      //case lexOperator:
//      //   return STYLE_OPERATOR;
//      //case lexComment:
//      //case lexComment2:
//      //   return STYLE_COMMENT;
//      //case lexDigit:
//      //   return STYLE_NUMBER;
//      //case lexQuote:
//      //case lexChar:
//      //   return STYLE_STRING;
//      default:
//         return INVALID_POS;
//   }
//}

// --- SourceFormatter ---

void SourceFormatter :: start(FormatterInfo& info)
{
   //info.lookAhead = false;
   info.state = lexStart;
   info.style = STYLE_DEFAULT;

   info.context.argument = &makeStep;
   info.context.mode = 0;
   info.context.level = 0;
   info.context.bufLen = 0;
}

void SourceFormatter :: repeat(text_c ch, FormatterInfo& info)
{
   text_c state = ((StepMaker)(info.context.argument))(ch, info);

   if (state <= lexTerminal) {
      Resolver* resolveMap = test(info.context.mode, CODE_MODE) ? codeResolver : scopeResolver;

      resolveMap[state - lexResolvedDefault](info, ch);
   }
   else info.state = state;
}

bool SourceFormatter :: next(text_c ch, FormatterInfo& info)
{
   text_c state = ((StepMaker)(info.context.argument))(ch, info);

   bool retVal = false;
   if (state <= lexTerminal) {
      Resolver* resolveMap = test(info.context.mode, CODE_MODE) ? codeResolver : scopeResolver;

      retVal = resolveMap[state - lexResolvedDefault](info, ch);
   }
   else info.state = state;

   //pos_t currentStyle = defineStyle(info.state, info.style);

   ///*if (info.lookAhead) {
   //   info.style = currentStyle;
   //}
   //else */if (currentStyle != INVALID_POS) {
   //   info.style = currentStyle;
   //   definedStyle = currentStyle;

   //   info.state = makeStep(ch, info.state);

   //   retVal = true;
   //}

   ////info.lookAhead = info.state == lexLookahead;

   return retVal;
}
