//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef VIEW_H
#define VIEW_H

#include "guieditor.h"

namespace elena_lang
{
   typedef Map<ustr_t, DocumentView*, allocUStr, freeUStr, freeobj> DocumentViewMap;
   typedef List<TextViewListener*> TextViewListeners;

   // --- TextViewModel ---
   class TextViewModel : public TextViewModelBase
   {
   protected:
      DocumentViewMap   _documents;
      TextViewListeners _listeners;
      DocumentNotifiers _docListeners;

      void onNewDocumentView(int index);
      void onDocumentViewSelect(int index);

   public:
      void attachListener(TextViewListener* listener) override;

      void attachDocListener(DocumentNotifier* listener) override;
      void removeDocListener(DocumentNotifier* listener) override;

      void addDocumentView(ustr_t name, Text* text) override;

      ustr_t getDocumentName(int index) override;
      bool selectDocumentView(ustr_t name) override;

      void resize(Point size) override;

      TextViewModel(int fontSize);
      virtual ~TextViewModel() = default;
   };

}

#endif