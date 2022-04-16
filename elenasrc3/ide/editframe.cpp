//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     SourceViewModel implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "editframe.h"

using namespace elena_lang;

// --- SourceViewModel ---

SourceViewModel :: SourceViewModel(int fontSize)
   : TextViewModel(fontSize)
{
   traceRow = -1;
}

void SourceViewModel :: setTraceLine(int row)
{
   if (traceRow != -1) {
      docView->removeMarker(traceRow, STYLE_TRACE_LINE);
   }

   docView->addMarker(row, STYLE_TRACE_LINE);
   traceRow = row;
}

void SourceViewModel :: clearTraceLine()
{
   if (traceRow != -1) {
      docView->removeMarker(traceRow, STYLE_TRACE_LINE);
   }
   traceRow = -1;
}

