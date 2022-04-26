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

void TextViewModel :: attachListener(TextViewListener* listener)
{
   _listeners.add(listener);

   int index = 0;
   int selected = -1;
   for (auto it = _documents.start(); !it.eof(); ++it) {
      listener->onDocumentView(index);
      index++;
      if (*it == _currentView)
         selected = index;
   }

   if (selected != -1)
      listener->onDocumentViewSelect(selected);
}

void TextViewModel::attachDocListener(DocumentNotifier* listener)
{
   _docListeners.add(listener);

   for (auto it = _documents.start(); !it.eof(); ++it) {
      (*it)->attachNotifier(listener);
   }
}

void TextViewModel :: removeDocListener(DocumentNotifier* listener)
{
   for (auto it = _documents.start(); !it.eof(); ++it) {
      (*it)->removeNotifier(listener);
   }

   _docListeners.cut(listener);
}

void TextViewModel :: onNewDocumentView(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentView(index);
   }
}

void TextViewModel :: onDocumentViewSelect(int index)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onDocumentViewSelect(index);
   }
}

void TextViewModel :: addDocumentView(ustr_t name, Text* text)
{
   auto docView = new DocumentView(text, ELENADocFormatter::getInstance());

   _documents.add(name, docView);

   for (auto it = _docListeners.start(); !it.eof(); ++it) {
      docView->attachNotifier(*it);
   }

   onNewDocumentView(_documents.count() - 1);
}

bool TextViewModel :: selectDocumentView(ustr_t name)
{
   int index = 0;
   for (auto it = _documents.start(); !it.eof(); ++it) {
      if (it.key().compare(name)) {
         _currentView = *it;

         onDocumentViewSelect(index);
         break;
      }

      index++;
   }

   return _currentView != nullptr;
}

ustr_t TextViewModel :: getDocumentName(int index)
{
   int currentIndex = 0;
   for (auto it = _documents.start(); !it.eof(); ++it) {
      if (currentIndex == index)
         return it.key();

      currentIndex++;
   }

   return nullptr;
}

void TextViewModel :: resize(Point size)
{
   for (auto it = _documents.start(); !it.eof(); ++it) {
      (*it)->resize(size);
   }
}
