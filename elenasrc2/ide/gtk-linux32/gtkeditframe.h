//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux EditFrame container File
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkeditframeH
#define gtkeditframeH

#include "gtk-linux32/gtktabbar.h"
#include "gtk-linux32/gtktextview.h"
#include "gtk-linux32/gtkmenu.h"
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

public:
   int newDocument(const char* name, Document* doc);

   void eraseDocumentTab(int index);

   virtual void onTabChange(int index);

   void refreshDocument();

   EditFrame(Model* model);
};

} // _GUI_

#endif // gtkeditframeH
