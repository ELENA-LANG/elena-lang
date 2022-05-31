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
      void newSource(TextViewModelBase* model, ustr_t name, bool autoSelect);
      bool openSource(TextViewModelBase* model, ustr_t name, path_t sourcePath,
         FileEncoding encoding, bool autoSelect);

      void renameSource(TextViewModelBase* model, ustr_t oldName, ustr_t newName, path_t newSourcePath);

      void saveSource(TextViewModelBase* model, ustr_t name);

      SourceViewController(TextViewSettings& settings)
         : TextViewController(settings)
      {
      }
   };

   // --- DebugAction ---
   enum class DebugAction
   {
      None,
      Run,
      StepInto,
      StepOver
   };

   // --- ProjectController ---
   class ProjectController : public NotifierBase
   {
      DebugController      _debugController;
      NotifierBase*        _notifier;

      bool onDebugAction(ProjectModel& model, DebugAction action);
      bool isOutaged(bool noWarning);

      bool startDebugger(ProjectModel& model/*, bool stepMode*/);

   public:
      void defineSourceName(path_t path, IdentifierString& retVal);

      bool doCompileProject(ProjectModel& model, DebugAction postponedAction);

      void doDebugAction(ProjectModel& model, DebugAction action);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;
      }

      void notifyMessage(int messageCode) override
      {
         if (_notifier)
            _notifier->notifyMessage(messageCode);
      }
      void notifyModelChange(int modelCode, int arg) override
      {
         if (_notifier)
            _notifier->notifyModelChange(modelCode, arg);
      }

      ProjectController(DebugProcessBase* process, ProjectModel* model, SourceViewModel* sourceModel)
         : _debugController(process, model, sourceModel, this)
      {
         _notifier = nullptr;
      }
   };

   // --- IDEController ---
   class IDEController
   {
      NotifierBase*           _notifier;

      bool openFile(IDEModel* model, path_t sourceFile);

   public:
      FileEncoding         defaultEncoding;

      SourceViewController sourceController;
      ProjectController    projectController;

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;

         projectController.setNotifier(notifier);
      }

      void doNewFile(IDEModel* model);
      void doOpenFile(FileDialogBase& dialog, IDEModel* model);
      void doSaveFile(FileDialogBase& dialog, IDEModel* model, bool saveAsMode);

      bool doExit();

      IDEController(DebugProcessBase* process, IDEModel* model, 
         TextViewSettings& textViewSettings
      ) :
         sourceController(textViewSettings),
         projectController(process, &model->projectModel, &model->sourceViewModel)
      {
         _notifier = nullptr;
         defaultEncoding = FileEncoding::UTF8;
      }
   };

} // elena:lang

#endif // IDECONTROLLER_H
