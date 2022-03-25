//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller implementation File
//                                             (C)2005-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecontroller.h"

using namespace elena_lang;

// --- SourceViewController ---

// --- ProjectController ---

bool ProjectController :: startDebugger(ProjectModel& model)
{
   
}

bool ProjectController :: isOutaged(bool noWarning)
{
   return false; // !! temporal
}

bool ProjectController :: onDebugAction(ProjectModel& model, DebugAction action)
{
   if (testIDEStatus(*model.status, IDEStatus::Busy))
      return false;

   if (!debugController.isStarted()) {
      bool toRecompile = model.autoRecompile && !testIDEStatus(*model.status, IDEStatus::AutoRecompiling);
      if (!isOutaged(toRecompile)) {
         if (toRecompile) {
            if (!doCompileProject(model, action))
               return false;
         }
         return false;
      }
      if (!startDebugger(model))
         return false;
   }
   return true;
}

void ProjectController :: doDebugAction(ProjectModel& model, DebugAction action)
{
   if (!testIDEStatus(*model.status, IDEStatus::Busy)) {
      if (onDebugAction(model, action)) {
         switch (action) {
            case DebugAction::Run:
               debugController.run();
               break;
            default:
               break;
         }
      }
   }
}

bool ProjectController :: doCompileProject(ProjectModel& model, DebugAction postponedAction)
{
   return true; // !! temporal
}
