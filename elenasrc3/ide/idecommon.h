//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE common classes header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDECOMMON_H
#define IDECOMMON_H

#include "guicommon.h"

#define IDE_REVISION_NUMBER                     6

namespace elena_lang
{
   // --- IDEStatus ---
   enum class IDEStatus
   {
      None            = 0,
      Busy            = 1,
      AutoRecompiling = 2,
   };

   inline bool testIDEStatus(IDEStatus value, IDEStatus mask)
   {
      return test((int)value, (int)mask);
   }

   struct GUISettinngs
   {
      bool withTabAboverscore;
   };

   // --- GUIFactory ---
   class GUIFactoryBase
   {
   public:
      virtual GUIApp* createApp() = 0;
      virtual GUIControlBase* createMainWindow() = 0;
   };
}

#endif
