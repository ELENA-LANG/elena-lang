//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux TextView container implementation
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtkidetextview.h"

using namespace elena_lang;

// --- IDETextViewFrame ---

IDETextViewFrame :: IDETextViewFrame(TextViewModel* model, TextViewControllerBase* controller, ViewStyles* styles)
   : TextViewFrame(model, controller, styles)
{

}

void IDETextViewFrame :: on_text_model_change(TextViewModelEvent event)
{
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
