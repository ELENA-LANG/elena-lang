//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux EditFrame container File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkeditframeH
#define gtkeditframeH

#include "gtk-linux32/gtktabbar.h"
#include "gtk-linux32/gtktextview.h"
////#include "gtk-linux32/gtkmenu.h"
//#include "gtk-linux32/gtksdi.h"
#include "../idecommon.h"

namespace _GUI_
{

// --- EditFrame ---

class EditFrame : public TabBar
{
protected:
   StyleInfo* _schemes[2];
   size_t     _scheme;

   Model*       _model;

//   SDIWindow* _owner;

public:
   int newDocument(const char* name, Document* doc);

//   virtual int getCurrentDocumentIndex() { return getCurrentIndex(); }
//   virtual void markDocument(int index, bool modified);
   void eraseDocumentTab(int index);

//   virtual const char* getDocumentName(int index);
//   virtual int getCurrentDocumentIndex() { return get_current_page(); }
//   virtual int getDocumentIndex(const char* name) { return getTabIndex(name); }
//   virtual Document* getDocument(int index);
//   virtual int getDocumentCount() { return getCount(); }

//   virtual void renameDocument(int index, const TCHAR* name);
//   virtual void markDocument(int index, bool modified);
//   virtual void closeDocument(int index);
//
//   virtual void selectDocument(const TCHAR* name);
//   virtual void selectDocument(int index);

   virtual void onTabChange(int index);

//   bool copyClipboard(Clipboard& board);
//   void eraseSelection();
//   void pasteClipboard(Clipboard& board);
//
//   void indent();
//   void outdent();
//
//   virtual void showContextMenu(int x, int y);
//   virtual void reloadSettings();
//
//   void init(Model* model);
   void refreshDocument();

   EditFrame(Model* model/*SDIWindow* owner*/);
};

} // _GUI_

#endif // gtkeditframeH
