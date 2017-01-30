//---------------------------------------------------------------------------
//                E L E N A P r o j e c t: ELENA IDE
//                      SourceDoc class header
//                                             (C)2011-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef sourcedocH
#define sourcedocH

#include "document.h"

namespace _GUI_
{

class SourceDoc : public Document
{
   enum IndentDirection
   {
      idNone = 0,
      idLeft,
      idRight
   };

   IndentDirection IsAutoIndent(text_c ch);

   text_c getCurrentChar();
   text_t getCurrentLine(disp_t disp, size_t& length);

public:
   virtual void insertNewLine();

   SourceDoc(Text* text, LexicalStyler* styler, int encoding)
      : Document(text, styler, encoding)
   {
   }
};

} // _GUI_

#endif // sourcedocH
