//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE menu history header File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MENUHISTORY_H
#define MENUHISTORY_H

#include "guieditor.h"

namespace elena_lang
{
   // --- MenuHistory ---
   class MenuHistoryBase : public TextViewListener
   {
   protected:
      int                     _maxCount;
      int                     _count;
      int                     _menuBaseId;
      bool                    _withSeparator;

      const_text_t            _defCaption;

      TextViewModelBase*      _model;
      GUIMenuBase*            _menu;

      List<path_t, freepath>  _list;

      int getListIndex(int docIndex);

      virtual void eraseLast();

      void reloadList();

   public:
      void assign(GUIMenuBase* menu)
      {
         _menu = menu;
      }

      void onDocumentNew(int index) override;
      void beforeDocumentClose(int index) override;

      MenuHistoryBase(TextViewModelBase* model, int maxCount, int menuBaseId,
         const_text_t defCaption,
         bool withSeparator);
   };

} // elena:lang

#endif // MENUHISTORY_H
