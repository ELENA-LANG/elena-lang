//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//		Win32: Static dialogs header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINDIALOGS_H
#define WINDIALOGS_H

#include "controller.h"

namespace elena_lang
{
   // --- DialogController ---
   class DialogController : public DialogControllerBase
   {
   public:
      bool selectFiles() override;

      DialogController();
   };

}

#endif

