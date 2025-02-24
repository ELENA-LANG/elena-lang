//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux EditFrame container File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtktextframe.h"

using namespace elena_lang;

// --- TextViewFrame ---
TextViewFrame :: TextViewFrame(TextViewModel* model, TextViewControllerBase* controller, ViewStyles* styles)
   : TabBar()
{
   _model = model;
   _styles = styles;
   _controller = controller;

   _model->attachListener(this);
}

void TextViewFrame :: onDocumentNew(int index)
{
   TextViewWindow* view = new TextViewWindow(_model, _controller, _styles);
   //textView->textview_changed().connect(sigc::mem_fun(*this,
   //           &EditFrame::on_textview_changed));

//   textView->setStyles(STYLE_MAX + 1, _schemes[_scheme], 15, 20);
//   textView->applySettings(_model->tabSize, _model->tabCharUsing, _model->lineNumberVisible, _model->highlightSyntax);
//
//   textView->setDocument(document);
//
//   textView->show(); // !! temporal?

   ustr_t title = _model->getDocumentName(index);
   addTab(title, view);
}

void TextViewFrame :: onDocumentSelect(int index)
{
}

void TextViewFrame :: onDocumentModeChanged(int index, bool modifiedMode)
{
}

void TextViewFrame :: beforeDocumentClose(int index)
{
}

void TextViewFrame :: onDocumentClose(int index, bool empty)
{
}
