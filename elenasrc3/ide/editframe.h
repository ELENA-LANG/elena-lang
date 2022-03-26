//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     SourceViewModel header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef EDITFRAME_H
#define EDITFRAME_H

#include "guicommon.h"
#include "view.h"

namespace elena_lang
{
   // --- SourceViewModel ---
   class SourceViewModel : public TextViewModel
   {
   protected:
      int traceRow;

   public:
      void setTraceLine(int row);
      void clearTraceLine();

      SourceViewModel();
   };
}

#endif

