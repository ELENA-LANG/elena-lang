//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ Menu Header File
//                                              (C)2005-2010, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkmenuH
#define gtkmenuH

#include "gtkcommon.h"

namespace _GUI_
{

// --- Menu ---

class Menu //: public _BaseControl
{
//   GtkItemFactory* _factory;
//   GtkWidget*      _widget;
//   GtkAccelGroup*  _accel;

public:
//   void _show()
//   {
//      gtk_widget_show_all(_widget);
//   }
//
//   GtkAccelGroup* getAccel() { return _accel; }
//
//   virtual void* getHandle() { return _widget; }

   char getMnemonicAccKey() const { return '_'; };

   void enableItemById(int id, bool enabled) {}
   void checkItemById(int id, bool enabled) {}

   void eraseItemById(int id) {}

   void insertItemById(int positionId, int id, const char* caption) { }
   void insertSeparatorById(int positionId, int id) {}

   void renameItemById(int id, const char* caption) {}

//   Menu(GtkItemFactoryEntry* items, int itemNumber, void* object);
};

} // _GUI_

#endif // gtkmenuH
