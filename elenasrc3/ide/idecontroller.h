//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller header File
//                                             (C)2005-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef IDECONTROLLER_H
#define IDECONTROLLER_H

#include "controller.h"
#include "debugcontroller.h"
#include "ideproject.h"

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
      DebugController      _debugController;
      NotifierBase*        _notifier;

      bool onDebugAction(ProjectModel& model, DebugAction action);
      bool isOutaged(bool noWarning);

      bool startDebugger(ProjectModel& model/*, bool stepMode*/);

   public:
      bool doCompileProject(ProjectModel& model, DebugAction postponedAction);

      void doDebugAction(ProjectModel& model, DebugAction action);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;
      }

      void notify(int messageCode)
      {
         if (_notifier)
            _notifier->notify(messageCode);
      }

      ProjectController(DebugProcessBase* process, ProjectModel* model)
         : _debugController(process, model)
      {
         _notifier = nullptr;
      }
   };

   // --- IDEController ---
   class IDEController
   {
      NotifierBase* _notifier;

   public:
      SourceViewController sourceController;
      ProjectController    projectController;

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;

         projectController.setNotifier(notifier);
      }

      IDEController(DebugProcessBase* process, ProjectModel* model)
         : projectController(process, model)
      {
      }
   };

} // elena:lang

#endif // IDECONTROLLER_H
