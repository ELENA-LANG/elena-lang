//---------------------------------------------------------------------------
//                E L E N A P r o j e c t: ELENA IDE
//                      ELENA Document formatter implementations
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "sourceformatter.h"
#include "view.h"

using namespace elena_lang;

// --- Lexical DFA Table ---

const text_c lexStart = 'a';
const text_c lexCommentStart = 'b';
const text_c lexKeyword = 'c';
const text_c lexOperator = 'd';
const text_c lexBrackets = 'e';
const text_c lexObject = 'f';
const text_c lexCloseBracket = 'g';
const text_c lexStick = 'h';
const text_c lexDigit = 'i';
const text_c lexHint = 'j';
const text_c lexMessage = 'k';
const text_c lexLookahead = 'l';
const text_c lexLineComment = 'm';
const text_c lexComment = 'n';
const text_c lexComment2 = 'o';
const text_c lexQuote = 'p';
const text_c lexQuote2 = 'q';
const text_c lexHint2 = 'r';

const text_c* lexDFA[] =
{
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfjgdddddliiiiiiiiiiddddddacccccccccccccccccccccccccceaedcaccccccccccccccccccccccccccehedc"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpaaadfegdddddlffffffffffddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpaaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpaaadaegdddddliiiiiiiiiiddddddakkkkkkkkkkkkkkkkkkkkkkkkkkeaedkakkkkkkkkkkkkkkkkkkkkkkkkkkehedk"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpcaadfegdddddliiiiiiiiiiddddddaiiiiiikkkkkkkkkkkkkkkkkkkkeaedkaiiiiiikikkkikkkkkikkkkkkkkehedk"),
     _T("ajjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjrjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpcaadkegdddddlkkkkkkkkkkddddddakkkkkkkkkkkkkkkkkkkkkkkkkkeaedkakkkkkkkkkkkkkkkkkkkkkkkkkkehedk"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaddpcaadfagnddddmaaaaaaaaaaddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("ammmmmmmmmammammmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm"),
     _T("nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnonnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn"),
     _T("nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnbnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn"),
     _T("ppppppppppppppppppppppppppppppppppqppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpaaadaegdddddliiiiiiiiiiddddddakkkkkkkkkkkkkkkkkkkkkkkkkkeaedkakkkkkkkkkkkkkkkkkkkkkkkkkkehedk"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfjgddjddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
};

pos_t defineStyle(text_c state, pos_t style)
{
   switch (state) {
      case lexStart:
      case lexObject:
         return STYLE_DEFAULT;
      //case lexKeyword:
      //   return STYLE_KEYWORD;
      //case lexMessage:
      //   return STYLE_MESSAGE;
      //case lexOperator:
      //case lexBrackets:
      //case lexStick:
      //case lexCloseBracket:
      //case lexLookahead:
      //   return STYLE_OPERATOR;
      //case lexLineComment:
      //case lexComment:
      //case lexComment2:
      //case lexCommentStart:
      //case lexHint:                  // !! temporal, do hint needs its own style
      //case lexHint2:
      //   return STYLE_COMMENT;
      //case lexDigit:
      //   return STYLE_NUMBER;
      //case lexQuote:
      //case lexQuote2:
      //   return STYLE_STRING;
      default:
         return style;
   }
}

text_c makeStep(text_c ch, text_c state)
{
   return (unsigned)ch < 128 ? lexDFA[state - lexStart][ch] : lexDFA[state - lexStart][127];
}

// --- ELENADocFormatter ---

void ELENADocFormatter :: start(FormatterInfo& info)
{
   info.lookAhead = false;
   info.state = lexStart;
   info.style = STYLE_DEFAULT;
}

bool ELENADocFormatter :: next(text_c ch, FormatterInfo& info, pos_t& lastStyle)
{
   info.state = makeStep(ch, info.state);
   pos_t nextStyle = defineStyle(info.state, info.style);

   bool retVal = false;
   if (info.lookAhead) {
      info.style = nextStyle;
   }
   else if (info.style != nextStyle || info.state == lexLookahead) {
      lastStyle = info.style;
      info.style = nextStyle;
      retVal = true;
   }

   info.lookAhead = info.state == lexLookahead;

   return retVal;
}
