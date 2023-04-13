//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller header File
//                                             (C)2021-2023, by Aleksey Rakov
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
      void newSource(TextViewModelBase* model, ustr_t name, bool autoSelect, NotificationStatus& status);
      bool openSource(TextViewModelBase* model, ustr_t name, path_t sourcePath,
         FileEncoding encoding, bool autoSelect, NotificationStatus& status);
      void closeSource(TextViewModelBase* model, int index, bool autoSelect, NotificationStatus& status);

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
      StepOver,
      RunTo
   };

   // --- ProjectController ---
   class ProjectController : public NotifierBase
   {
      PlatformType            _platform;

      ProcessBase*            _outputProcess;
      DebugController         _debugController;
      NotifierBase*           _notifier;
      WatchContext            _autoWatch;

      void loadConfig(ProjectModel& model, ConfigFile& config, ConfigFile::Node platformRoot);

      path_t retrieveSourceName(ProjectModel* model, path_t sourcePath, ReferenceName& retVal);

      bool onDebugAction(ProjectModel& model, DebugAction action);
      bool isOutaged(bool noWarning);

      bool startDebugger(ProjectModel& model/*, bool stepMode*/);

      bool isIncluded(ProjectModel& model, ustr_t ns);

      bool compileProject(ProjectModel& model);
      bool compileSingleFile(ProjectModel& model);

   public:
      bool isStarted()
      {
         return _debugController.isStarted();
      }

      NotificationStatus openSingleFileProject(ProjectModel& model, path_t singleProjectFile);
      NotificationStatus openProject(ProjectModel& model, path_t projectFile);
      NotificationStatus closeProject(ProjectModel& model);

      path_t getSourceByIndex(ProjectModel& model, int index);

      void defineSourceName(ProjectModel* model, path_t path, ReferenceName& retVal);

      void defineFullPath(ProjectModel& model, ustr_t ns, path_t path, PathString& fullPath);

      bool doCompileProject(ProjectModel& model, DebugAction postponedAction);

      void doDebugAction(ProjectModel& model, SourceViewModel& sourceModel, DebugAction action);
      void doDebugStop(ProjectModel& model);

      void runToCursor(ProjectModel& model, SourceViewModel& sourceModel);
      void refreshDebugContext(ContextBrowserBase* contextBrowser);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;
      }

      void notify(int id, NotificationStatus status) override
      {
         if (_notifier)
            _notifier->notify(id, status);
      }
      void notifySelection(int id, size_t param) override
      {
         if (_notifier)
            _notifier->notifySelection(id, param);
      }
      void notifyCompletion(int id, int param) override
      {
         if (_notifier)
            _notifier->notifyCompletion(id, param);

      }

      ProjectController(ProcessBase* outputProcess, DebugProcessBase* debugProcess, ProjectModel* model, SourceViewModel* sourceModel,
         DebugSourceController* sourceController, PlatformType platform)
         : _outputProcess(outputProcess), _debugController(debugProcess, model, sourceModel, this, sourceController),
           _autoWatch({ nullptr, 0 }) 
      {
         //_notifier = nullptr;
         _platform = platform;
      }
   };

   // --- IDEController ---
   class IDEController : public DebugSourceController
   {
      NotifierBase*           _notifier;

      bool openFile(SourceViewModel* model, ProjectModel* projectModel, path_t sourceFile, NotificationStatus& status);
      bool openFile(IDEModel* model, path_t sourceFile, NotificationStatus& status);
      bool openProject(IDEModel* model, path_t projectFile, NotificationStatus& status);
      bool closeProject(DialogBase& dialog, IDEModel* model, NotificationStatus& status);

      bool closeFile(DialogBase& dialog, IDEModel* model, int index, NotificationStatus& status);
      bool closeAll(DialogBase& dialog, IDEModel* model, NotificationStatus& status);

      void displayErrors(IDEModel* model, text_str output, ErrorLogBase* log);

      void onCompilationStart(IDEModel* model);
      void onCompilationStop(IDEModel* model);
      void onCompilationBreak(IDEModel* model);

   public:
      FileEncoding         defaultEncoding;

      SourceViewController sourceController;
      ProjectController    projectController;

      bool loadConfig(IDEModel* model, path_t configPath);
      void saveConfig(IDEModel* model, path_t configPath);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;

         projectController.setNotifier(notifier);
      }

      path_t retrieveSingleProjectFile(IDEModel* model);

      //bool openFile(IDEModel* model, path_t sourceFile);
      bool doOpenProjectSourceByIndex(IDEModel* model, int index);

      bool selectSource(ProjectModel* model, SourceViewModel* sourceModel,
         ustr_t moduleName, path_t sourcePath);

      void highlightError(IDEModel* model, int row, int column, path_t path);

      void doNewFile(IDEModel* model);
      void doOpenFile(DialogBase& dialog, IDEModel* model);
      bool doSaveFile(DialogBase& dialog, IDEModel* model, bool saveAsMode, bool forcedSave);
      bool doCloseFile(DialogBase& dialog, IDEModel* model);
      bool doCloseAll(DialogBase& dialog, IDEModel* model);
      bool doOpenProject(DialogBase& dialog, IDEModel* model);
      bool doCloseProject(DialogBase& dialog, IDEModel* model);
      bool doSaveProject(DialogBase& dialog, IDEModel* model, bool forcedMode);

      bool doCompileProject(DialogBase& dialog, IDEModel* model);
      void doDebugAction(IDEModel* model, DebugAction action);
      void doDebugStop(IDEModel* model);

      void refreshDebugContext(ContextBrowserBase* contextBrowser, IDEModel* model);

      void doSelectNextWindow(IDEModel* model);
      void doSelectPrevWindow(IDEModel* model);

      void onCompilationCompletion(IDEModel* model, int exitCode, 
         text_str output, ErrorLogBase* log);
      void onDebuggerStop(IDEModel* model);
      void onStatusChange(IDEModel* model, IDEStatus newStatus);

      bool doExit(DialogBase& dialog, IDEModel* model);

      bool onClose(DialogBase& dialog, IDEModel* model);

      void init(IDEModel* model);

      void onProgramStop(IDEModel* model);

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
