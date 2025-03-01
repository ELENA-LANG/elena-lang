//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux EditFrame container File
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKIDETEXTFRAME_H
#define GTKIDETEXTFRAME_H

namespace elena_lang
{

class IDETextViewFrame : public TextViewFrame
{
public:
   void on_text_model_change(TextViewModelEvent event);

   IDETextViewFrame();
};

} // _GUI_

#endif // winideH
