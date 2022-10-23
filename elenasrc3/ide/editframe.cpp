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

void SourceViewModel :: setTraceLine(int row, bool withCursor)
{
   if (traceRow != -1) {
      _currentView->removeMarker(traceRow, STYLE_TRACE_LINE);
   }

   _currentView->addMarker(row, STYLE_TRACE_LINE, false);
   if (withCursor)
      _currentView->setCaret(0, row - 1, false);

   traceRow = row;
}

void SourceViewModel :: clearTraceLine()
{
   if (traceRow != -1) {
      _currentView->removeMarker(traceRow, STYLE_TRACE_LINE);
   }
   traceRow = -1;
}

void SourceViewModel :: setErrorLine(int row, int column, bool withCursor)
{
   if (errorRow != -1) {
      _currentView->removeMarker(errorRow, STYLE_ERROR_LINE);
   }

   _currentView->addMarker(row, STYLE_ERROR_LINE, true);
   if (withCursor)
      _currentView->setCaret(column - 1, row - 1, false);

   errorRow = row;

}
void SourceViewModel::clearErrorLine()
{
   if (errorRow != -1) {
      _currentView->removeMarker(errorRow, STYLE_ERROR_LINE);

      _currentView->notifyOnChange();
   }
   errorRow = -1;
}

void SourceViewModel :: onModelChanged()
{
   if (errorRow != -1)
      clearErrorLine();
}
