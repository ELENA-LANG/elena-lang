//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windows/winide.h"
#include "windows/windialogs.h"

#include <windows/Resource.h>

using namespace elena_lang;

void IDEWindow :: onActivate()
{
   auto center = _layoutManager.getCenter();
   if (center)
      center->setFocus();
}

void IDEWindow :: newFile()
{
   _controller->doNewFile(_model);
}

void IDEWindow :: openFile()
{
   FileDialog dialog(_instance, this, FileDialog::SourceFilter, OPEN_FILE_CAPTION, *_model->projectModel.paths.lastPath);

   _controller->doOpenFile(dialog, _model);
}

void IDEWindow :: saveFile()
{
   FileDialog dialog(_instance, this, FileDialog::SourceFilter, SAVEAS_PROJECT_CAPTION, *_model->projectModel.paths.lastPath);

   _controller->doSaveFile(dialog, _model, false);
}

bool IDEWindow :: onCommand(int command)
{
   switch (command) {
      case IDM_FILE_NEW:
         newFile();
         break;
      case IDM_FILE_OPEN:
         openFile();
         break;
      case IDM_FILE_SAVE:
         saveFile();
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
