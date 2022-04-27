//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windows/winide.h"
#include <windows/Resource.h>

using namespace elena_lang;

void IDEWindow :: onActivate()
{
   auto center = _layoutManager.getCenter();
   if (center)
      center->setFocus();
}

bool IDEWindow :: onCommand(int command)
{
   switch (command) {
      case IDM_FILE_NEW:
         _controller->doNewFile(_model);
         break;
      case IDM_DEBUG_RUN:
         _controller->projectController.doDebugAction(_model->projectModel, DebugAction::Run);
         break;
      case IDM_DEBUG_STEPOVER:
         _controller->projectController.doDebugAction(_model->projectModel, DebugAction::StepOver);
         break;
      case IDM_DEBUG_STEPINTO:
         _controller->projectController.doDebugAction(_model->projectModel, DebugAction::StepInto);
         break;
      default:
         return false;
   }

   return true;
}

void IDEWindow :: onModelChange(ExtNMHDR* hdr)
{
   auto docView = _model->sourceViewModel.DocView();

   switch (hdr->extParam) {
      case NOTIFY_SOURCEMODEL:
         docView->notifyOnChange();
         break;
      case NOTIFY_CURRENTVIEW_CHANGED:
         _model->sourceViewModel.onDocumentSelected(hdr->extParam);
         _model->sourceViewModel.onModelChanged();
         break;
      default:
         break;
   }
   
}

void IDEWindow :: onTabSelChanged(HWND wnd)
{
   for (size_t i = 0; i < _childCounter; i++) {
      if (_children[i]->checkHandle(wnd)) {
         _children[i]->onSelChanged();
         break;
      }
   }
}


void IDEWindow :: onNotify(NMHDR* hdr)
{
   switch (hdr->code) {
      case NMHDR_Model:
         onModelChange((ExtNMHDR*)hdr);
         break;
      case TCN_SELCHANGE:
         onTabSelChanged(hdr->hwndFrom);
         break;
      default:
         break;
   }
}
