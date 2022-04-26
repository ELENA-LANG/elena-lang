//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Win32 EditFrame container File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wintextframe.h"
#include "elena.h"

using namespace elena_lang;

// --- TextViewFrame ---

TextViewFrame::TextViewFrame(bool withAbovescore, ControlBase* view, TextViewModel* model)
   : MultiTabControl(withAbovescore, view)
{
   _model = model;

   model->attachListener(this);
}

void TextViewFrame :: onDocumentView(int index)
{
   WideMessage title(_model->getDocumentName(index));

   addTabView(*title, nullptr);
}

void TextViewFrame :: onDocumentViewSelect(int index)
{

}
