//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     SourceViewModel header File
//                                             (C)2021-2025, by Aleksey Rakov
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
      void refresh(DocumentChangeStatus& status) override;

      void setTraceLine(int row, bool withCursor, DocumentChangeStatus& status);
      void clearTraceLine(DocumentChangeStatus& status);

      void setErrorLine(int row, int column, bool withCursors, DocumentChangeStatus& status);
      void clearErrorLine(DocumentChangeStatus& status);

      void clearDocumentView() override;

      SourceViewModel(TextViewSettings settings);
   };
}

#endif

