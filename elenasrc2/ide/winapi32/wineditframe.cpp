//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI EditFrame Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "wineditframe.h"

using namespace _GUI_;

// -- Styles ---

StyleInfo defaultStyles[STYLE_MAX + 1] = {
	{Colour(0), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0), Colour(Canvas::Chrome()), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0), Colour(0xC0, 0xC0, 0xC0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0, 0, 0xFF), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0, 0x80, 0), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0x40, 0x80, 0x80), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
	{Colour(0, 0x00, 0x80), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
	{Colour(0xFF, 0x80, 0x40), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0, 0x80, 0x80), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0, 0x80, 0), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0xFF, 0xFF, 0xFF), Colour(0xFF, 0x0, 0x0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0x60, 0x60, 0x60), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Colour(0xFF, 0xFF, 0xFF), Colour(0xFF, 0x0, 0x0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false}
};

StyleInfo classicStyles[STYLE_MAX + 1] = {
	{Colour(0xFF, 0xFF, 0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0), Colour(Canvas::Chrome()), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0x60, 0x60, 0x60), Colour(0xC0, 0xC0, 0xC0), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
	{Colour(0xFF, 0xFF, 0xFF), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0xC0, 0xC0, 0xC0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0xC0, 0xC0, 0xC0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0xFF, 0xFF, 0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
	{Colour(0, 0xFF, 0x80), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0, 0xFF, 0xFF), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0x80, 0xFF, 0xFF), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0, 0, 0x80), Colour(0xC0, 0xC0, 0xC0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
	{Colour(0x60, 0x60, 0x60), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Colour(0xFF, 0xFF, 0xFF), Colour(0xFF, 0x0, 0x0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Colour(0xFF, 0xFF, 0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false}
};

// --- EditFrame ---

EditFrame :: EditFrame(Window* owner, bool withAbovescore, ContextMenu* contextMenu, Model* model)
   : MultiTabView(owner, withAbovescore, NULL)
{
   _contextMenu = contextMenu;
   _scheme = 0;

   _schemes[0] = defaultStyles;
   _schemes[1] = classicStyles;

   _model = model;
}

int EditFrame :: newDocument(const wchar_t* name, Document* doc)
{
   return addTabView(name, doc);
}

//void _EditFrame :: openDocument(_ELENA_::path_t path, Text* text, _GUI_::LexicalStyler* styler, int encoding)
//{
//   int index = addDocumentTab(_ELENA_::FileName(path), doc);
//
//   _mappings.add(path, index);
//
//   return doc;
//}

//void EditFrame :: selectDocumentTab(int index)
//{
//   selectTab(index);
//}
//
//int EditFrame :: getDocumentTabCount()
//{
//   return getTabCount();
//}
//
//int EditFrame :: addDocumentTab(const wchar_t* name, Document* doc)
//{
//}

void EditFrame :: renameDocumentTab(int index, const wchar_t* newName)
{
   renameTabView(index, newName);
}

bool EditFrame :: resizeStyles(int size)
{
   bool changed = false;
   for(int i = 0 ; i <= STYLE_MAX ; i++) {
      for(int j = 0 ; j < 2 ; j++) {
         if (_schemes[j][i].size != size) {
            changed = true;
            _schemes[j][i].size = size;
         }
      }
   }

   return changed;
}

void EditFrame :: init(Model* model)
{
   bool resized = resizeStyles(model->font_size);

   if(_child) {
      if (resized || _scheme != model->scheme)
         ((TextView*)_child)->setStyles(STYLE_MAX + 1, _schemes[model->scheme], model->font_size + 5, 20);

      ((TextView*)_child)->applySettings(model->tabSize, model->tabCharUsing,
         model->lineNumberVisible, model->highlightSyntax);
   }

   _scheme = model->scheme;
}

void EditFrame :: populate(TextView* textView)
{
   textView->setStyles(STYLE_MAX + 1, _schemes[_scheme], 15, 20);
   textView->applySettings(_model->tabSize, _model->tabCharUsing, _model->lineNumberVisible, _model->highlightSyntax);

   setChild(textView);
}

void EditFrame :: onTabChange(int)
{
   int index = getCurrentIndex();
   if (index >= 0) {
      _model->currentDoc = (Document*)getTabParam(index);

      ((TextView*)_child)->setDocument(_model->currentDoc);
      _child->show();

      refreshDocument();
   }
   else if (getTabCount() == 0) {
      _child->hide();
      _notSelected = true;
      _model->currentDoc = NULL;
   }
}

void EditFrame :: markDocument(int index, bool modified)
{
   wchar_t caption[0x100];
   getTabName(index, caption, 0x100);

   size_t length = _ELENA_::getlength(caption);
   bool marked = caption[length - 1] == '*';

   if (modified) {
      if (!marked) {
         caption[length] = '*';
         caption[length + 1] = 0;

         renameTab(index, caption);
      }
   }
   else if (marked) {
      caption[length - 1] = 0;
      renameTab(index, caption);
   }
}

void EditFrame :: eraseDocumentTab(int index)
{
   eraseTabView(index);
}

void EditFrame :: refreshDocument()
{
   ((TextView*)_child)->refreshView();
}

void EditFrame :: showContextMenu(int x, int y, bool hasSelection)
{
   Point p(x, y);

   _contextMenu->enableItemById(IDM_EDIT_CUT, hasSelection);
   _contextMenu->enableItemById(IDM_EDIT_COPY, hasSelection);
   _contextMenu->enableItemById(IDM_EDIT_PASTE, Clipboard::isAvailable());

   _contextMenu->show(getOwner(), p);
}
