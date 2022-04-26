//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI common editor header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GUIEDITOR_H
#define GUIEDITOR_H

#include "guicommon.h"
#include "document.h"

namespace elena_lang
{
   // --- ELENA IDE Styles ---
   //#define SCHEME_COUNT                            2

   constexpr auto STYLE_DEFAULT        = 0;
   constexpr auto STYLE_MARGIN         = 1;
   constexpr auto STYLE_TRACE_LINE     = 2;
   //#define STYLE_KEYWORD                           3
   //#define STYLE_COMMENT                           4
   //#define STYLE_OPERATOR                          5
   //#define STYLE_MESSAGE                           6
   //#define STYLE_NUMBER                            7
   //#define STYLE_STRING                            8
   //#define STYLE_HINT                              9  // !! not used
   //#define STYLE_ERROR_LINE                        10
   //#define STYLE_TRACE                             12
   //#define STYLE_BREAKPOINT                        13
   //#define STYLE_HIGHLIGHTED_BRACKET               14
   constexpr auto STYLE_MAX = 2;

   class TextViewListener
   {
   public:
      virtual void onDocumentViewSelect(int index) = 0;
      virtual void onDocumentView(int index) = 0;
   };

   // --- TextViewBase ---
   class TextViewModelBase
   {
   protected:
      DocumentView* _currentView;

   public:
      bool          lineNumbersVisible;
      int           fontSize;

      DocumentView* DocView()
      {
         return _currentView;
      }

      virtual void attachListener(TextViewListener* listener) = 0;

      virtual void attachDocListener(DocumentNotifier* listener) = 0;
      virtual void removeDocListener(DocumentNotifier* listener) = 0;

      virtual void addDocumentView(ustr_t name, Text* text) = 0;

      virtual bool selectDocumentView(ustr_t name) = 0;

      virtual ustr_t getDocumentName(int index) = 0;

      virtual void resize(Point size) = 0;

      bool isAssigned() const
      {
         return _currentView != nullptr;
      }

      TextViewModelBase(int fontSize)
      {
         this->_currentView = nullptr;
         this->lineNumbersVisible = false;
         this->fontSize = fontSize;
      }
   };

   // --- TextViewControllerBase ---
   class TextViewControllerBase
   {
   public:
      //virtual int newDocument(TextViewModelBase* model) = 0;
      virtual void openDocument(TextViewModelBase* model, ustr_t name, path_t path, FileEncoding encoding) = 0;

      virtual void selectDocument(TextViewModelBase* model, ustr_t name) = 0;

      virtual void moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
   };

}

#endif
