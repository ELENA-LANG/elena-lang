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
   // --- FileDialog ---
   class FileDialog : public FileDialogBase
   {
      OPENFILENAME _struct;
      wchar_t      _fileName[MAX_PATH * 8];        // ??
      int          _defaultFlags;

   public:
      static const wchar_t* SourceFilter;

      bool openFiles(List<path_t, freepath>& files) override;

      FileDialog(HINSTANCE instance, WindowBase* owner, const wchar_t* filter, const wchar_t* caption, 
         const wchar_t* initialDir = nullptr);
   };

}

#endif

