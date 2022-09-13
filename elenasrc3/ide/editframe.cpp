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
   traceRow = -1;
}

void SourceViewModel :: setTraceLine(int row, bool withCursor)
{
   if (traceRow != -1) {
      _currentView->removeMarker(traceRow, STYLE_TRACE_LINE);
   }

   _currentView->addMarker(row, STYLE_TRACE_LINE);
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

