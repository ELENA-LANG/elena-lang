//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//		Win32: Static dialogs header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINDIALOGS_H
#define WINDIALOGS_H

#include "controller.h"
#include "editcontrol.h"

namespace elena_lang
{
   class MsgBox
   {
   public:
      static int show(HWND owner, const wchar_t* message, int type);

      static int showQuestion(HWND owner, const wchar_t* message);

      static bool isCancel(int result) { return result == IDCANCEL; }
      static bool isYes(int result) { return result == IDYES; }
      static bool isNo(int result) { return result == IDNO; }
   };

   // --- Dialog ---
   class Dialog : public DialogBase
   {
      WindowBase*  _owner;

      OPENFILENAME _struct;
      wchar_t      _fileName[MAX_PATH * 8];        // ??
      int          _defaultFlags;

   public:
      static const wchar_t* SourceFilter;

      bool openFiles(List<path_t, freepath>& files) override;
      bool saveFile(path_t ext, PathString& path) override;

      Answer question(text_str message, text_str param) override;

      Dialog(HINSTANCE instance, WindowBase* owner, const wchar_t* filter, const wchar_t* caption, 
         const wchar_t* initialDir = nullptr);
   };

}

#endif

