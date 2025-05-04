//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller header File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CONTOLLER_H
#define CONTOLLER_H

#include "elena.h"
#include "guieditor.h"

namespace elena_lang
{
   // --- FileDialogBase ---
   class FileDialogBase
   {
   public:
      virtual bool openFile(PathString& path) = 0;
      virtual bool openFiles(List<path_t, freepath>& files) = 0;
      virtual bool saveFile(path_t ext, PathString& path) = 0;
   };

   // --- FontDialogBase ---
   class FontDialogBase
   {
   public:
      virtual bool selectFont(FontInfo& fontInfo) = 0;
   };

   // --- MessageDialogBase ---
   class MessageDialogBase
   {
   public:
      enum Answer
      {
         Yes, No, Cancel
      };

      virtual Answer question(text_str message, const text_str param) = 0;
      virtual Answer question(text_str message) = 0;

      virtual void info(text_str message) = 0;
   };

   // --- ProjectSettingsBase ---
   class ProjectSettingsBase
   {
   public:
      virtual bool showModal() = 0;
   };

   class EditorSettingsBase
   {
   public:
      virtual bool showModal() = 0;
   };

   class IDESettingsBase
   {
   public:
      virtual bool showModal() = 0;
   };

   class DebuggerSettingsBase
   {
   public:
      virtual bool showModal() = 0;
   };

   class FindDialogBase
   {
   public:
      virtual bool showModal() = 0;
   };

   class GotoDialogBase
   {
   public:
      virtual bool showModal(int& row) = 0;
   };

   class WindowListDialogBase
   {
   public:
      enum class Mode
      {
         None = 0,
         Activate,
         Close
      };

      typedef Pair<int, Mode, 0, Mode::None> SelectResult;

      virtual SelectResult selectWindow() = 0;
   };

   // --- TextViewController ---
   class TextViewController : public TextViewControllerBase
   {
   protected:      
      NotifierBase*    _notifier;

      //void onTextChanged(TextViewModelBase* model, DocumentView* view);

      void notifyOnClipboardOperation(ClipboardBase* clipboard); // !! obsolete
      void notifyTextModelChange(TextViewModelBase* model, DocumentChangeStatus& changeStatus);

   public:
      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;
      }

      bool openDocument(TextViewModelBase* model, ustr_t name, path_t path,
         FileEncoding encoding, bool included) override;
      bool selectDocument(TextViewModelBase* model, int index) override;
      void selectNextDocument(TextViewModelBase* model);
      void selectPreviousDocument(TextViewModelBase* model);
      void closeDocument(TextViewModelBase* model, int index, int& status) override;

      void newDocument(TextViewModelBase* model, ustr_t name, bool included) override;

      bool insertNewLine(TextViewModelBase* model) override;
      bool insertChar(TextViewModelBase* model, text_c ch) override;
      bool insertLine(TextViewModelBase* model, text_t s, size_t length) override;
      bool eraseChar(TextViewModelBase* model, bool moveback) override;

      void setOverwriteMode(TextViewModelBase* model) override;

      void indent(TextViewModelBase* model) override;
      void outdent(TextViewModelBase* model) override;

      void undo(TextViewModelBase* model) override;
      void redo(TextViewModelBase* model) override;

      void trim(TextViewModelBase* model) override;
      void eraseLine(TextViewModelBase* model) override;
      void duplicateLine(TextViewModelBase* model) override;

      void deleteText(TextViewModelBase* model) override;
      void insertBlockText(TextViewModelBase* model, const_text_t s, size_t length) override;
      void deleteBlockText(TextViewModelBase* model, const_text_t s, size_t length) override;

      void upperCase(TextViewModelBase* model);
      void lowerCase(TextViewModelBase* model);

      bool copyToClipboard(TextViewModelBase* model, ClipboardBase* clipboard) override;
      void pasteFromClipboard(TextViewModelBase* model, ClipboardBase* clipboard) override;

      void moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretHome(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretEnd(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void movePageDown(TextViewModelBase* model, bool kbShift) override;
      void movePageUp(TextViewModelBase* model, bool kbShift) override;
      void moveToFrame(TextViewModelBase* model, int col, int row, bool kbShift) override;

      void selectWord(TextViewModelBase* model) override;
      void selectAll(TextViewModelBase* model) override;

      void resizeModel(TextViewModelBase* model, Point size) override;

      bool findText(TextViewModelBase* model, FindModel* findModel);
      bool replaceText(TextViewModelBase* model, FindModel* findModel);

      void goToLine(TextViewModelBase* model, int row);

      void highlightBrackets(TextViewModelBase* model, DocumentChangeStatus& changeStatus);

      TextViewController()
      {
         _notifier = nullptr;
      }
   };

}

#endif
