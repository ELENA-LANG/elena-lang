//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CONTOLLER_H
#define CONTOLLER_H

#include "guieditor.h"

namespace elena_lang
{
   // --- FileDialogBase ---
   class FileDialogBase
   {
   public:
      virtual bool openFiles(List<path_t, freepath>& files) = 0;
      virtual bool saveFile(path_t ext, PathString& path) = 0;
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

   public:
      //void onFrameChange() override;

      bool openDocument(TextViewModelBase* model, ustr_t name, path_t path, 
         FileEncoding encoding, int notifyMessage) override;
      void selectDocument(TextViewModelBase* model, ustr_t name) override;

      void newDocument(TextViewModelBase* model, ustr_t name, 
         int notifyMessage) override;

      void indent(TextViewModelBase* model) override;

      void moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;

      TextViewController(TextViewSettings& settings)
      {
         _settings = settings;
      }
   };

}

#endif