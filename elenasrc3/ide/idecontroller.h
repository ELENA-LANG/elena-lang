//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller header File
//                                             (C)2021-2025, by Aleksey Rakov
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
      void newSource(TextViewModelBase* model, ustr_t name, bool included, bool autoSelect, int& status);
      bool openSource(TextViewModelBase* model, ustr_t name, path_t sourcePath,
         FileEncoding encoding, bool included, bool autoSelect, int& status);
      void closeSource(TextViewModelBase* model, int index, bool autoSelect, int& status);

      void renameSource(TextViewModelBase* model, ustr_t oldName, ustr_t newName, path_t newSourcePath);

      void saveSource(TextViewModelBase* model, ustr_t name);

      SourceViewController()
         : TextViewController()
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

   struct DebugActionResult
   {
      bool outaged;
      bool targetMissing;
      bool noDebugFile;

      DebugActionResult()
      {
         outaged = targetMissing = noDebugFile = false;
      }
   };

   typedef bool(*CompareFileDateTime)(path_t, path_t);

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

      CompareFileDateTime     _compareFileModifiedTime;

      void loadConfig(ProjectModel& model, ConfigFile& config, ConfigFile::Node platformRoot);
      void saveConfig(ProjectModel& model, ConfigFile& config, ConfigFile::Node root, ConfigFile::Node platformRoot);

      void loadProfileList(ProjectModel& model, ConfigFile& config, ConfigFile::Node platformRoot);

      path_t retrieveSourceName(ProjectModel* model, path_t sourcePath, NamespaceString& retVal, PathString& subPath);

      bool isOutaged(ProjectModel& projectModel, SourceViewModel& sourceModel);

      bool startDebugger(ProjectModel& model, DebugActionResult& result);

      bool isIncluded(ProjectModel& model, ustr_t ns);

      bool compileProject(ProjectModel& model, int postponedAction);
      bool compileSingleFile(ProjectModel& model, int postponedAction);

   public:
      bool isStarted()
      {
         return _debugController.isStarted();
      }

      bool isFileIncluded(ProjectModel& model, path_t filePath);

      void includeFile(ProjectModel& model, path_t filePath);
      void excludeFile(ProjectModel& model, path_t filePath);

      void setProjectPath(ProjectModel& model, path_t projectFile);

      int openSingleFileProject(ProjectModel& model, path_t singleProjectFile);
      int newProject(ProjectModel& model);
      int openProject(ProjectModel& model, path_t projectFile);
      int saveProject(ProjectModel& model);
      int closeProject(ProjectModel& model);

      path_t getSourceByIndex(ProjectModel& model, int index);

      void defineSourceName(ProjectModel* model, path_t path, NamespaceString& retVal);

      void defineFullPath(ProjectModel& model, ustr_t ns, path_t path, PathString& fullPath);

      bool doCompileProject(ProjectModel& model, DebugAction postponedAction);

      bool startVMConsole(ProjectModel& model);
      void stopVMConsole();

      bool onDebugAction(ProjectModel& model, SourceViewModel& sourceModel, DebugAction action,
         DebugActionResult& result, bool withoutPostponeAction);

      void doDebugAction(ProjectModel& model, SourceViewModel& sourceModel, DebugAction action);
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

      ProjectController(ProcessBase* outputProcess, ProcessBase* vmConsoleProcess, IDEDebugProcessBase* debugProcess,
         ProjectModel* model, SourceViewModel* sourceModel,
         DebugSourceController* sourceController, PlatformType platform, PathHelperBase* pathHelper, CompareFileDateTime comparer)
         : _outputProcess(outputProcess), _vmProcess(vmConsoleProcess), _debugController(debugProcess, model, sourceModel, sourceController),
           _autoWatch({ nullptr, 0 }),
           _pathHelper(pathHelper)
      {
         _notifier = nullptr;
         _platform = platform;
         _compareFileModifiedTime = comparer;
      }
   };

   // --- IDEController ---
   class IDEController : public DebugSourceController
   {
      NotifierBase*           _notifier;

      bool openFile(SourceViewModel* model, ProjectModel* projectModel, path_t sourceFile, int& status);
      bool openFile(IDEModel* model, path_t sourceFile, int& status);
      int openProject(IDEModel* model, path_t projectFile);
      bool closeProject(IDEModel* model, int& status);

      bool closeFile(IDEModel* model, int index, int& status);
      //bool saveFile(FileDialogBase& dialog, IDEModel* model, int index, bool forcedMode);
      bool closeAll(IDEModel* model, int& status);
      bool closeAllButActive(IDEModel* model, int& status);
      //bool saveAll(FileDialogBase& dialog, IDEModel* model, bool forcedMode);
      //bool saveProject(FileDialogBase& projectDialog, IDEModel* model, bool saveAsMode, int& status);

      void displayErrors(IDEModel* model, text_str output, ErrorLogBase* log);

      void onCompilationStart(IDEModel* model);

      void notifyOnModelChange(int projectStatus);

   public:
      FileEncoding         defaultEncoding;

      SourceViewController sourceController;
      ProjectController    projectController;

      void addToRecentProjects(IDEModel* model, path_t path);

      void loadSystemConfig(IDEModel* model, path_t configPath, ustr_t typeXPath, ustr_t platformXPath);

      bool loadConfig(IDEModel* model, path_t configPath, GUISettinngs& guiSettings);
      void saveConfig(IDEModel* model, path_t configPath, GUISettinngs& guiSettings);

      void setNotifier(NotifierBase* notifier)
      {
         _notifier = notifier;

         projectController.setNotifier(notifier);
         sourceController.setNotifier(notifier);
      }

      path_t retrieveSingleProjectFile(IDEModel* model);

      //bool openFile(IDEModel* model, path_t sourceFile);
      bool doOpenProjectSourceByIndex(IDEModel* model, int index);

      void traceStart(ProjectModel* model) override;
      bool selectSource(ProjectModel* model, SourceViewModel* sourceModel,
         ustr_t moduleName, path_t sourcePath) override;
      void traceStep(SourceViewModel* sourceModel, bool found, int row) override;
      void traceFinish(SourceViewModel* sourceModel) override;

      void highlightError(IDEModel* model, int row, int column, path_t path);

      void doNewFile(IDEModel* model);
      bool doOpenFile(IDEModel* model, PathList& files);
      void doOpenFile(IDEModel* model, path_t path);

      bool ifFileUnnamed(IDEModel* model, int index = -1);
      bool ifFileNotSaved(IDEModel* model, int index);
      bool ifProjectNotSaved(IDEModel* model);
      bool ifProjectUnnamed(IDEModel* model);

      bool doSaveFile(IDEModel* model, int index, bool forcedSave, path_t filePath = nullptr);

      bool doCloseFile(IDEModel* model, int index);
      bool doCloseAll(IDEModel* model, bool closeProjectMode);
      bool doCloseAllButActive(IDEModel* model);
      void doNewProject(IDEModel* model);
      void doOpenProject(IDEModel* model, path_t path);
      //bool doCloseProject(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model);
      bool doSaveProject(IDEModel* model, path_t newPath = nullptr);

      bool doSearch(FindDialogBase& dialog, IDEModel* model);
      bool doSearchNext(IDEModel* model);
      bool doReplace(FindDialogBase& dialog, MessageDialogBase& qusetionDialog, IDEModel* model);
      void doGoToLine(GotoDialogBase& dialog, IDEModel* model);

      bool doCompileProject(IDEModel* model);
      void doChangeProject(ProjectSettingsBase& prjDialog, IDEModel* model);
      void doDebugAction(IDEModel* model, DebugAction action,
         MessageDialogBase& mssgDialog, bool withoutPostponeAction);
      void doDebugStop(IDEModel* model);

      void doStartVMConsole(IDEModel* model);
      void doStopVMConsole();

      void doIndent(IDEModel* model);
      void doOutdent(IDEModel* model);

      void doConfigureFontSettings(FontDialogBase& editorDialog, IDEModel* model);
      void doConfigureEditorSettings(EditorSettingsBase& editorDialog, IDEModel* model);
      void doConfigureIDESettings(IDESettingsBase& editorDialog, IDEModel* model);
      void doConfigureDebuggerSettings(DebuggerSettingsBase& editorDialog, IDEModel* model);

      void refreshDebugContext(ContextBrowserBase* contextBrowser, IDEModel* model);
      void refreshDebugContext(ContextBrowserBase* contextBrowser, IDEModel* model, size_t item, size_t param);

      void toggleBreakpoint(IDEModel* model, int row);

      void doSelectNextWindow(IDEModel* model);
      void doSelectPrevWindow(IDEModel* model);
      void doSelectWindow(TextViewModelBase* viewModel, path_t path);

      void onCompilationCompletion(IDEModel* model, int exitCode,
         text_str output, ErrorLogBase* log);
      void onDebuggerHook(ProjectModel* model);
      void onDebuggerStep(IDEModel* model);
      void onDebuggerStop(IDEModel* model);
      void onDebuggerNoSource(MessageDialogBase& mssgDialog, IDEModel* model);
      void onDocSelection(IDEModel* model, int index);

      void onIDEStop(IDEModel* model, GUISettinngs& guiSettings);

      void doInclude(IDEModel* model);
      void doExclude(IDEModel* model);

      //bool doExit(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model);

      bool onClose(IDEModel* model);

      void init(IDEModel* model, int& status);

      IDEController(ProcessBase* outputProcess, ProcessBase* vmConsoleProcess, IDEDebugProcessBase* process,
         IDEModel* model, PlatformType platform, PathHelperBase* pathHelper, CompareFileDateTime comparer
      ) :
         sourceController(),
         projectController(outputProcess, vmConsoleProcess, process, &model->projectModel, &model->sourceViewModel,
            this, platform, pathHelper, comparer)
      {
         _notifier = nullptr;
         defaultEncoding = FileEncoding::UTF8;
      }
   };

} // elena:lang

#endif // IDECONTROLLER_H
