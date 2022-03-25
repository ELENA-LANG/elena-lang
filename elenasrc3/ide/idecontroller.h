//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller header File
//                                             (C)2005-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDECONTROLLER_H
#define IDECONTROLLER_H

#include "controller.h"
#include "debugcontroller.h"
#include "project.h"

namespace elena_lang
{

   // --- SourceViewController ---
   class SourceViewController : public TextViewController
   {
   public:
   };

   // --- DebugAction ---
   enum class DebugAction
   {
      None,
      Run
   };

   // --- ProjectController ---
   class ProjectController
   {
      DebugController      debugController;

      bool onDebugAction(ProjectModel& model, DebugAction action);
      bool isOutaged(bool noWarning);

      bool startDebugger(ProjectModel& model/*, bool stepMode*/);

   public:
      bool doCompileProject(ProjectModel& model, DebugAction postponedAction);

      void doDebugAction(ProjectModel& model, DebugAction action);
   };

   // --- IDEController ---
   class IDEController
   {
   public:
      SourceViewController sourceController;
      ProjectController    projectController;
   };

} // elena:lang

#endif // IDECONTROLLER_H
