//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Gtk+ EditFrame Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkeditframe.h"
//#include "../appwindow.h"

using namespace _GUI_;

////static void editor_changed(gpointer view, gpointer window)
////{
////   ((SDIWindow*)window)->_onClientChanged(NULL, 0, VIEWCHANGED_NOTIFY);
////}
////
////static void ctrl_tab_pressed(GtkWidget* sender, gboolean shift, gpointer window)
////{
////   ((SDIWindow*)window)->_onMenu(shift ? IDM_WINDOW_PREVIOUS : IDM_WINDOW_NEXT);
////}

// -- Styles ---

StyleInfo defaultStyles[STYLE_MAX + 1] = {
   {Colour(0, 0, 0), Colour(1, 1, 1), _T("Monospace 10")},
   {Colour(0, 0, 0), Colour(0.84, 0.84, 0.84), _T("Monospace 10")},
	{Colour(0, 0, 0), Colour(0.75, 0.75, 0.75), _T("Monospace 10")},
	{Colour(0, 0, 1), Colour(1, 1, 1), _T("Monospace 10")},
   {Colour(0, 0.4, 0.5), Colour(1, 1, 1), _T("Monospace Bold 10")},
	{Colour(0, 0, 0), Colour(1, 1, 1), _T("Monospace Bold 10")},
	{Colour(1, 0.5, 0.4), Colour(1, 1, 1), _T("Monospace 10")},
	{Colour(0, 0.5, 0.5), Colour(1, 1, 1), _T("Monospace 10")},
   {Colour(0, 0.5, 0), Colour(1, 1, 1), _T("Monospace 10")},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), _T("Monospace Bold 10")},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), _T("Monospace Bold 10")},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), _T("Monospace Bold 10")},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), _T("Monospace Bold 10")},
	{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), _T("Monospace Bold 10")},
	{Colour(0.37, 0.37, 0.37), Colour(1, 1, 1), _T("Monospace Bold 10")},
};

StyleInfo classicStyles[STYLE_MAX + 1] = {
	{Colour(1, 1, 0), Colour(0, 0, 0.5), _T("Monospace 10")},
	{Colour(0, 0, 0), Colour(0.84, 0.84, 0.84), _T("Monospace 10")},
	{Colour(0.37, 0.37, 0.37), Colour(1, 1, 1), _T("Monospace 10")},
	{Colour(0.7, 0.7, 0.7), Colour(0, 0, 0.5), _T("Monospace 10")},
	{Colour(1, 1, 1), Colour(0, 0, 0.5), _T("Monospace 10")},
	{Colour(1, 1, 0), Colour(0, 0, 0.5), _T("Monospace Bold 10")},
	{Colour(0, 1, 0.5), Colour(0, 0, 0.5), _T("Monospace 10")},
	{Colour(0, 1, 1), Colour(0, 0, 0.5), _T("Monospace 10")},
   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), _T("Monospace 10")},
   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), _T("Monospace 10")},
	{Colour(1, 1, 0), Colour(0, 0, 0.5), _T("Monospace Bold 10")},
	{Colour(0, 1, 0.5), Colour(0, 0, 0.5), _T("Monospace 10")},
	{Colour(0, 1, 1), Colour(0, 0, 0.5), _T("Monospace 10")},
   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), _T("Monospace 10")},
   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), _T("Monospace 10")}//,
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
//
int EditFrame :: newDocument(const char* name, Document* document)
{
   TextView* textView = new _GUI_::TextView();

   textView->setStyles(STYLE_MAX + 1, _schemes[_scheme], 15, 20);
   textView->applySettings(_model->tabSize, _model->tabCharUsing, _model->lineNumberVisible, _model->highlightSyntax);

   textView->setDocument(document);

   textView->show(); // !! temporal?

//   g_signal_connect(textView->getWidgetHandle(), "editor-changed",
//         G_CALLBACK(editor_changed), _owner);
//
//   g_signal_connect(textView->getWidgetHandle(), "ctrl-tab-pressed",
//         G_CALLBACK(ctrl_tab_pressed), _owner);

   addTab(name, textView);

   show();
   setFocus();
}

////void EditFrame :: renameDocument(int index, const TCHAR* name)
////{
////   renameTab(index, name);
////}

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

////void EditFrame :: selectDocument(const TCHAR* name)
////{
////   _currentDoc = NULL;
////   selectTab(getTabIndex(name));
////   if (_current != NULL)
////      _currentDoc = ((TextView*)_current)->getDocument();
////}
////
////void EditFrame :: selectDocument(int index)
////{
////   _currentDoc = NULL;
////   selectTab(index);
////   if (_current != NULL)
////      _currentDoc = ((TextView*)_current)->getDocument();
////}
////
////bool EditFrame :: copyClipboard(Clipboard& board)
////{
////   if (_currentDoc && _currentDoc->hasSelection()) {
////      //board.clear();
////
////      int length = _currentDoc->getSelectionLength();
////      TCHAR* text = _ELENA_::String::allocate(length);
////
////      _currentDoc->copySelection(text);
////
////      board.settext(text);
////
////      _ELENA_::freestr(text);
////
////      return true;
////   }
////   else return false;
////}
//
//void EditFrame :: eraseSelection()
//{
////   if (_currentDoc && !_currentDoc->status.readOnly) {
////      _currentDoc->eraseChar(false);
////
////      ((TextView*)_current)->refreshView();
////   }
//}
//
//void EditFrame :: pasteClipboard(Clipboard& board)
//{
////   if (_currentDoc && !_currentDoc->status.readOnly) {
////      TCHAR* text = board.gettext();
////      if  (!_ELENA_::emptystr(text)) {
////         _currentDoc->insertLine(text, _ELENA_::getlength(text));
////
////         board.freetext(text);
////      }
////      ((TextView*)_current)->refreshView();
////   }
//}

void EditFrame :: refreshDocument()
{
   if (_current)
      ((TextView*)_current)->refreshView();
}

//void EditFrame :: outdent()
//{
////   // !! partially migrated
////   if (_currentDoc && !_currentDoc->status.readOnly) {
////      ((TextView*)_current)->outdent();
////   }
//}
//
////void EditFrame :: markDocument(int index, bool modified)
////{
////   _ELENA_::String caption(getTabName(index));
////   if (modified) {
////      caption.append('*');
////
////      renameTabCaption(index, caption);
////   }
////   else renameTabCaption(index, caption);
////}
//
//const char* EditFrame :: getDocumentName(int index)
//{
////   return getTabName((index == -1) ? getCurrentIndex() : index);
//   return NULL; //!!temporal
//}
//
//void EditFrame :: showContextMenu(int x, int y)
//{
//
//}
//
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
