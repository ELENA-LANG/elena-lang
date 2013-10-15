//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux EditFrame container File
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkeditframeH
#define gtkeditframeH

#include "../editframe.h"
#include "gtk-linux32/gtktabbar.h"
#include "gtk-linux32/gtktextview.h"
//#include "gtk-linux32/gtkmenu.h"
#include "gtk-linux32/gtksdi.h"

namespace _GUI_
{

// --- EditFrame ---

class EditFrame : public TabBar, public _EditFrame
{
protected:
//   StyleInfo* _schemes[2];
//
//   size_t     _scheme;
//
//   SDIWindow* _owner;

   virtual void addDocumentTab(const _text_t* name, Document* doc);

public:
   virtual const _text_t* getDocumentName(int index);
   virtual int getCurrentDocumentIndex() { return get_current_page(); }
   virtual int getDocumentIndex(const _text_t* name) { return getTabIndex(name); }
   virtual Document* getDocument(int index);
   virtual int getDocumentCount() { return getCount(); }

//   virtual void renameDocument(int index, const TCHAR* name);
//   virtual void markDocument(int index, bool modified);
//   virtual void closeDocument(int index);
//
//   virtual void selectDocument(const TCHAR* name);
//   virtual void selectDocument(int index);
//
//   virtual void onTabChange(int index);
//
//   bool copyClipboard(Clipboard& board);
   void eraseSelection();
   void pasteClipboard(Clipboard& board);

   void indent();
   void outdent();

   virtual void showContextMenu(int x, int y);
   virtual void reloadSettings();

   virtual void refreshDocument();

   EditFrame(SDIWindow* owner);
};

} // _GUI_

#endif // gtkeditframeH
