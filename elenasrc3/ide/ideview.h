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

   SourceViewModel* viewModel() { return &sourceViewModel; }

   void attachListener(IDEListener* listener);

   void changeStatus(IDEStatus status);

   void onIDEChange();

   IDEModel(int fontSize)
      : listeners(nullptr), sourceViewModel(fontSize), projectModel(&status)
   {
      status = IDEStatus::None;
   }
};

} // elena:lang

#endif // IDEVIEW_H
