//---------------------------------------------------------------------------
//                E L E N A P r o j e c t: ELENA IDE
//                      ELENA Document formatter implementations
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "sourceformatter.h"
#include "view.h"

#ifdef _MSC_VER

#include <tchar.h>

#endif

using namespace elena_lang;

// --- Lexical DFA Table ---

const text_c lexStart = 'a';
const text_c lexKeyword = 'd';
const text_c lexObject = 'e';
const text_c lexOperator = 'g';
const text_c lexComment = 'j';
const text_c lexComment2 = 'n';
const text_c lexDigit = 'p';
const text_c lexQuote = 's';
const text_c lexChar = 'u';

const text_c* lexDFA[] =
{
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaeqataaaeeeeeeehooooooooooeeeeeeebbbbbbbbbbbbbbbbbbbbbbbbbbeaeebabbbbbbbbbbbbbbbbbbbbbbbbbbeeeab"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaceaaaaaaeeeeeeeabbbbbbbbbbeeeeeeebbbbbbbbbbbbbbbbbbbbbbbbbbeaeebabbbbbbbbbbbbbbbbbbbbbbbbbbeeeab"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaceaaaaaaeeeeeeeaddddddddddeeeeeeeddddddddddddddddddddddddddeaeedaddddddddddddddddddddddddddeeead"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaeeeeeeeaaaaaaaaaaaeeeeeeebbbbbbbbbbbbbbbbbbbbbbbbbbeaeebabbbbbbbbbbbbbbbbbbbbbbbbbbaaaaa"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafaaaaaafffffffaaaaaaaaaaafffffffaaaaaaaaaaaaaaaaaaaaaaaaaafaffaaaaaaaaaaaaaaaaaaaaaaaaaaaafffaa"),
     _T("gggggggggggggggggggggggggggggggggfggggggfffffffgggggggggggfffffffggggggggggggggggggggggggggfgfgggggggggggggggggggggggggggggfffgg"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaqataaaaaaaaaaaooooooooooaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
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
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafaaaaaafffffffaaaaaaaaaaaffffffaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaafaaa"),
     _T("uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuqutuuuuuuuuuuuttttttttttuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
};

pos_t defineStyle(text_c state, pos_t style)
{
   switch (state) {
      case lexStart:
      case lexObject:
         return STYLE_DEFAULT;
      case lexKeyword:
         return STYLE_KEYWORD;
      case lexOperator:
         return STYLE_OPERATOR;
      case lexComment:
      case lexComment2:
         return STYLE_COMMENT;
      case lexDigit:
         return STYLE_NUMBER;
      case lexQuote:
      case lexChar:
         return STYLE_STRING;
      default:
         return INVALID_POS;
   }
}

text_c makeStep(text_c ch, text_c state)
{
   return (unsigned)ch < 128 ? lexDFA[state - lexStart][ch] : lexDFA[state - lexStart][127];
}

// --- SourceFormatter ---

void SourceFormatter :: start(FormatterInfo& info)
{
   info.lookAhead = false;
   info.state = lexStart;
   info.style = STYLE_DEFAULT;
}

bool SourceFormatter :: next(text_c ch, FormatterInfo& info, pos_t& definedStyle)
{
   info.state = makeStep(ch, info.state);
   pos_t currentStyle = defineStyle(info.state, info.style);

   bool retVal = false;
   /*if (info.lookAhead) {
      info.style = currentStyle;
   }
   else */if (currentStyle != INVALID_POS) {
      info.style = currentStyle;
      definedStyle = currentStyle;

      info.state = makeStep(ch, info.state);

      retVal = true;
   }

   //info.lookAhead = info.state == lexLookahead;

   return retVal;
}
