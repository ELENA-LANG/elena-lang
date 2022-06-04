//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Status Bar Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windows/winidestatusbar.h"

using namespace elena_lang;

int StatusBarWidths[5] = { 200, 120, 80, 60, 80 };


// --- IDEStatusBar --

IDEStatusBar :: IDEStatusBar(IDEModel* model)
   : StatusBar(5, StatusBarWidths), _model(model)
{
   _model->sourceViewModel.attachDocListener(this);
   _model->attachListener(this);

   _pendingIDESettings = true;
}

void IDEStatusBar :: setRectangle(Rectangle rec)
{
   StatusBar::setRectangle(rec);

   if (_pendingIDESettings) {
      onIDEChange();

      _pendingIDESettings = false;
   }
}

void IDEStatusBar :: onIDEChange()
{
   switch (_model->status) {
      case IDEStatus::Ready:
         setText(0, _T(" Ready"));
         break;
      case IDEStatus::Busy:
         setText(0, _T(" Busy"));
         break;
      case IDEStatus::AutoRecompiling:
         setText(0, _T(" Recompiling..."));
         break;
      default:
         break;
   }
}

void IDEStatusBar :: onDocumentUpdate()
{

}