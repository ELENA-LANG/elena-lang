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

// --- IDEScheme ---
struct IDEScheme
{
   int               textFrameId;
   int               resultControl;
   int               compilerOutputControl;
   int               errorListControl;
   int               projectView;
   int               debugWatch;
   int               menu;
   int               statusBar;
   int               debugContextMenu;
   int               vmConsoleControl;

   Map<int, text_t>  captions;

   IDEScheme() : 
      textFrameId(-1), 
      resultControl(-1), 
      compilerOutputControl(-1), 
      errorListControl(-1),
      projectView(-1),
      debugWatch(-1),
      menu(-1),
      statusBar(-1),
      debugContextMenu(-1),
      vmConsoleControl(-1),
      captions(nullptr)
   {
   }
};

// --- IDEModel ---
class IDEModel
{
public:
   IDEStatus       status;

   SourceViewModel sourceViewModel;
   ProjectModel    projectModel;
   IDEScheme       ideScheme;

   FindModel       findModel;

   bool            appMaximized;

   SourceViewModel* viewModel() { return &sourceViewModel; }

   void changeStatus(IDEStatus status);

   IDEModel()
      : projectModel(&status)
   {
      status = IDEStatus::Empty;
      appMaximized = false;
   }
};

} // elena:lang

#endif // IDEVIEW_H
