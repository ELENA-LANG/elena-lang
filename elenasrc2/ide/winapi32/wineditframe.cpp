//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI EditFrame Implementation
//                                              (C)2005-2013, by Alexei Rakov
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

EditFrame :: EditFrame(Window* owner, bool withAbovescore, ContextMenu* contextMenu)
   : MultiTabView(owner, withAbovescore, NULL)
{
   _currentDoc = NULL;
   _contextMenu = contextMenu;
   _scheme = 0;

   _schemes[0] = defaultStyles;
   _schemes[1] = classicStyles;
}

void EditFrame :: selectDocumentTab(int index)
{
   selectTab(index);
}

int EditFrame :: getDocumentTabCount()
{
   return getTabCount();
}

int EditFrame :: addDocumentTab(const wchar_t* name, Document* doc)
{
   return addTabView(name, doc);
}

void EditFrame :: eraseDocumentTab(int index)
{
   eraseTabView(index);
}

void EditFrame :: renameDocumentTab(int index, const _text_t* newName)
{
   renameTabView(index, newName);
}


void EditFrame :: reloadSettings()
{
   if(_child) {
      if (_scheme != Settings::scheme)
         ((TextView*)_child)->setStyles(STYLE_MAX + 1, _schemes[Settings::scheme], 15, 20);

      ((TextView*)_child)->applySettings(Settings::tabSize, Settings::tabCharUsing, 
         Settings::lineNumberVisible, Settings::highlightSyntax);
   }

   _scheme = Settings::scheme;
}

void EditFrame :: populate(TextView* textView)
{
   textView->setStyles(STYLE_MAX + 1, _schemes[_scheme], 15, 20);
   textView->applySettings(Settings::tabSize, Settings::tabCharUsing, Settings::lineNumberVisible, Settings::highlightSyntax);

   setChild(textView);
}

void EditFrame :: onTabChange(int)
{
   int index = getCurrentIndex();
   if (index >= 0) {
      _currentDoc = (Document*)getTabParam(index);

      ((TextView*)_child)->setDocument(_currentDoc);
      _child->show();

      refreshDocument();
   }
   else if (getTabCount() == 0) {
      _child->hide();
      _notSelected = true;
      _currentDoc = NULL;
   }
}

void EditFrame :: markDocument(int index, bool modified)
{
   wchar16_t caption[0x100];
   getTabName(index, caption, 0x100);

   int length = _ELENA_::getlength(caption);
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

bool EditFrame :: copyClipboard(Clipboard& board)
{
   if (_currentDoc && _currentDoc->hasSelection()) {
      board.clear();

      HGLOBAL buffer = board.create(_currentDoc->getSelectionLength());
      wchar_t* text = board.allocate(buffer);

      _currentDoc->copySelection(text);

      board.free(buffer);
      board.copy(buffer);

      return true;
   }
   else return false;
}

void EditFrame :: eraseSelection()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->eraseChar(false);

      ((TextView*)_child)->refreshView();
   }
}

void EditFrame :: pasteClipboard(Clipboard& board)
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      HGLOBAL buffer = board.get();
      wchar_t* text = board.allocate(buffer);
      if  (!_ELENA_::emptystr(text)) {
         _currentDoc->insertLine(text, _ELENA_::getlength(text));

         board.free(buffer);
      }
      ((TextView*)_child)->refreshView();
   }
}

void EditFrame :: refreshDocument()
{
   ((TextView*)_child)->refreshView();
}

void EditFrame :: indent()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      ((TextView*)_child)->indent();
   }
}

void EditFrame :: outdent()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      ((TextView*)_child)->outdent();
   }
}

void EditFrame :: showContextMenu(int x, int y)
{
   Point p(x, y);

   bool selected = _currentDoc ? _currentDoc->hasSelection() : false;

   _contextMenu->enableItemById(IDM_EDIT_CUT, selected);
   _contextMenu->enableItemById(IDM_EDIT_COPY, selected);
   _contextMenu->enableItemById(IDM_EDIT_PASTE, Clipboard::isAvailable());

   _contextMenu->show(getOwner(), p);
}
