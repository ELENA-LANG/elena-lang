//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller header File
//                                             (C)2021-2023, by Aleksey Rakov
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

   // --- MessageDialogBase ---
   class MessageDialogBase
   {
   public:
      enum Answer
      {
         Yes, No, Cancel
      };

      virtual Answer question(text_str message, const text_str param) = 0;
   };

   // --- ProjectSettingsBase ---
   class ProjectSettingsBase
   {
   public:
      virtual bool showModal() = 0;
   };

   // --- TextViewSettings ---
   struct TextViewSettings
   {
      EOLMode  eolMode;
      bool     tabUsing;
      int      tabSize;
   };

   // --- TextViewController ---
   class TextViewController : public TextViewControllerBase
   {
   protected:
      TextViewSettings _settings;

      //void onTextChanged(TextViewModelBase* model, DocumentView* view);

      void notifyOnChange(TextViewModelBase* model, DocumentChangeStatus& status);
      void notifyOnClipboardOperation(ClipboardBase* clipboard);

   public:
      bool openDocument(TextViewModelBase* model, ustr_t name, path_t path, 
         FileEncoding encoding) override;
      bool selectDocument(TextViewModelBase* model, int index, NotificationStatus& status) override;
      void selectNextDocument(TextViewModelBase* model);
      void selectPreviousDocument(TextViewModelBase* model);
      void closeDocument(TextViewModelBase* model, int index, NotificationStatus& status) override;

      void newDocument(TextViewModelBase* model, ustr_t name) override;

      bool insertNewLine(TextViewModelBase* model) override;
      bool insertChar(TextViewModelBase* model, text_c ch) override;
      bool eraseChar(TextViewModelBase* model, bool moveback) override;

      void indent(TextViewModelBase* model) override;

      void undo(TextViewModelBase* model) override;
      void redo(TextViewModelBase* model) override;

      void deleteText(TextViewModelBase* model) override;
      void insertBlockText(TextViewModelBase* model, const text_t s, size_t length) override;
      void deleteBlockText(TextViewModelBase* model, const text_t s, size_t length) override;

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

      TextViewController(TextViewSettings& settings)
      {
         _settings = settings;
      }
   };

}

#endif