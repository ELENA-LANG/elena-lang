//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Status Bar Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <tchar.h>

#include "windows/winidestatusbar.h"

using namespace elena_lang;

int StatusBarWidths[5] = { 200, 120, 80, 60, 80 };


// --- IDEStatusBar --

IDEStatusBar :: IDEStatusBar(IDEModel* model)
   : StatusBar(5, StatusBarWidths), _model(model)
{
   setIDEStatus(IDEStatus::Empty);

   //_pendingIDESettings = true;
}

void IDEStatusBar :: onDocumentUpdate(DocumentChangeStatus& changeStatus)
{
   if (changeStatus.caretChanged) {
      auto docView = _model->viewModel()->DocView();
      auto caret = docView->getCaret();

      String<text_c, 30> line;
      line.append(_T("Ln "));
      line.appendInt(caret.y + 1);
      line.append(_T(" Col "));
      line.appendInt(caret.x + 1);

      setText(1, line.str());
   }
}

void IDEStatusBar :: setRectangle(Rectangle rec)
{
   StatusBar::setRectangle(rec);

//   if (_pendingIDESettings) {
      //onIDEChange();

      //_pendingIDESettings = false;
   //}
}

void IDEStatusBar :: setIDEStatus(IDEStatus status)
{
   _status = status;

   switch (_model->status) {
      case IDEStatus::Empty:
         setText(0, _T(" Please open a new project or a file"));
         break;
      case IDEStatus::Ready:
         setText(0, _T(" Ready"));
         break;
      case IDEStatus::Compiling:
         setText(0, _T(" Compiling..."));
         break;
      case IDEStatus::Busy:
         setText(0, _T(" Busy"));
         break;
      case IDEStatus::AutoRecompiling:
         setText(0, _T(" Recompiling..."));
         break;
      case IDEStatus::CompiledSuccessfully:
         setText(0, _T(" Successfully compiled"));
         break;
      case IDEStatus::CompiledWithErrors:
         setText(0, _T(" Compiled with errors"));
         break;
      case IDEStatus::CompiledWithWarnings:
         setText(0, _T(" Compiled with warnings"));
         break;
      case IDEStatus::Broken:
         setText(0, _T(" The process was broken"));
         break;
      case IDEStatus::Running:
         setText(0, _T(" Running..."));
         break;
      case IDEStatus::Stopped:
         setText(0, _T(" Stopped"));
         break;
      default:
         break;
   }
}

void IDEStatusBar :: refresh()
{
   if (_status != _model->status) {
      setIDEStatus(_model->status);
   }

   ControlBase::refresh();
}
