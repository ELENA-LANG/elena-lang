//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Win32 EditFrame container File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef wineditframeH
#define wineditframeH

#include "..\editframe.h"
#include "winapi32\wintabbar.h"
#include "winapi32\wintextview.h"
#include "winapi32\winmenu.h"

namespace _GUI_
{

// --- EditFrame ---

class EditFrame : public MultiTabView//, public _EditFrame
{
protected:
   StyleInfo*   _schemes[2];

   size_t       _scheme;
   ContextMenu* _contextMenu;

   Model*       _model;

   bool resizeStyles(int size);

//   virtual int getDocumentTabCount();
//   virtual void selectDocumentTab(int index);
//   virtual int addDocumentTab(const tchar_t* path, Document* doc);   

public:
   int newDocument(const wchar_t* name, Document* doc);

   virtual void onTabChange(int);

   virtual void renameDocumentTab(int index, const wchar_t* newName);

   virtual int getCurrentDocumentIndex() { return getCurrentIndex(); }
   virtual void markDocument(int index, bool modified);
   virtual void eraseDocumentTab(int index);

   void populate(TextView* textView);

//   bool copyClipboard(Clipboard& board);
//   void pasteClipboard(Clipboard& board);
//   void eraseSelection();

   virtual void showContextMenu(int x, int y, bool hasSelection);

//   void indent();
//   void outdent();

   void init(Model* model);
   void refreshDocument();

   EditFrame(Window* owner, bool withAbovescore, ContextMenu* contextMenu, Model* model);
};

} // _GUI_

#endif // wineditframeH
