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
const text_c lexNamedOperator = 'm';

const text_c lexResolvedDefault = 'A';
const text_c lexTerminal = 'Z';

const int CODE_QUERY_MODE = 1;
const int CODE_MODE = 2;
const int CODE_OP_MODE = 0x04;
const int POSTFIX_MODE = 0x08;
const int COMPLEX_MODE = 0x10;

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
     _T("AAAAAAAAAaaAAaAAAAAAAAAAAAAAAAAAadhomAdAdddddddfeeeeeeeeeeSCTTddAbbbbbbbbbbbbbbbbbbbbbbbbbbddddbAGGGGGGGGGGGGGGGGGGGGGGGGGGEdFdb"),
     _T("AAAAAAAAAJJAAJAAAAAAAAAAAAAAAAAAJKAAAAKAIHKKKKKbbbbbbbbbbbSCKKKKAbbbbbbbbbbbbbbbbbbbbbbbbbbKKKKbAbbbbbbbbbbbbbbbbbbbbbbbbbbEKFKb"),
     _T("AAAAAAAAAccAAAAAAAAAAAAAAAAAAAAAcKAAQAKAIHKKKKKQAAAAAAAAAASAKKKKABBBBBBBBBBBBBBBBBBBBBBBBBBKKKKBABBBBBBBBBBBBBBBBBBBBBBBBBBEKFKB"),
     _T("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDdDDDDdDIHdddddDDDDDDDDDDDdCddddDDDDDDDDDDDDDDDDDDDDDDDDDDDddddDDDDDDDDDDDDDDDDDDDDDDDDDDDDEdFdD"),
     _T("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLHKKKKKLeeeeeeeeeeLCLLLLLeeeeeeLLLLLLLLLLLLLLLLLLLLKLKLLLLLLLLLLeLLLeLLLLLeLLLLLLLLELFLL"),
     _T("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDjDDDDgDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"),
     _T("MgggggggggMggMgggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg"),
     _T("hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhihhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"),
     _T("NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNhNNNNNNNNNNNNNNNNNNNNNNNNCNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNENFNN"),
     _T("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjkjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"),
     _T("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjljjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"),
     _T("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"),
     _T("PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPnnnnnnnnnnPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPmmmmmmmmmmmmmmmmmmmmmmmmmmPPPPP"),
     _T("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOnnnnnnnnnnOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"),
     _T("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRooooooooooooooooooooooooooRRRRR"),
};

const pos_t styleMapping[] =
{
   STYLE_DEFAULT, STYLE_DEFAULT, STYLE_DEFAULT, STYLE_OPERATOR, STYLE_NUMBER, STYLE_DEFAULT, STYLE_DEFAULT, STYLE_DEFAULT,
   STYLE_STRING, STYLE_DEFAULT, STYLE_DEFAULT, STYLE_DEFAULT, STYLE_OPERATOR, STYLE_DEFAULT, STYLE_DEFAULT,
};

const size_t operation_keywords_len = 6;
const text_c* operation_keywords[] = { _T("else"), _T("for"), _T("if"), _T("if:not"), _T("try"), _T("while") };

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
   return styleMapping[state - lexStart];

   //pos_t retVal = state == lexOperator ? STYLE_OPERATOR : STYLE_DEFAULT;
   //retVal = state == lexNamedOperator ? STYLE_OPERATOR : retVal;
   //retVal = state == lexString ? STYLE_STRING : retVal;

   //return state == lexDigit ? STYLE_NUMBER : retVal;
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
   else if (test(info.context.mode, COMPLEX_MODE)) {
      info.context.mode &= ~COMPLEX_MODE;
      info.context.bufLen--;
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
   info.state = lexKeyword;

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

inline static bool doublecolonState(FormatterInfo& info, text_c ch)
{
   bool retVal = false;
   if (test(info.context.mode, CODE_OP_MODE)) {
      info.context.mode |= COMPLEX_MODE;

      retVal = operatorState(info, ch);

      info.style = STYLE_KEYWORD;
   }
   else retVal = operatorState(info, ch);

   return retVal;
}

inline static bool equalScopeState(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.style = defineStyle(info.state);
   info.state = lexOperator;
   info.context.mode &= ~POSTFIX_MODE;

   return retVal;
}

inline static bool postfixStart(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.style = defineStyle(info.state);
   info.state = lexOperator;
   info.context.mode |= POSTFIX_MODE;

   return retVal;
}

inline static bool semicolonScope(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.context.mode &= ~(CODE_QUERY_MODE | POSTFIX_MODE);
   info.style = defineStyle(info.state);
   info.state = lexOperator;

   return retVal;
}

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

   info.style = defineStyle(info.state);
   info.state = lexOperator;
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

inline static bool openingBracketScope(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.style = test(info.context.mode, POSTFIX_MODE) ? STYLE_META : defineStyle(info.state);
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

inline static bool curlyBracketsOpeningScope(FormatterInfo& info, text_c)
{
   bool retVal = info.state != lexOperator;

   info.context.mode &= ~POSTFIX_MODE;
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

   if (test(info.context.mode, CODE_OP_MODE) && info.context.bufLen > 1) {
      //if (info.context.buffer[info.context.bufLen - 1] == '(')
      //   info.context.bufLen--;

      info.context.buffer[info.context.bufLen] = 0;

      info.style = binarySearchKeywords(info.context.buffer) ? STYLE_KEYWORD : STYLE_DEFAULT;
      info.context.bufLen = 0;
   }
   else info.style = STYLE_DEFAULT;

   info.context.argument = makeStep;
   info.state = lexOperator;
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

inline static bool stringStyle(FormatterInfo& info, text_c ch)
{
   info.style = STYLE_STRING;
   info.state = lexStart;
   SourceFormatter::repeat(ch, info);

   return true;
}

inline static bool charStyle(FormatterInfo& info, text_c ch)
{
   info.state = lexStart;
   SourceFormatter::repeat(ch, info);

   info.style = STYLE_STRING;

   return true;
}

inline static bool namedOperatorStyle(FormatterInfo& info, text_c ch)
{
   info.state = lexStart;
   SourceFormatter::repeat(ch, info);
   info.style = STYLE_OPERATOR;

   return true;
}

inline static bool identifierStyle(FormatterInfo& info, text_c ch)
{
   info.style = STYLE_DEFAULT;
   info.state = lexStart;
   SourceFormatter::repeat(ch, info);

   return true;
}

inline static bool directiveStyle(FormatterInfo& info, text_c ch)
{
   info.style = STYLE_OPERATOR;
   info.state = lexStart;
   SourceFormatter::repeat(ch, info);

   return true;
}

Resolver scopeResolver[] = { 
   defaultStyle, keywordStyle, semicolonScope, operatorStyle, curlyBracketsOpeningScope, curlyBracketsClosingScope, startLineScope,
   closingBracketScope, openingBracketScope, spaceState, operatorState, digitStyle, lineCommentStyle, stringStyle, charStyle,
   namedOperatorStyle, identifierStyle, directiveStyle, postfixStart, equalScopeState,
};

Resolver codeResolver[] = { 
   defaultStyle, keywordStyle, semicolonCode, operatorStyle, curlyBracketsOpening, curlyBracketsClosing, startLineCode, 
   closingBracket, openingBracket, spaceState, operatorState, digitStyle, lineCommentStyle, stringStyle, charStyle,
   namedOperatorStyle, identifierStyle, directiveStyle, doublecolonState, operatorState,
};

// --- SourceFormatter ---

void SourceFormatter :: start(FormatterInfo& info)
{
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

   return retVal;
}
