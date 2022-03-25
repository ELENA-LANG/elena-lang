//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDEVIEW_H
#define IDEVIEW_H

#include "idecommon.h"
#include "editframe.h"
#include "ideproject.h"

namespace elena_lang
{
// --- IDEModel ---
class IDEModel
{
public:
   IDEStatus       status;

   SourceViewModel sourceViewModel;
   ProjectModel    projectModel;

   SourceViewModel* viewModel() { return &sourceViewModel; }

   IDEModel()
      : projectModel(&status)
   {
      status = IDEStatus::None;
   }
};

} // elena:lang

#endif // IDEVIEW_H
