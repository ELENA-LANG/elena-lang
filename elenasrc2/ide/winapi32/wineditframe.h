//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Win32 EditFrame container File
//                                              (C)2005-2013, by Alexei Rakov
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

class EditFrame : public MultiTabView, public _EditFrame
{
protected:
   StyleInfo*   _schemes[2];

   size_t       _scheme;
   ContextMenu* _contextMenu;

   virtual int getDocumentTabCount();
   virtual void selectDocumentTab(int index);
   virtual int addDocumentTab(const _path_t* path, Document* doc);   
   virtual void eraseDocumentTab(int index);
   virtual void renameDocumentTab(int index, const _text_t* newName);

public:
   virtual void onTabChange(int);

   virtual int getCurrentDocumentIndex() { return getCurrentIndex(); }

   virtual void markDocument(int index, bool modified);

   void populate(TextView* textView);

   bool copyClipboard(Clipboard& board);
   void pasteClipboard(Clipboard& board);
   void eraseSelection();

   virtual void showContextMenu(int x, int y);

   void indent();
   void outdent();

   virtual void reloadSettings();
   virtual void refreshDocument();

   EditFrame(Window* owner, bool withAbovescore, ContextMenu* contextMenu);
};

} // _GUI_

#endif // wineditframeH
