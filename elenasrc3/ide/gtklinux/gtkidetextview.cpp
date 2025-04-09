//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux TextView container implementation
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtkidetextview.h"

using namespace elena_lang;

// --- IDETextViewFrame ---

IDETextViewFrame :: IDETextViewFrame(TextViewModel* model, TextViewControllerBase* controller, ViewStyles* styles,
BroadcasterBase* eventBroadcaster)
   : TextViewFrame(model, controller, styles)
{
   _eventBroadcaster = eventBroadcaster;
}

void IDETextViewFrame :: onDocumentModeChanged(int index, bool modifiedMode)
{
   IdentifierString title(_model->getDocumentName(index), modifiedMode ? "*" : "");

   if (index == -1)
      index = _model->getCurrentIndex();

   renameTab(index - 1, *title);
}

void IDETextViewFrame :: on_text_model_change(TextViewModelEvent event)
{
   if (test(event.status, STATUS_FRAME_CHANGED)) {
      int docIndex = _model->getCurrentIndex();
      if (docIndex > 0) {
         selectTab(docIndex - 1);
      }
   }

   if (event.changeStatus.modifiedChanged) {
      auto docInfo = _model->DocView();

      onDocumentModeChanged(-1, docInfo->isModified());
   }

   auto client = getCurrentTextView();
   if (client)
      client->onDocumentUpdate(event.changeStatus);

   if (test(event.status, STATUS_FRAME_VISIBILITY_CHANGED)) {
      if (client)
         client->grab_focus();

      //grab_focus();
   }

   //   if (_model->sourceViewModel.isAssigned()) {
         //_children[_model->ideScheme.textFrameId]->show();
         //_children[_model->ideScheme.textFrameId]->setFocus();
    //  }
      //else _children[_model->ideScheme.textFrameId]->hide();
   //}
}

void IDETextViewFrame :: onTabChange(int page_num)
{
   int index = getCurrentTabIndex();
   if (index >= 0) {
      _model->selectDocumentView(index + 1);

      SelectionEvent event(EVENT_TEXTFRAME_SELECTION_CHANGED, index);
      _eventBroadcaster->sendMessage(&event);

      //((TextView*)_child)->setDocument(_model->currentDoc);
      //_child->show();

      //refreshDocument();
   }
   //else if (getTabCount() == 0) {
      //_child->hide();
      //_notSelected = true;
      //_model->currentDoc = NULL;
   //}
}
