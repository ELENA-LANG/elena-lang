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
#include "config.h"

namespace elena_lang
{
   // --- SourceViewController ---
   class SourceViewController : public TextViewController
   {
   public:
      void newSource(TextViewModelBase* model, ustr_t name, bool autoSelect);
      bool openSource(TextViewModelBase* model, ustr_t name, path_t sourcePath,
         FileEncoding encoding, bool autoSelect);
      void closeSource(TextViewModelBase* model, ustr_t name, bool autoSelect);

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
      PlatformType            _platform;

      ProcessBase*            _outputProcess;
      DebugController         _debugController;
      NotifierBase*           _notifier;

      void loadConfig(ProjectModel& model, ConfigFile& config, ConfigFile::Node platformRoot);

      bool onDebugAction(ProjectModel& model, DebugAction action);
      bool isOutaged(bool noWarning);

      bool startDebugger(ProjectModel& model/*, bool stepMode*/);

      bool isIncluded(ProjectModel& model, ustr_t ns);

      bool compileProject(ProjectModel& model);
      bool compileSingleFile(ProjectModel& model);

   public:
      void openSingleFileProject(ProjectModel& model, path_t singleProjectFile);
      void openProject(ProjectModel& model, path_t projectFile);

      path_t getSourceByIndex(ProjectModel& model, int index);

      void defineSourceName(path_t path, IdentifierString& retVal);

      void defineFullPath(ProjectModel& model, ustr_t ns, path_t path, PathString& fullPath);

      bool doCompileProject(ProjectModel& model, DebugAction postponedAction);

      void doDebugAction(ProjectModel& model, DebugAction action);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;
      }

      void notifyMessage(int messageCode, int arg1 = 0, int arg2 = 0) override
      {
         if (_notifier)
            _notifier->notifyMessage(messageCode, arg1);
      }
      void notifyModelChange(int modelCode, int arg) override
      {
         if (_notifier)
            _notifier->notifyModelChange(modelCode, arg);
      }

      ProjectController(ProcessBase* outputProcess, DebugProcessBase* debugProcess, ProjectModel* model, SourceViewModel* sourceModel,
         DebugSourceController* sourceController, PlatformType platform)
         : _outputProcess(outputProcess), _debugController(debugProcess, model, sourceModel, this, sourceController)
      {
         _notifier = nullptr;
         _platform = platform;
      }
   };

   // --- IDEController ---
   class IDEController : public DebugSourceController
   {
      NotifierBase*           _notifier;

      bool openFile(SourceViewModel* model, path_t sourceFile);
      bool openProject(IDEModel* model, path_t projectFile);

      void displayErrors(IDEModel* model, text_str output, ErrorLogBase* log);

      void onCompilationStart(IDEModel* model);
      void onCompilationStop(IDEModel* model);
      void onCompilationBreak(IDEModel* model);

   public:
      FileEncoding         defaultEncoding;

      SourceViewController sourceController;
      ProjectController    projectController;

      bool loadConfig(IDEModel* model, path_t configPath);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;

         projectController.setNotifier(notifier);
      }

      path_t retrieveSingleProjectFile(IDEModel* model);

      bool openFile(IDEModel* model, path_t sourceFile);
      bool openProjectSourceByIndex(IDEModel* model, int index);

      bool selectSource(ProjectModel* model, SourceViewModel* sourceModel,
         ustr_t moduleName, path_t sourcePath);

      void highlightError(IDEModel* model, int row, int column, path_t path);

      void doNewFile(IDEModel* model);
      void doOpenFile(DialogBase& dialog, IDEModel* model);
      bool doSaveFile(DialogBase& dialog, IDEModel* model, bool saveAsMode, bool forcedSave);
      bool doCloseFile(DialogBase& dialog, IDEModel* model);
      bool doOpenProject(DialogBase& dialog, IDEModel* model);
      bool doCloseProject();
      bool doSaveProject(DialogBase& dialog, IDEModel* model, bool forcedMode);

      bool doCompileProject(DialogBase& dialog, IDEModel* model);
      void doDebugAction(IDEModel* model, DebugAction action);

      void onCompilationCompletion(IDEModel* model, int exitCode, 
         text_str output, ErrorLogBase* log);

      bool doExit();

      void init(IDEModel* model);

      IDEController(ProcessBase* outputProcess, DebugProcessBase* process, IDEModel* model,
         TextViewSettings& textViewSettings, PlatformType platform
      ) :
         sourceController(textViewSettings),
         projectController(outputProcess, process, &model->projectModel, &model->sourceViewModel,
            this, platform)
      {
         _notifier = nullptr;
         defaultEncoding = FileEncoding::UTF8;
      }
   };

} // elena:lang

#endif // IDECONTROLLER_H
