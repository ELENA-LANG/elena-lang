//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Win32 EditFrame container File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wintextframe.h"
#include "elena.h"

using namespace elena_lang;

// --- TextViewFrame ---

TextViewFrame :: TextViewFrame(NotifierBase* notifier, bool withAbovescore, ControlBase* view, TextViewModel* model, int selNotificationId)
   : MultiTabControl(notifier, withAbovescore, view)
{
   _selNotificationId = selNotificationId;
   _model = model;

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

void TextViewFrame :: onDocumentClose(int index)
{
   eraseTabView(index - 1);

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
      _model->selectDocumentViewByIndex(index + 1);

      if (_selNotificationId)
         _notifier->notifySelection(_selNotificationId, index);
   }
   else {
      _child->hide();
      _notSelected = true;

      _model->clearDocumentView();
   }
}
