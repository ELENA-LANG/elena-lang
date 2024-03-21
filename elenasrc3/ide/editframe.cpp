//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     SourceViewModel implementation File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "editframe.h"

using namespace elena_lang;

// --- SourceViewModel ---

SourceViewModel :: SourceViewModel()
   : TextViewModel()
{
}

void SourceViewModel :: setTraceLine(int row, bool withCursor, DocumentChangeStatus& status)
{
   _currentView->removeMarker(STYLE_TRACE_LINE, status);

   _currentView->addMarker(row, STYLE_TRACE_LINE, false, false, status);
   if (withCursor)
      _currentView->setCaret({ 0, row - 1 }, false, status);
}

void SourceViewModel :: clearTraceLine(DocumentChangeStatus& status)
{
   if (_currentView != nullptr) {
      _currentView->removeMarker(STYLE_TRACE_LINE, status);
   }
}

void SourceViewModel :: setErrorLine(int row, int column, bool withCursor, DocumentChangeStatus& status)
{
   _currentView->removeMarker(STYLE_ERROR_LINE, status);

   _currentView->addMarker(row, STYLE_ERROR_LINE, true, false, status);
   if (withCursor)
      _currentView->setCaret({ column - 1, row - 1 }, false, status);
}

void SourceViewModel :: clearErrorLine(DocumentChangeStatus& status)
{
   if(_currentView->removeMarker(STYLE_ERROR_LINE, status)) {
      status.formatterChanged = true;
   }
}

void SourceViewModel :: refresh(DocumentChangeStatus& status)
{
   if (status.textChanged || status.caretChanged) {
      clearErrorLine(status);
      status.formatterChanged = true;
   }
         
   TextViewModel::refresh(status);
}

void SourceViewModel :: clearDocumentView()
{
   TextViewModel :: clearDocumentView();
}
