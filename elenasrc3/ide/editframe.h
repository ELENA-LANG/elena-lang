//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     SourceViewModel header File
//                                             (C)2021-2023, by Aleksey Rakov
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
   public:
      void beforeDocumentSelect(int index) override;

      void notifyOnChange(DocumentChangeStatus& status) override;

      void setTraceLine(int row, bool withCursor);
      void clearTraceLine();

      void setErrorLine(int row, int column, bool withCursor);
      void clearErrorLine();

      void clearDocumentView() override;

      SourceViewModel();
   };
}

#endif

