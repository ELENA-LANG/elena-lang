//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI common editor header File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GUIEDITOR_H
#define GUIEDITOR_H

#include "guicommon.h"
#include "document.h"

namespace elena_lang
{
   // --- ELENA IDE Styles ---
   constexpr auto SCHEME_COUNT         = 2;

   constexpr auto STYLE_DEFAULT        = 0;
   constexpr auto STYLE_MARGIN         = 1;
   constexpr auto STYLE_SELECTION      = 2;
   constexpr auto STYLE_TRACE_LINE     = 3;
   constexpr auto STYLE_ERROR_LINE     = 4;
   //#define STYLE_KEYWORD                           3
   //#define STYLE_COMMENT                           4
   //#define STYLE_OPERATOR                          5
   //#define STYLE_MESSAGE                           6
   //#define STYLE_NUMBER                            7
   //#define STYLE_STRING                            8
   //#define STYLE_HINT                              9  // !! not used
   //#define STYLE_TRACE                             12
   //#define STYLE_BREAKPOINT                        13
   //#define STYLE_HIGHLIGHTED_BRACKET               14
   constexpr auto STYLE_MAX            = 4;

   // --- ClipboardBase ----
   class ClipboardBase
   {
   public:
      virtual bool copyToClipboard(DocumentView* docView) = 0;
      virtual void pasteFromClipboard(DocumentChangeStatus& status, DocumentView* docView) = 0;
   };

   // --- TextViewListener --
   class TextViewListener
   {
   public:
      //virtual void afterDocumentSelect(int index) = 0;

      virtual void onDocumentSelect(int index) = 0;
      virtual void onDocumentNew(int index) = 0;
      //virtual void onDocumentRename(int index) = 0;
      //virtual void onDocumentModeChanged(int index, bool modifiedMode) = 0;

      virtual void beforeDocumentClose(int index) = 0;
      virtual void onDocumentClose(int index) = 0;
   };

   // --- TextViewBase ---
   class TextViewModelBase
   {
   protected:
      DocumentView* _currentView;

   public:
      bool          lineNumbersVisible;
      bool          empty;
      int           fontSize;
      int           schemeIndex;

      DocumentView* DocView()
      {
         return _currentView;
      }

      virtual void beforeDocumentSelect(int index) = 0;
      //virtual void afterDocumentSelect(int index) = 0;

      //virtual void onTextViewChanged() = 0;

      virtual void attachListener(TextViewListener* listener) = 0;

      virtual void attachDocListener(DocumentNotifier* listener) = 0;
      virtual void removeDocListener(DocumentNotifier* listener) = 0;

      virtual void addDocumentView(ustr_t name, Text* text, path_t path) = 0;
      virtual void renameDocumentView(ustr_t oldName, ustr_t newName, path_t path) = 0;

      virtual void clearDocumentView() = 0;
      virtual bool selectDocumentView(ustr_t name) = 0;
      virtual bool selectDocumentViewByIndex(int index) = 0;

      virtual bool closeDocumentView(ustr_t name) = 0;

      virtual ustr_t getDocumentName(int index) = 0;
      virtual ustr_t getDocumentNameByPath(path_t path) = 0;

      virtual DocumentView* getDocumentByIndex(int index) = 0;
      virtual DocumentView* getDocument(ustr_t name) = 0;
      virtual path_t getDocumentPath(ustr_t name) = 0;

      virtual int getDocumentIndex(ustr_t name) = 0;

      virtual pos_t getDocumentCount() = 0;

      virtual void resize(Point size) = 0;

      bool isAssigned() const
      {
         return _currentView != nullptr;
      }

      TextViewModelBase()
      {
         this->_currentView = nullptr;
         this->lineNumbersVisible = false;
         this->empty = true;
         this->fontSize = 10;
         this->schemeIndex = 0;
      }
   };

   // --- TextViewControllerBase ---
   class TextViewControllerBase
   {
   public:
      //virtual void onFrameChange() = 0;

      virtual void newDocument(TextViewModelBase* model, ustr_t name, 
         int notifyMessage) = 0;
      virtual bool openDocument(TextViewModelBase* model, ustr_t name, path_t path, 
         FileEncoding encoding) = 0;

      virtual bool selectDocument(TextViewModelBase* model, ustr_t name, NotificationStatus& status) = 0;

      virtual void closeDocument(TextViewModelBase* model, ustr_t name, NotificationStatus& status) = 0;

      virtual bool insertNewLine(TextViewModelBase* model) = 0;
      virtual bool insertChar(TextViewModelBase* model, text_c ch) = 0;
      virtual bool eraseChar(TextViewModelBase* model, bool moveback) = 0;

      virtual void undo(TextViewModelBase* model) = 0;
      virtual void redo(TextViewModelBase* model) = 0;

      virtual void indent(TextViewModelBase* model) = 0;
      virtual void deleteText(TextViewModelBase* model) = 0;
      virtual void insertBlockText(TextViewModelBase* model, const text_t s, size_t length) = 0;

      virtual bool copyToClipboard(TextViewModelBase* model, ClipboardBase* clipboard) = 0;
      virtual void pasteFromClipboard(TextViewModelBase* model, ClipboardBase* clipboard) = 0;

      virtual void selectWord(TextViewModelBase* model) = 0;

      virtual void moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void moveCaretHome(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void moveCaretEnd(TextViewModelBase* model, bool kbShift, bool kbCtrl) = 0;
      virtual void movePageUp(TextViewModelBase* model, bool kbShift) = 0;
      virtual void movePageDown(TextViewModelBase* model, bool kbShift) = 0;

      virtual void moveToFrame(TextViewModelBase* model, int col, int row, bool kbShift) = 0;

      virtual void resizeModel(TextViewModelBase* model, Point size) = 0;
   };

}

#endif
