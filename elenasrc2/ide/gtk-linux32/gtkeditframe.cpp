//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Gtk+ EditFrame Implementation
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkeditframe.h"
//#include "../appwindow.h"

using namespace _GUI_;

////static void ctrl_tab_pressed(GtkWidget* sender, gboolean shift, gpointer window)
////{
////   ((SDIWindow*)window)->_onMenu(shift ? IDM_WINDOW_PREVIOUS : IDM_WINDOW_NEXT);
////}

// -- Styles ---

StyleInfo defaultStyles[STYLE_MAX + 1] = {
   {Colour(0, 0, 0), Colour(1, 1, 1), "Monospace", 10, false, false},
   {Colour(0, 0, 0), Colour(0.84, 0.84, 0.84), "Monospace", 10, false, false},
	{Colour(0, 0, 0), Colour(0.75, 0.75, 0.75), "Monospace", 10, false, false},
	{Colour(0, 0, 1), Colour(1, 1, 1), "Monospace", 10, false, false},
   {Colour(0, 0.4, 0.5), Colour(1, 1, 1), "Monospace", 10, true, false},
	{Colour(0, 0, 0), Colour(1, 1, 1), "Monospace", 10, true, false},
	{Colour(1, 0.5, 0.4), Colour(1, 1, 1), "Monospace", 10, false, false},
	{Colour(0, 0.5, 0.5), Colour(1, 1, 1), "Monospace", 10, false, false},
   {Colour(0, 0.5, 0), Colour(1, 1, 1), "Monospace", 10, false, false},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
	{Colour(0.37, 0.37, 0.37), Colour(1, 1, 1), "Monospace", 10, true, false},
};

StyleInfo classicStyles[STYLE_MAX + 1] = {
	{Colour(1, 1, 0), Colour(0, 0, 0.5), "Monospace", 10, false, false},
	{Colour(0, 0, 0), Colour(0.84, 0.84, 0.84), "Monospace", 10, false, false},
	{Colour(0.37, 0.37, 0.37), Colour(1, 1, 1), "Monospace", 10, false, false},
	{Colour(0.7, 0.7, 0.7), Colour(0, 0, 0.5), "Monospace", 10, false, false},
	{Colour(1, 1, 1), Colour(0, 0, 0.5), "Monospace", 10, false, false},
	{Colour(1, 1, 0), Colour(0, 0, 0.5), "Monospace", 10, true, false},
	{Colour(0, 1, 0.5), Colour(0, 0, 0.5), "Monospace", 10, false, false},
	{Colour(0, 1, 1), Colour(0, 0, 0.5), "Monospace", 10, false, false},
   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), "Monospace", 10, false, false},
   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), "Monospace", 10, false, false},
	{Colour(1, 1, 0), Colour(0, 0, 0.5), "Monospace", 10, true, false},
	{Colour(0, 1, 0.5), Colour(0, 0, 0.5), "Monospace", 10, false, false},
	{Colour(0, 1, 1), Colour(0, 0, 0.5), "Monospace", 10, false, false},
   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), "Monospace", 10, false, false},
   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), "Monospace", 10, false, false}//,
};

// --- EditFrame ---

EditFrame :: EditFrame(Model* model/*SDIWindow* owner*/)
{
//   _owner = owner;
//   _currentDoc = NULL;

   _model = model;

   _scheme = 0;

   _schemes[0] = defaultStyles;
   _schemes[1] = classicStyles;
}

//Document* EditFrame :: getDocument(int index)
//{
////   if (index != -1) {
////      TextView* textView = (TextView*)_getTabControl(index);
////
////      return textView ? textView->getDocument() : NULL;
////   }
////   else return _currentDoc;
//
//   return NULL; // !!temporal
//}

type_textview_changed EditFrame :: textview_changed()
{
   return _textview_changed;
}

int EditFrame :: newDocument(const char* name, Document* document)
{
   TextView* textView = new _GUI_::TextView();
   textView->textview_changed().connect(sigc::mem_fun(*this,
              &EditFrame::on_textview_changed));

   textView->setStyles(STYLE_MAX + 1, _schemes[_scheme], 15, 20);
   textView->applySettings(_model->tabSize, _model->tabCharUsing, _model->lineNumberVisible, _model->highlightSyntax);

   textView->setDocument(document);

   textView->show(); // !! temporal?

//   g_signal_connect(textView->getWidgetHandle(), "editor-changed",
//         G_CALLBACK(editor_changed), _owner);
//
//   g_signal_connect(textView->getWidgetHandle(), "ctrl-tab-pressed",
//         G_CALLBACK(ctrl_tab_pressed), _owner);

   int index = addTab(name, textView);

   show();
   setFocus();

   return index;
}

void EditFrame :: eraseDocumentTab(int index)
{
   _model->currentDoc = NULL;

   deleteTab(index);
}

void EditFrame :: onTabChange(int index)
{
   TabBar::onTabChange(index);

   if (_current != NULL) {
      _model->currentDoc = ((TextView*)_current)->getDocument();

   }
   else _model->currentDoc = NULL;
}

void EditFrame :: refreshDocument()
{
   if (_current)
      ((TextView*)_current)->refreshView();
}

//void EditFrame :: reloadSettings()
//{
////   TabPages::Iterator it = _tabs.start();
////   while (!it.Eof()) {
////      ((TextView*)*it)->setStyles(STYLE_MAX + 1, _schemes[Settings::scheme], 15, 20);
////
////      ((TextView*)*it)->applySettings(Settings::tabSize, Settings::tabCharUsing,
////         Settings::lineNumberVisible, Settings::highlightSyntax);
////
////      it++;
////   }
////
//////   if(_child) {
//////      if (_scheme != Settings::scheme)
//////         ((TextView*)_child)->setStyles(STYLE_MAX + 1, _schemes[Settings::scheme], 15, 20);
//////
//////      ((TextView*)_child)->applySettings(Settings::tabSize, Settings::tabCharUsing,
//////         Settings::lineNumberVisible, Settings::highlightSyntax);
//////   }
//////
//////   _scheme = Settings::scheme;
////
//}
//
