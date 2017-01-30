//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Document class implementation
//                                              (C)2011-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "sourcedoc.h"

using namespace _GUI_;

// --- SourceDoc ---

SourceDoc::IndentDirection SourceDoc :: IsAutoIndent(text_c ch)
{
   if (ch == '[' || ch == '{')
      return idRight;
   else if (ch == ']' || ch == '}') {
      return idLeft;
   }
   else return idNone;
}

text_c SourceDoc::getCurrentChar()
{
   size_t length = 0;
   text_t line = NULL;

   TextBookmark bm = _caret;
   // return current or previous if EOL
   if (!bm.isEOL()) {
      line = _text->getLine(bm, length);
   }
   else {
      if (bm.moveOn(-1))
         line = _text->getLine(bm, length);
   }

   return (length > 0) ? line[0] : 0;
}

text_t SourceDoc :: getCurrentLine(disp_t disp, size_t& length)
{
   TextBookmark bm = _caret;
   bm.moveTo(0, _caret.getRow());
   if (disp == 0 || bm.moveOn(disp)) {
      return _text->getLine(bm, length);
   }
   else {
      length = 0;

      return NULL;
   }
}

void SourceDoc :: insertNewLine()
{
   disp_t disp = 0;
   size_t length = 0;
   text_t line = getCurrentLine(0, length);
   while (length > 0) {
      IndentDirection dir = IsAutoIndent(getCurrentChar());
      for (size_t i = 0 ; i < length ; i++) {
         if (line[i] != 0x20 && line[i] != 0x9) {
            disp += i;

            if (dir == idRight)
               disp += Text::TabSize;

            Document::insertNewLine();
            Document::insertChar(' ', disp);

            return;
         }
      }
      disp += length;

      line = getCurrentLine(disp, length);
   }
   // if it is the end of the file or an empty line
   Document::insertNewLine();
}
