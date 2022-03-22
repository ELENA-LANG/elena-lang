//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller header File
//                                             (C)2005-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDECONTROLLER_H
#define IDECONTROLLER_H

#include "controller.h"

namespace elena_lang
{

   // --- SourceViewController ---
   class SourceViewController : public TextViewController
   {
   public:
   };

   // --- IDEController ---
   class IDEController
   {
   public:
      SourceViewController sourceController;
   };

} // elena:lang

#endif // IDECONTROLLER_H
