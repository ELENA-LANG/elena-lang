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
      void newSource(TextViewModelBase* model, ustr_t name, bool autoSelect, int& status);
      bool openSource(TextViewModelBase* model, ustr_t name, path_t sourcePath,
         FileEncoding encoding, bool autoSelect, int& status);
      void closeSource(TextViewModelBase* model, int index, bool autoSelect, int& status);

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
      ProcessBase*            _vmProcess;
      DebugController         _debugController;
      NotifierBase*           _notifier;
      WatchContext            _autoWatch;

      PathHelperBase*         _pathHelper;

      void loadConfig(ProjectModel& model, ConfigFile& config, ConfigFile::Node platformRoot);
      void saveConfig(ProjectModel& model, ConfigFile& config, ConfigFile::Node root, ConfigFile::Node platformRoot);

      path_t retrieveSourceName(ProjectModel* model, path_t sourcePath, NamespaceString& retVal, PathString& subPath);

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

      void includeFile(ProjectModel& model, path_t filePath);

      void setProjectPath(ProjectModel& model, path_t projectFile);

      int openSingleFileProject(ProjectModel& model, path_t singleProjectFile);
      int newProject(ProjectModel& model);
      int openProject(ProjectModel& model, path_t projectFile);
      void saveProject(ProjectModel& model);
      int closeProject(ProjectModel& model);

      path_t getSourceByIndex(ProjectModel& model, int index);

      void defineSourceName(ProjectModel* model, path_t path, NamespaceString& retVal);

      void defineFullPath(ProjectModel& model, ustr_t ns, path_t path, PathString& fullPath);

      bool doCompileProject(ProjectModel& model, DebugAction postponedAction);

      bool startVMConsole(ProjectModel& model);
      void stopVMConsole();

      bool doDebugAction(ProjectModel& model, SourceViewModel& sourceModel, DebugAction action);
      void doDebugStop(ProjectModel& model);

      void runToCursor(ProjectModel& model, SourceViewModel& sourceModel);
      void refreshDebugContext(ContextBrowserBase* contextBrowser);
      void refreshDebugContext(ContextBrowserBase* contextBrowser, size_t param, addr_t address);

      bool toggleBreakpoint(ProjectModel& model, SourceViewModel& sourceModel, int row, DocumentChangeStatus& status);

      void loadBreakpoints(ProjectModel& model);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;
      }

      void notify(EventBase* event) override
      {
         if (_notifier)
            _notifier->notify(event);
      }

      ProjectController(ProcessBase* outputProcess, ProcessBase* vmConsoleProcess, DebugProcessBase* debugProcess, 
         ProjectModel* model, SourceViewModel* sourceModel,
         DebugSourceController* sourceController, PlatformType platform, PathHelperBase* pathHelper)
         : _outputProcess(outputProcess), _vmProcess(vmConsoleProcess), _debugController(debugProcess, model, sourceModel, sourceController),
           _autoWatch({ nullptr, 0 }),
           _pathHelper(pathHelper)
      {
         _notifier = nullptr;
         _platform = platform;
      }
   };

   // --- IDEController ---
   class IDEController : public DebugSourceController
   {
      NotifierBase*           _notifier;

      bool openFile(SourceViewModel* model, ProjectModel* projectModel, path_t sourceFile, int& status);
      bool openFile(IDEModel* model, path_t sourceFile, int& status);
      int openProject(IDEModel* model, path_t projectFile);
      bool closeProject(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, 
         int& status);

      bool closeFile(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, 
         int index, int& status);
      bool saveFile(FileDialogBase& dialog, IDEModel* model, int index, bool forcedMode);
      bool closeAll(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, 
         int& status);
      bool closeAllButActive(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model,
         int& status);
      bool saveAll(FileDialogBase& dialog, IDEModel* model, bool forcedMode);

      void displayErrors(IDEModel* model, text_str output, ErrorLogBase* log);

      void onCompilationStart(IDEModel* model);
      void onCompilationStop(IDEModel* model);
      void onCompilationBreak(IDEModel* model);

      void notifyOnModelChange(int projectStatus);

   public:
      FileEncoding         defaultEncoding;

      SourceViewController sourceController;
      ProjectController    projectController;

      void loadSystemConfig(IDEModel* model, path_t configPath, ustr_t typeXPath, ustr_t platformXPath);

      bool loadConfig(IDEModel* model, path_t configPath);
      void saveConfig(IDEModel* model, path_t configPath);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;

         projectController.setNotifier(notifier);
         sourceController.setNotifier(notifier);
      }

      path_t retrieveSingleProjectFile(IDEModel* model);

      //bool openFile(IDEModel* model, path_t sourceFile);
      bool doOpenProjectSourceByIndex(IDEModel* model, int index);

      bool selectSource(ProjectModel* model, SourceViewModel* sourceModel,
         ustr_t moduleName, path_t sourcePath);
      void traceSource(SourceViewModel* sourceModel, bool found, int row);
      void onProgramFinish(SourceViewModel* sourceModel);

      void highlightError(IDEModel* model, int row, int column, path_t path);

      void doNewFile(IDEModel* model);
      void doOpenFile(FileDialogBase& dialog, IDEModel* model);
      void doOpenFile(IDEModel* model, path_t path);
      bool doSaveFile(FileDialogBase& dialog, IDEModel* model, bool saveAsMode, bool forcedSave);
      bool doSaveAll(FileDialogBase& dialog, FileDialogBase& projectDialog, IDEModel* model);
      bool doCloseFile(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model);
      bool doCloseFile(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, int index);
      bool doCloseAll(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model);
      bool doCloseAllButActive(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model);
      void doNewProject(FileDialogBase& dialog, MessageDialogBase& mssgDialog, ProjectSettingsBase& prjDialog, 
         IDEModel* model);
      bool doOpenProject(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model);
      void doOpenProject(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, path_t path);
      bool doCloseProject(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model);
      bool doSaveProject(FileDialogBase& dialog, FileDialogBase& projectDialog, IDEModel* model, bool forcedMode);

      bool doSearch(FindDialogBase& dialog, IDEModel* model);
      bool doSearchNext(IDEModel* model);
      bool doReplace(FindDialogBase& dialog, MessageDialogBase& qusetionDialog, IDEModel* model);
      void doGoToLine(GotoDialogBase& dialog, IDEModel* model);

      bool doCompileProject(FileDialogBase& dialog, FileDialogBase& projectDialog, IDEModel* model);
      void doChangeProject(ProjectSettingsBase& prjDialog, IDEModel* model);
      void doDebugAction(IDEModel* model, DebugAction action);
      void doDebugStop(IDEModel* model);

      void doStartVMConsole(IDEModel* model);
      void doStopVMConsole();

      void doIndent(IDEModel* model);
      void doOutdent(IDEModel* model);

      void doConfigureEditorSettings(EditorSettingsBase& editorDialog, IDEModel* model);

      void refreshDebugContext(ContextBrowserBase* contextBrowser, IDEModel* model);
      void refreshDebugContext(ContextBrowserBase* contextBrowser, IDEModel* model, size_t item, size_t param);

      void toggleBreakpoint(IDEModel* model, int row);

      void doSelectNextWindow(IDEModel* model);
      void doSelectPrevWindow(IDEModel* model);
      void doSelectWindow(TextViewModelBase* viewModel, path_t path);

      void doSelectWindow(FileDialogBase& fileDialog, MessageDialogBase& mssgDialog, WindowListDialogBase& dialog, 
         IDEModel* model);

      void onCompilationCompletion(IDEModel* model, int exitCode, 
         text_str output, ErrorLogBase* log);
      void onDebuggerHook(IDEModel* model);
      void onDebuggerStop(IDEModel* model);
      void onStatusChange(IDEModel* model, IDEStatus newStatus);
      void onDebuggerNoSource(MessageDialogBase& mssgDialog, IDEModel* model);
      void onProgramRuning(IDEModel* model);
      void onDocSelection(IDEModel* model, int index);
      void onProgramStop(IDEModel* model);

      void doInclude(IDEModel* model);

      bool doExit(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model);

      bool onClose(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model);

      void init(IDEModel* model, int& status);


      IDEController(ProcessBase* outputProcess, ProcessBase* vmConsoleProcess, DebugProcessBase* process,
         IDEModel* model, TextViewSettings& textViewSettings, PlatformType platform, PathHelperBase* pathHelper
      ) :
         sourceController(textViewSettings),
         projectController(outputProcess, vmConsoleProcess, process, &model->projectModel, &model->sourceViewModel,
            this, platform, pathHelper)
      {
         _notifier = nullptr;
         defaultEncoding = FileEncoding::UTF8;
      }
   };

} // elena:lang

#endif // IDECONTROLLER_H
