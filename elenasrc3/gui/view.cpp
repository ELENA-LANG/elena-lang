//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class body File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "view.h"

#include "idecommon.h"
#include "sourceformatter.h"

using namespace elena_lang;

// --- TextViewModel ---

TextViewModel :: TextViewModel() :
   TextViewModelBase(),
   _documents(nullptr),
   _listeners(nullptr),
   _docListeners(nullptr)
{
}

int TextViewModel :: getCurrentIndex()
{
   int index = 0;
   for (auto it = _documents.start(); !it.eof(); ++it) {
      index++;

      if ((*it)->documentView == _currentView)
         return index;
   }

   return 0;
}

int TextViewModel :: getDocumentIndex(ustr_t name)
{
   if (name.empty())
      name = getDocumentName(-1);

   int index = 0;
   for (auto it = _documents.start(); !it.eof(); ++it) {
      index++;

      if ((*it)->name.compare(name))
         return index;
   }

   return -1;
}

void TextViewModel :: attachListener(TextViewListener* listener)
{
   _listeners.add(listener);

   int index = 0;
   int selected = -1;
   for (auto it = _documents.start(); !it.eof(); ++it) {
      index++;

      listener->onDocumentNew(index);
      if ((*it)->documentView == _currentView)
         selected = index;
   }

   if (selected != -1)
      listener->onDocumentSelect(selected);
}

void TextViewModel :: onDocumentNew(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentNew(index);
   }
}

void TextViewModel :: onDocumentSelect(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentSelect(index);
   }
}

//void TextViewModel :: afterDocumentSelect(int index)
//{
//   for (auto it = _listeners.start(); !it.eof(); ++it) {
//      (*it)->afterDocumentSelect(index);
//   }
//}
//
//void TextViewModel :: onDocumentRename(int index)
//{
//   for (auto it = _listeners.start(); !it.eof(); ++it) {
//      (*it)->onDocumentRename(index);
//   }
//}
//
//void TextViewModel :: onModelChanged()
//{
//   
//}
//
//void TextViewModel :: onModelModeChanged(int index)
//{
//   DocumentView* view = nullptr;
//   if (index == -1) {
//      index = getCurrentIndex();
//      view = _currentView;
//   }
//   else view = getDocumentByIndex(index);
//
//   for (auto it = _listeners.start(); !it.eof(); ++it) {
//      (*it)->onDocumentModeChanged(index, view->status.modifiedMode);
//   }
//}

void TextViewModel :: beforeDocumentClose(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->beforeDocumentClose(index);
   }
}

void TextViewModel :: onDocumentClose(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentClose(index);
   }
}

void TextViewModel :: addDocumentView(ustr_t name, Text* text, path_t path)
{
   empty = false;

   auto docView = new DocumentView(text, SourceFormatter::getInstance());
   docView->setSize(_size);

   _documents.add(new DocumentViewScope(name, path, docView));
   if (emptystr(path)) {
      docView->markAsUnnamed();
      docView->markAsModified();
   }

   onDocumentNew(_documents.count());
}

void TextViewModel :: renameDocumentView(ustr_t oldName, ustr_t newName, path_t path)
{
   int index = getDocumentIndex(oldName);
   if (index <= 0)
      return;

   auto info = _documents.get(index);
   if (info != nullptr) {
      info->name.free();

      if (!info->path.empty())
         info->path.free();

      info->name = newName.clone();
      info->path = path.clone();
   }

   //onDocumentRename(index);
}

bool TextViewModel :: closeDocumentView(int index)
{
   bool closed = false;
   if (index > 0) {
      beforeDocumentClose(index);

      auto info = _documents.get(index);

      _documents.cut(info);

      closed = true;
   }

   clearDocumentView();

   if (closed) {
      empty = _documents.count() == 0;

      onDocumentClose(index);

      return true;
   }
   else return false;
}

void TextViewModel :: clearDocumentView()
{
   _currentView = nullptr;
}

bool TextViewModel :: selectDocumentView(int index)
{
   if (_documents.count() == 0)
      return false;

   if (index > _documents.count_int())
      index = _documents.count_int();

   bool selected = false;

   auto scope = _documents.get(index);
   if (scope->documentView) {
      _currentView = scope->documentView;

      selected = true;

      onDocumentSelect(index);
   }

   return selected;
}

ustr_t TextViewModel :: getDocumentName(int index)
{
   int currentIndex = 1;
   for (auto it = _documents.start(); !it.eof(); ++it) {
      if (currentIndex == index || (index == -1 && (*it)->documentView == _currentView))
         return (*it)->name;

      currentIndex++;
   }

   return nullptr;
}

ustr_t TextViewModel :: getDocumentNameByPath(path_t path)
{
   auto info =_documents.retrieve<path_t>(path, [](path_t path, DocumentViewScope* item)
      {
         return PathUtil::compare(path, item->path.str());
      });

   return info ? info->name : nullptr;
}

DocumentView* TextViewModel :: getDocument(int index)
{
   auto info = _documents.get(index);

   return info ? info->documentView : nullptr;
}

path_t TextViewModel :: getDocumentPath(int index)
{
   auto info = _documents.get(index);

   return info ? info->path : nullptr;
}

void TextViewModel :: resize(Point size)
{
   _size = size;

   for (auto it = _documents.start(); !it.eof(); ++it) {
      (*it)->documentView->setSize(size);
   }
}
