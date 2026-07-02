//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Win32 EditFrame container File
//                                             (C)2021-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <windowsx.h>
//---------------------------------------------------------------------------
#include "wintextframe.h"
#include "elena.h"

using namespace elena_lang;

// --- TextViewFrame ---

TextViewFrame :: TextViewFrame(NotifierBase* notifier, bool withAbovescore, bool withHighlighting, ControlBase* view,
   TextViewModel* model, SelectionEventInvoker invoker, int closeCommandId, int closeIcon, int activeCloseIcon)
   : MultiTabControl(notifier, withAbovescore, withHighlighting, view, closeIcon, activeCloseIcon)
{
   _selectionInvoker = invoker;
   _model = model;
   _closeCommandId = closeCommandId;

   model->attachListener(this);
}

void TextViewFrame :: onDocumentNew(int index)
{
   WideMessage title(_model->getDocumentName(index));

   addTabView(*title, nullptr);
}

void TextViewFrame :: onDocumentSelect(int index)
{
   selectTab(index - 1);
}

//void TextViewFrame :: afterDocumentSelect(int index)
//{
//   _child->show();
//   _child->refresh();
//}

void TextViewFrame :: beforeDocumentClose(int index)
{

}

void TextViewFrame :: onDocumentClose(int index, bool empty)
{
   eraseTabView(index - 1);

   if (empty)
      _child->hide();
}

//void TextViewFrame :: onDocumentRename(int index)
//{
//   WideMessage title(_model->getDocumentName(index));
//
//   renameTabView(index - 1, *title);
//}

void TextViewFrame :: onDocumentModeChanged(int index, bool modifiedMode)
{
   WideMessage title(_model->getDocumentName(index), modifiedMode ? "*" : "");

   if (index == -1)
      index = getCurrentIndex() + 1;

   renameTabView(index - 1, *title);
}

void TextViewFrame :: onSelChanged()
{
   int index = getCurrentIndex();
   if (index >= 0) {
      _model->selectDocumentView(index + 1);

      if (_selectionInvoker) {
         _selectionInvoker(_notifier, index);
      }
   }
   else {
      _child->hide();
      _notSelected = true;

      _model->clearDocumentView();
   }
}

void TextViewFrame :: onClick(NMHDR* hdr)
{
   DWORD dwpos = ::GetMessagePos();
   Point p(GET_X_LPARAM(dwpos), GET_Y_LPARAM(dwpos));

   if (isOverButton(&p)) {
      AppCommandEvent appCommand(_closeCommandId);

      _notifier->notify(&appCommand);
   }
}
