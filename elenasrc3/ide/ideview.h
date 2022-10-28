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

   Map<int, text_t>  captions;

   IDEScheme() : 
      textFrameId(-1), 
      resultControl(-1), 
      compilerOutputControl(-1), 
      errorListControl(-1),
      projectView(-1),
      captions(nullptr)
   {
   }
};

// --- IDEListener ---

class IDEListener
{
public:
   virtual void onIDEChange() = 0;
};

typedef List<IDEListener*> IDEListenerListeners;

// --- IDEModel ---
class IDEModel
{
   IDEListenerListeners listeners;

public:
   IDEStatus       status;

   SourceViewModel sourceViewModel;
   ProjectModel    projectModel;
   IDEScheme       ideScheme;

   bool            appMaximized;


   SourceViewModel* viewModel() { return &sourceViewModel; }

   void attachListener(IDEListener* listener);

   void changeStatus(IDEStatus status);

   void onIDEChange();

   IDEModel()
      : listeners(nullptr), projectModel(&status)
   {
      status = IDEStatus::None;
      appMaximized = false;
   }
};

} // elena:lang

#endif // IDEVIEW_H
