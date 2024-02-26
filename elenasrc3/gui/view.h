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
   //typedef Pair<path_t, > DocumentInfo 
   typedef Map<ustr_t, DocumentView*, allocUStr, freeUStr, freeobj> DocumentViewMap;
   typedef Map<ustr_t, path_t, allocUStr, freeUStr, freepath>       DocumentPathMap;

   struct DocumentViewScope
   {
      ustr_t        name;
      path_t        path;
      DocumentView* documentView;

      DocumentViewScope(ustr_t name, path_t path, DocumentView* view)
         : name(name.clone()), path(path.clone()), documentView(view)
      {
      }

      ~DocumentViewScope()
      {
         freeobj(documentView);
         name.free();
         path.free();
      }
   };

   typedef List<DocumentViewScope*, freeobj> DocumentViewList;
   typedef List<TextViewListener*>           TextViewListeners;

   // --- TextViewModel ---
   class TextViewModel : public TextViewModelBase
   {
   protected:
      Point             _size;
      DocumentViewList  _documents;

      TextViewListeners _listeners;
      DocumentNotifiers _docListeners;

      void onDocumentNew(int index);
      void onDocumentSelect(int index);
      //void onDocumentRename(int index);

      void beforeDocumentClose(int index);
      void onDocumentClose(int index);

   public:
      void attachListener(TextViewListener* listener) override;

      void addDocumentView(ustr_t name, Text* text, path_t path) override;
      void renameDocumentView(ustr_t oldName, ustr_t newName, path_t path) override;

      int getCurrentIndex();
      int getDocumentIndex(ustr_t name) override;

      ustr_t getDocumentName(int index) override;
      ustr_t getDocumentNameByPath(path_t path) override;

      DocumentView* getDocument(int index) override;
      path_t getDocumentPath(int index) override;

      void clearDocumentView() override;
      bool selectDocumentView(int index) override;

      bool closeDocumentView(int index) override;

      pos_t getDocumentCount() override
      {
         return _documents.count();
      }

      void resize(Point size) override;

      TextViewModel();
      virtual ~TextViewModel() = default;
   };

}

#endif