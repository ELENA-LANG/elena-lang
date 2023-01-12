//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     SourceViewModel implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "editframe.h"

using namespace elena_lang;

// --- SourceViewModel ---

SourceViewModel :: SourceViewModel()
   : TextViewModel()
{
   traceRow = errorRow = -1;
}

void SourceViewModel :: beforeDocumentSelect(int index)
{
   if (errorRow != -1)
      clearErrorLine();
}

void SourceViewModel :: setTraceLine(int row, bool withCursor)
{
   DocumentChangeStatus status = {};

   if (traceRow != -1) {
      _currentView->removeMarker(traceRow, STYLE_TRACE_LINE, status);
   }

   _currentView->addMarker(row, STYLE_TRACE_LINE, false, status);
   if (withCursor)
      _currentView->setCaret({ 0, row - 1 }, false, status);

   traceRow = row;

   _currentView->notifyOnChange(status);
}

void SourceViewModel :: clearTraceLine()
{
   DocumentChangeStatus status = {};

   if (traceRow != -1) {
      _currentView->removeMarker(traceRow, STYLE_TRACE_LINE, status);
   }
   traceRow = -1;

   _currentView->notifyOnChange(status);
}

void SourceViewModel :: setErrorLine(int row, int column, bool withCursor)
{
   DocumentChangeStatus status = {};

   if (errorRow != -1) {
      _currentView->removeMarker(errorRow, STYLE_ERROR_LINE, status);
   }

   _currentView->addMarker(row, STYLE_ERROR_LINE, true, status);
   if (withCursor)
      _currentView->setCaret({ column - 1, row - 1 }, false, status);

   errorRow = row;

   _currentView->notifyOnChange(status);
}

void SourceViewModel :: clearErrorLine()
{
   DocumentChangeStatus status = {};

   if (errorRow != -1) {
      _currentView->removeMarker(errorRow, STYLE_ERROR_LINE, status);

      _currentView->notifyOnChange(status);
   }
   errorRow = -1;
}
//
//void SourceViewModel :: onModelChanged()
//{
//   if (errorRow != -1)
//      clearErrorLine();
//}
