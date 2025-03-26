//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux TextView container declaration
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKIDETEXTFRAME_H
#define GTKIDETEXTFRAME_H

#include "gtklinux/gtktextframe.h"
#include "idecommon.h"

namespace elena_lang
{

class IDETextViewFrame : public TextViewFrame
{
   BroadcasterBase* _eventBroadcaster;

protected:
    void onTabChange(int page_num) override;

public:
   void on_text_model_change(TextViewModelEvent event);

   IDETextViewFrame(TextViewModel* model, TextViewControllerBase* controller, ViewStyles* styles, BroadcasterBase* eventBroadcaster);
};

} // _GUI_

#endif // winideH
