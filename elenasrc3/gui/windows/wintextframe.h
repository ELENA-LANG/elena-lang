//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Win32 EditFrame container File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINTEXTFRAME_H
#define WINTEXTFRAME_H

#include "wintabbar.h"

namespace elena_lang
{
   // --- TextViewFrame --
   class TextViewFrame : public MultiTabControl
   {
   public:
      TextViewFrame(ControlBase* view);
   };

}

#endif

