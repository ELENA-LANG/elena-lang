//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class body File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "view.h"
#include "sourceformatter.h"

using namespace elena_lang;

// --- TextViewModel ---

TextViewModel :: TextViewModel(int fontSize) :
   TextViewModelBase(fontSize),
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

   return -1;
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

      listener->onDocumentNew(index, 0);
      if ((*it)->documentView == _currentView)
         selected = index;
   }

   if (selected != -1)
      listener->onDocumentSelect(selected);
}

void TextViewModel::attachDocListener(DocumentNotifier* listener)
{
   _docListeners.add(listener);

   for (auto it = _documents.start(); !it.eof(); ++it) {
      (*it)->documentView->attachNotifier(listener);
   }
}

void TextViewModel :: removeDocListener(DocumentNotifier* listener)
{
   for (auto it = _documents.start(); !it.eof(); ++it) {
      (*it)->documentView->removeNotifier(listener);
   }

   _docListeners.cut(listener);
}

void TextViewModel :: onDocumentNew(int index, int notifyMessage)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentNew(index, notifyMessage);
   }
}

void TextViewModel :: onDocumentSelect(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentSelect(index);
   }
}

void TextViewModel :: afterDocumentSelect(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->afterDocumentSelect(index);
   }
}

void TextViewModel :: onDocumentRename(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentRename(index);
   }
}

void TextViewModel :: onModelChanged()
{
   
}

void TextViewModel :: beforeDocumentClose(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->beforeDocumentClose(index);
   }
}

void TextViewModel :: onDocumentClose(int index, int notifyMessage)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentClose(index, notifyMessage);
   }
}

void TextViewModel :: addDocumentView(ustr_t name, Text* text, path_t path, int notifyMessage)
{
   empty = false;

   auto docView = new DocumentView(text, SourceFormatter::getInstance());

   _documents.add(new DocumentViewScope(name, path, docView));

   for (auto it = _docListeners.start(); !it.eof(); ++it) {
      docView->attachNotifier(*it);
   }

   onDocumentNew(_documents.count(), notifyMessage);
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

   onDocumentRename(index);
}

void TextViewModel :: closeDocumentView(ustr_t name, int notifyMessage)
{
   int index = getDocumentIndex(name);
   if (index >= 0) {
      beforeDocumentClose(index);

      auto info = _documents.get(index);

      _documents.cut(info);

      empty = _documents.count() == 0;
   }

   clearDocumentView();

   onDocumentClose(index, notifyMessage);
}

void TextViewModel :: clearDocumentView()
{
   _currentView = nullptr;
}

bool TextViewModel :: selectDocumentViewByIndex(int index)
{
   ustr_t name = getDocumentName(index);

   return selectDocumentView(name);
}

bool TextViewModel :: selectDocumentView(ustr_t name)
{
   int index = 1;
   for (auto it = _documents.start(); !it.eof(); ++it) {
      if ((*it)->name.compare(name)) {
         _currentView = (*it)->documentView;
         _currentView->status.frameChanged = true;

         onDocumentSelect(index);
         break;
      }

      index++;
   }

   return _currentView != nullptr;
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

DocumentView* TextViewModel :: getDocument(ustr_t name)
{
   int index = getDocumentIndex(name);

   auto info = _documents.get(index);

   return info ? info->documentView : nullptr;
}

path_t TextViewModel :: getDocumentPath(ustr_t name)
{
   int index = getDocumentIndex(name);

   auto info = _documents.get(index);

   return info ? info->path : nullptr;
}

void TextViewModel :: resize(Point size)
{
   for (auto it = _documents.start(); !it.eof(); ++it) {
      (*it)->documentView->resize(size);
   }
}
