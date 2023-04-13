//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller implementation File
//                                             (C)2005-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <tchar.h>

#include "idecontroller.h"
#include "eng/messages.h"

using namespace elena_lang;

constexpr auto MAX_RECENT_FILES = 10;

inline ustr_t getPlatformName(PlatformType type)
{
   switch (type) {
      case PlatformType::Win_x86:
         return WIN_X86_KEY;
      case PlatformType::Win_x86_64:
         return WIN_X86_64_KEY;
      case PlatformType::Linux_x86:
         return LINUX_X86_KEY;
      case PlatformType::Linux_x86_64:
         return LINUX_X86_64_KEY;
      case PlatformType::Linux_PPC64le:
         return LINUX_PPC64le_KEY;
      case PlatformType::Linux_ARM64:
         return LINUX_ARM64_KEY;
      default:
         return nullptr;
   }
}

// --- SourceViewController ---

void SourceViewController :: newSource(TextViewModelBase* model, ustr_t caption, bool autoSelect, NotificationStatus& status)
{
   IdentifierString tabName("unnamed");

   bool empty = model->empty;

   newDocument(model, caption);

   if (empty)
      status |= FRAME_VISIBILITY_CHANGED;

   if (autoSelect) {
      int index = model->getDocumentIndex(caption);

      selectDocument(model, index, status);
   }
}

bool SourceViewController :: openSource(TextViewModelBase* model, ustr_t caption, path_t sourcePath, 
   FileEncoding encoding, bool autoSelect, NotificationStatus& status)
{
   bool empty = model->empty;

   if (openDocument(model, caption, sourcePath, encoding)) {
      if (empty)
         status |= FRAME_VISIBILITY_CHANGED;

      if (autoSelect) {
         int index = model->getDocumentIndex(caption);

         selectDocument(model, index, status);
      }

      return true;
   }
   return false;
}

void SourceViewController :: renameSource(TextViewModelBase* model, ustr_t oldName, ustr_t newName, path_t newSourcePath)
{
   model->renameDocumentView(oldName, newName, newSourcePath);
}

void SourceViewController :: closeSource(TextViewModelBase* model, int index, bool autoSelect, NotificationStatus& status)
{
   if (index > 0) {
      int count = model->getDocumentCount();

      closeDocument(model, index, status);

      if (autoSelect && !model->empty) {
         if (index == count) {
            selectDocument(model, count - 1, status);
         }
         else selectDocument(model, index, status);
      }

      status |= IDE_LAYOUT_CHANGED;
      status |= FRAME_VISIBILITY_CHANGED;
   }
}

void SourceViewController :: saveSource(TextViewModelBase* model, ustr_t name)
{
   int index = model->getDocumentIndex(name);

   auto docInfo = model->getDocument(index);
   path_t path = model->getDocumentPath(index);

   if (docInfo) {
      docInfo->save(path);

      //model->onModelModeChanged(model->getDocumentIndex(name));
   }
}

// --- ProjectController ---

bool ProjectController::isIncluded(ProjectModel& model, ustr_t ns)
{
   return NamespaceString::isIncluded(model.getPackage(), ns);
}

void ProjectController :: defineFullPath(ProjectModel& model, ustr_t ns, path_t path, 
   PathString& fullPath)
{
   if (isIncluded(model, ns)) {
      fullPath.copy(*model.projectPath);
      fullPath.combine(path);
   }
   else {
      fullPath.copy(*model.paths.librarySourceRoot);
      // HOTFIX : ignore sub ns
      size_t index = ns.find('\'');
      if (index != NOTFOUND_POS) {
         fullPath.combine(ns, index);
      }
      else fullPath.combine(ns);
      fullPath.combine(path);
   }
}

path_t ProjectController :: retrieveSourceName(ProjectModel* model, path_t sourcePath, ReferenceName& name)
{
   size_t projectPathLen = model->projectPath.length();

   PathString path;
   path.copySubPath(sourcePath, true);

   if (!model->projectPath.empty() && (*path).compareSub(*model->projectPath, 0, projectPathLen)) {
      name.copy(model->getPackage());
      if (path.length() > projectPathLen) {
         name.pathToName(*path + projectPathLen);
      }
      return sourcePath + projectPathLen;
   }
   else {
      path_t rootPath = *model->paths.librarySourceRoot;
      size_t rootPathLen = rootPath.length();

      if (!rootPath.empty() && PathUtil::compare(sourcePath, rootPath, rootPathLen)) {
         name.pathToName(sourcePath + rootPathLen);

         return sourcePath + rootPathLen;
      }
      else {
         FileNameString fileName(sourcePath);
         IdentifierString tmp(*fileName);

         name.copy(*tmp);
      }
   }

   return sourcePath;
}

void ProjectController :: defineSourceName(ProjectModel* model, path_t path, ReferenceName& retVal)
{
   if (path.empty()) {
      retVal.copy("undefined");
   }
   else {
      //_ELENA_::ReferenceNs module;
      retrieveSourceName(model, path, retVal);
      retVal.append(':');

      FileNameString fileName(path, true);
      IdentifierString tmp(*fileName);
      retVal.append(*tmp);
   }
}

bool ProjectController :: startDebugger(ProjectModel& model)
{
   ustr_t target = model.getTarget();
   ustr_t arguments = model.getArguments();

   if (!target.empty()) {
      PathString exePath(*model.projectPath, target);

      // provide the whole command line including the executable path and name
      PathString commandLine(exePath);
      commandLine.append(_T(" "));
      commandLine.append(arguments);

      bool debugMode = model.getDebugMode();
      if (debugMode) {
         if (!_debugController.start(exePath.str(), commandLine.str(), debugMode/*, _breakpoints */)) {
            notifyCompletion(NOTIFY_DEBUGGER_RESULT, ERROR_DEBUG_FILE_NOT_FOUND_COMPILE);

            return false;
         }

      }
      else {
         if (!_debugController.start(exePath.str(), commandLine.str(), false/*, _breakpoints */)) {
            notifyCompletion(NOTIFY_DEBUGGER_RESULT, ERROR_RUN_NEED_RECOMPILE);

            return false;
         }
      }

      return true;
   }
   else {
      notifyCompletion(NOTIFY_DEBUGGER_RESULT, ERROR_RUN_NEED_TARGET);

      return false;
   }
}

bool ProjectController :: isOutaged(bool noWarning)
{
   return false; // !! temporal
}

bool ProjectController :: onDebugAction(ProjectModel& model, DebugAction action)
{
   if (model.getStatus() == IDEStatus::Busy)
      return false;

   if (!_debugController.isStarted()) {
      bool toRecompile = model.autoRecompile && !testIDEStatus(model.getStatus(), IDEStatus::AutoRecompiling);
      if (isOutaged(toRecompile)) {
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

void ProjectController :: doDebugAction(ProjectModel& model, SourceViewModel& sourceModel , DebugAction action)
{
   if (model.getStatus() != IDEStatus::Busy) {
      if (onDebugAction(model, action)) {
         switch (action) {
            case DebugAction::Run:
               _debugController.run();
               break;
            case DebugAction::StepInto:
               _debugController.stepInto();
               break;
            case DebugAction::StepOver:
               _debugController.stepOver();
               break;
            case DebugAction::RunTo:
               runToCursor(model, sourceModel);
               break;
            default:
               break;
         }
      }
   }
}

void ProjectController :: doDebugStop(ProjectModel& model)
{
   if (model.getStatus() != IDEStatus::Busy) {
      _debugController.stop();
   }
}
void ProjectController :: runToCursor(ProjectModel& model, SourceViewModel& sourceModel)
{
   auto currentDoc = sourceModel.DocView();
   if (currentDoc != nullptr) {
      int index = sourceModel.getCurrentIndex();
      ustr_t currentSource = sourceModel.getDocumentName(index);
      path_t currentPath = sourceModel.getDocumentPath(index);

      ReferenceName ns;
      currentPath = retrieveSourceName(&model, currentPath, ns);

      IdentifierString pathStr(currentPath);
      _debugController.runToCursor(*ns, *pathStr, currentDoc->getCaret().y);
   }
}

bool ProjectController :: compileProject(ProjectModel& model)
{
   PathString appPath(model.paths.appPath);
   appPath.combine(*model.paths.compilerPath);

   PathString cmdLine(*model.paths.compilerPath);
   cmdLine.append(" ");
   cmdLine.append(*model.projectFile);

   PathString curDir;
   curDir.append(*model.projectPath);

   return _outputProcess->start(*appPath, *cmdLine, *model.projectPath, true);
}

bool ProjectController :: compileSingleFile(ProjectModel& model)
{
   path_t singleProjectFile = model.sources.get(1);

   PathString appPath(model.paths.appPath);
   appPath.combine(*model.paths.compilerPath);

   PathString cmdLine(*model.paths.compilerPath);
   cmdLine.append(" ");
   cmdLine.append(singleProjectFile);

   PathString curDir;
   curDir.append(*model.projectPath);

   return _outputProcess->start(*appPath, *cmdLine, *model.projectPath, true);
}

bool ProjectController :: doCompileProject(ProjectModel& model, DebugAction postponedAction)
{
   if (model.singleSourceProject) {
      return compileSingleFile(model);
   }
   else if (!model.name.empty()) {
      return compileProject(model);
   }
   else return false;   
}

void ProjectController :: loadConfig(ProjectModel& model, ConfigFile& config, ConfigFile::Node configRoot)
{
   // load settings
   DynamicString<char> value;
   auto targetOption = config.selectNode(configRoot, TARGET_SUB_CATEGORY);
   if (!targetOption.isNotFound()) {
      targetOption.readContent(value);

      model.target.copy(value.str());
   }

   // load source files
   DynamicString<char> subNs;
   DynamicString<char> path;

   ConfigFile::Collection modules;
   if (config.select(configRoot, MODULE_CATEGORY, modules)) {
      for (auto m_it = modules.start(); !m_it.eof(); ++m_it) {
         ConfigFile::Node moduleNode = *m_it;

         if (!moduleNode.readAttribute("name", subNs)) {
            subNs.clear();
         }

         ConfigFile::Collection files;
         if (config.select(moduleNode, "*", files)) {
            for (auto it = files.start(); !it.eof(); ++it) {
               // add source file
               ConfigFile::Node node = *it;
               node.readContent(path);

               PathString filePath(path.str());
               model.sources.add((*filePath).clone());
            }
         }
      }
   }
}

NotificationStatus ProjectController :: openProject(ProjectModel& model, path_t projectFile)
{
   ustr_t key = getPlatformName(_platform);

   FileNameString src(projectFile, true);
   FileNameString name(projectFile);

   model.sources.clear();

   model.empty = false;
   model.name.copy(*name);
   model.projectFile.copy(*src);
   model.projectPath.copySubPath(projectFile, true);

   model.singleSourceProject = false;

   ConfigFile projectConfig;
   if (projectConfig.load(projectFile, FileEncoding::UTF8)) {
      DynamicString<char> value;

      ConfigFile::Node root = projectConfig.selectRootNode();

      ConfigFile::Node nsNode = projectConfig.selectNode(NAMESPACE_CATEGORY);
      nsNode.readContent(value);
      model.package.copy(value.str());

      // select platform configuration
      ConfigFile::Node platformRoot = projectConfig.selectNode<ustr_t>(PLATFORM_CATEGORY, key, [](ustr_t key, ConfigFile::Node& node)
         {
            return node.compareAttribute("key", key);
         });

      loadConfig(model, projectConfig, root);
      loadConfig(model, projectConfig, platformRoot);
   }

   return PROJECT_CHANGED;
}

NotificationStatus ProjectController :: closeProject(ProjectModel& model)
{
   model.empty = true;
   model.name.clear();
   model.projectFile.clear();
   model.projectPath.clear();

   model.singleSourceProject = false;

   model.sources.clear();

   model.package.clear();
   model.target.clear();
   model.outputPath.clear();

   return PROJECT_CHANGED;
}

NotificationStatus ProjectController :: openSingleFileProject(ProjectModel& model, path_t singleProjectFile)
{
   FileNameString src(singleProjectFile, true);
   FileNameString name(singleProjectFile);

   model.sources.clear();

   model.empty = false;
   model.name.copy(*name);
   model.projectPath.copySubPath(singleProjectFile, true);
   
   model.outputPath.copySubPath(singleProjectFile, false);
   model.singleSourceProject = true;

   model.sources.add((*src).clone());

   IdentifierString tmp(*name);
   model.package.copy(*tmp);
   model.target.copy(*tmp);
   model.target.append(".exe");

   return PROJECT_CHANGED;
}

path_t ProjectController :: getSourceByIndex(ProjectModel& model, int index)
{
   if (index < 0)
      return nullptr;

   return model.sources.get(index + 1);
}

void ProjectController :: refreshDebugContext(ContextBrowserBase* contextBrowser)
{
   WatchItems refreshedItems;

   _debugController.readAutoContext(contextBrowser, 4, &refreshedItems);

   contextBrowser->removeUnused(refreshedItems);
}

// --- IDEController ---

inline int loadSetting(ConfigFile& config, ustr_t xpath, int defValue)
{
   // read target type; merge it with platform if required
   ConfigFile::Node targetType = config.selectNode(xpath);
   if (!targetType.isNotFound()) {
      DynamicString<char> key;
      targetType.readContent(key);

      return key.toInt();
   }
   else return defValue;
}

inline void loadRecentFiles(ConfigFile& config, ustr_t xpath, ProjectPaths& paths)
{
   DynamicString<char> path;

   ConfigFile::Collection list;
   if (config.select(xpath, list)) {
      for (auto m_it = list.start(); !m_it.eof(); ++m_it) {
         ConfigFile::Node pathNode = *m_it;
         pathNode.readContent(path);

         PathString filePath(path.str());

         paths.add((*filePath).clone());
      }
   }
}

inline void saveSetting(ConfigFile& config, ustr_t xpath, int value)
{
   String<char, 15> number;
   number.appendInt(value);

   config.appendSetting(xpath, number.str());
}

inline void saveRecentFiles(ConfigFile& config, ustr_t xpath, ProjectPaths& paths)
{
   for (int index = paths.count(); index > 0; index--) {
      path_t path = paths.get(index);

      IdentifierString pathStr(path);

      config.appendSetting(xpath, *pathStr);
   }
}

bool IDEController :: loadConfig(IDEModel* model, path_t path)
{
   model->projectModel.paths.configPath.copy(path);

   ConfigFile config;
   if (config.load(path, FileEncoding::UTF8)) {
      model->appMaximized = loadSetting(config, MAXIMIZED_SETTINGS, -1) != 0;
      model->sourceViewModel.fontSize = loadSetting(config, FONTSIZE_SETTINGS, 12);
      model->sourceViewModel.schemeIndex = loadSetting(config, SCHEME_SETTINGS, 1);

      loadRecentFiles(config, RECENTFILES_SETTINGS, model->projectModel.lastOpenFiles);

      return true;
   }
   else {
      return false;
   }

}

void IDEController :: saveConfig(IDEModel* model, path_t configPath)
{
   ConfigFile config(ROOT_NODE);

   saveSetting(config, MAXIMIZED_SETTINGS, model->appMaximized);
   saveSetting(config, FONTSIZE_SETTINGS, model->sourceViewModel.fontSize);
   saveSetting(config, SCHEME_SETTINGS, model->sourceViewModel.schemeIndex);

   saveRecentFiles(config, RECENTFILE_SETTINGS, model->projectModel.lastOpenFiles);

   config.save(*model->projectModel.paths.configPath, FileEncoding::UTF8);
}

void IDEController :: init(IDEModel* model)
{
   NotificationStatus status = IDE_STATUS_CHANGED;

   if (model->projectModel.lastOpenFiles.count() > 0) {
      path_t path = model->projectModel.lastOpenFiles.get(1);

      if (openFile(model, path, status)) {
         model->changeStatus(IDEStatus::Ready);
      }
   }
   
   _notifier->notify(NOTIFY_IDE_CHANGE, status);
}

bool IDEController :: selectSource(ProjectModel* model, SourceViewModel* sourceModel,
   ustr_t ns, path_t sourcePath)
{
   PathString fullPath;
   projectController.defineFullPath(*model, ns, sourcePath, fullPath);

   NotificationStatus status = NONE_CHANGED;
   if (openFile(sourceModel, model, *fullPath, status)) {
      _notifier->notify(NOTIFY_IDE_CHANGE, status);

      return true;
   }
   return false;
}

void IDEController :: doNewFile(IDEModel* model)
{
   NotificationStatus status = NONE_CHANGED;
   ReferenceName sourceNameStr;
   projectController.defineSourceName(&model->projectModel, nullptr, sourceNameStr);

   sourceController.newSource(&model->sourceViewModel, *sourceNameStr, true, status);

   _notifier->notify(NOTIFY_IDE_CHANGE, status);
}

bool IDEController :: openFile(IDEModel* model, path_t sourceFile, NotificationStatus& status)
{
   if (model->projectModel.name.empty()) {
      status |= projectController.openSingleFileProject(model->projectModel, sourceFile);
   }

   return openFile(&model->sourceViewModel, &model->projectModel, sourceFile, status);
}

bool IDEController :: openFile(SourceViewModel* model, ProjectModel* projectModel, path_t sourceFile, NotificationStatus& status)
{
   ustr_t sourceName = model->getDocumentNameByPath(sourceFile);
   int index = sourceName.empty() ? 0 : model->getDocumentIndex(sourceName);

   if (index > 0) {
      return sourceController.selectDocument(model, index, status);
   }
   else {
      ReferenceName sourceNameStr;
      projectController.defineSourceName(projectModel, sourceFile, sourceNameStr);

      sourceName = *sourceNameStr;
   }

   bool retVal = sourceController.openSource(model, sourceName, sourceFile,
      defaultEncoding, true, status);

   if (test(status, FRAME_VISIBILITY_CHANGED))
      status |= IDE_LAYOUT_CHANGED;

   return retVal;
}

bool IDEController :: doOpenProjectSourceByIndex(IDEModel* model, int index)
{
   NotificationStatus status = NONE_CHANGED;

   path_t sourcePath = projectController.getSourceByIndex(model->projectModel, index);
   if (!sourcePath.empty()) {
      PathString fullPath(*model->projectModel.projectPath, sourcePath);

      if(openFile(model, *fullPath, status)) {
         _notifier->notify(NOTIFY_IDE_CHANGE, status);

         return true;
      }
   }
   return false;
}

bool IDEController :: openProject(IDEModel* model, path_t projectFile, NotificationStatus& status)
{
   status |= projectController.openProject(model->projectModel, projectFile);

   return true;
}

void IDEController :: doOpenFile(DialogBase& dialog, IDEModel* model)
{
   NotificationStatus status = {};

   List<path_t, freepath> files(nullptr);
   if (dialog.openFiles(files)) {
      for (auto it = files.start(); !it.eof(); ++it) {
         if(openFile(model, *it, status)) {
            while (model->projectModel.lastOpenFiles.count() >= MAX_RECENT_FILES)
               model->projectModel.lastOpenFiles.cut(
                  model->projectModel.lastOpenFiles.get(model->projectModel.lastOpenFiles.count()));

            model->projectModel.lastOpenFiles.insert((*it).clone());
         }
      }

      _notifier->notify(NOTIFY_IDE_CHANGE, status);
   }
}

bool IDEController :: doSaveFile(DialogBase& dialog, IDEModel* model, bool saveAsMode, bool forcedSave)
{
   auto docView = model->sourceViewModel.DocView();
   if (!docView)
      return false;

   if (docView->isUnnamed() || saveAsMode) {
      PathString path;
      if (!dialog.saveFile(_T("l"), path))
         return false;

      ReferenceName sourceNameStr;
      projectController.defineSourceName(&model->projectModel, *path, sourceNameStr);

      sourceController.renameSource(&model->sourceViewModel, nullptr, *sourceNameStr, *path);

      forcedSave = true;
   }

   if (forcedSave)
      sourceController.saveSource(&model->sourceViewModel, nullptr);

   return true;
}

bool IDEController :: doOpenProject(DialogBase& dialog, IDEModel* model)
{
   NotificationStatus status = NONE_CHANGED;

   PathString path;
   if (dialog.openFile(path)) {
      if (!closeProject(dialog, model, status))
         return false;

      if (openProject(model, *path, status)) {
         model->changeStatus(IDEStatus::Ready);
         status |= IDE_LAYOUT_CHANGED;

         if (status != NONE_CHANGED)
            _notifier->notify(NOTIFY_IDE_CHANGE, status);

         return true;
      }
   }

   return false;
}


bool IDEController :: doSaveProject(DialogBase& dialog, IDEModel* model, bool forcedMode)
{
   //// !! temporal
   //if (!doSaveFile(dialog, model, false, forcedMode))
   //   return false;

   return true;
}

bool IDEController :: closeProject(DialogBase& dialog, IDEModel* model, NotificationStatus& status)
{
   if (closeAll(dialog, model, status)) {
      status |= projectController.closeProject(model->projectModel);

      return true;
   }
   else return false;
}

bool IDEController :: doCloseProject(DialogBase& dialog, IDEModel* model)
{
   NotificationStatus status = NONE_CHANGED;

   if (closeAll(dialog, model, status)) {
      status |= projectController.closeProject(model->projectModel);
      model->changeStatus(IDEStatus::Empty);
      status |= IDE_LAYOUT_CHANGED;

      if (status != NONE_CHANGED)
         _notifier->notify(NOTIFY_IDE_CHANGE, status);

      return true;
   }
   else return false;
}

bool IDEController :: closeFile(DialogBase& dialog, IDEModel* model, int index, NotificationStatus& status)
{
   auto docView = model->sourceViewModel.getDocument(index);
   if (docView->isUnnamed()) {
      if (!doSaveFile(dialog, model, false, true))
         return false;
   }
   else if (docView->isModified()) {
      path_t path = model->sourceViewModel.getDocumentPath(index);

      auto result = dialog.question(
         QUESTION_SAVE_FILECHANGES, path);

      if (result == DialogBase::Answer::Cancel) {
         return false;
      }
      else if (result == DialogBase::Answer::Yes) {
         if (!doSaveFile(dialog, model, false, true))
            return false;
      }
   }

   sourceController.closeSource(&model->sourceViewModel, index, true, status);

   return true;
}

bool IDEController :: doCloseFile(DialogBase& dialog, IDEModel* model)
{
   auto docView = model->sourceViewModel.DocView();
   if (docView) {
      NotificationStatus status = NONE_CHANGED;

      int index = model->sourceViewModel.getCurrentIndex();
      if (index > 0) {
         bool retVal = closeFile(dialog, model, index, status);

         if (status != NONE_CHANGED)
            _notifier->notify(NOTIFY_IDE_CHANGE, status);
      }
   }
   return false;
}

bool IDEController :: closeAll(DialogBase& dialog, IDEModel* model, NotificationStatus& status)
{
   while (model->sourceViewModel.getDocumentCount() > 0) {
      if (!closeFile(dialog, model, 1, status))
         return false;
   }

   return true;
}

bool IDEController :: doCloseAll(DialogBase& dialog, IDEModel* model)
{
   NotificationStatus status = NONE_CHANGED;
   if (closeAll(dialog, model, status)) {
      if (status != NONE_CHANGED)
         _notifier->notify(NOTIFY_IDE_CHANGE, status);

      return true;
   }

   return false;
}

bool IDEController :: doExit(DialogBase& dialog, IDEModel* model)
{
   return doCloseAll(dialog, model);
}

void IDEController :: doSelectNextWindow(IDEModel* model)
{
   sourceController.selectNextDocument(&model->sourceViewModel);
}

void IDEController :: doSelectPrevWindow(IDEModel* model)
{
   sourceController.selectPreviousDocument(&model->sourceViewModel);
}

path_t IDEController :: retrieveSingleProjectFile(IDEModel* model)
{
   if (model->projectModel.sources.count() != 0) {
      return *model->projectModel.sources.start();
   }
   else return nullptr;
}

void IDEController :: doDebugAction(IDEModel* model, DebugAction action)
{
   projectController.doDebugAction(model->projectModel, model->sourceViewModel, action);
}

void IDEController :: doDebugStop(IDEModel* model)
{
   projectController.doDebugStop(model->projectModel);
}

void IDEController :: onCompilationStart(IDEModel* model)
{
   model->status = IDEStatus::Compiling;

   _notifier->notify(NOTIFY_IDE_CHANGE, IDE_STATUS_CHANGED | IDE_COMPILATION_STARTED);
   _notifier->notifySelection(NOTIFY_SHOW_RESULT, model->ideScheme.compilerOutputControl);
}

void IDEController :: onCompilationStop(IDEModel* model)
{
   model->status = IDEStatus::Ready;

   _notifier->notify(NOTIFY_IDE_CHANGE, IDE_STATUS_CHANGED);
}

void IDEController :: onCompilationBreak(IDEModel* model)
{
   model->status = IDEStatus::Broken;

   _notifier->notify(NOTIFY_IDE_CHANGE, IDE_STATUS_CHANGED);
}

void IDEController :: displayErrors(IDEModel* model, text_str output, ErrorLogBase* log)
{
   _notifier->notifySelection(NOTIFY_SHOW_RESULT, model->ideScheme.errorListControl);

   log->clearMessages();

   // parse output for errors
   size_t length = output.length();
   size_t index = 0;

   WideMessage message;
   WideMessage fileStr, rowStr, colStr;
   while (true) {
      bool found = false;
      while (index < length) {
         if (output[index] == ':') {
            if (output.compareSub(_T(": error "), index, 8)) {
               found = true;
               break;
            }
            else if (output.compareSub(_T(": warning "), index, 10)) {
               found = true;
               break;
            }
         }
         index++;
      }
      if (!found)
         break;

      size_t errPos = index;
      size_t rowPos = NOTFOUND_POS;
      size_t colPos = NOTFOUND_POS;
      size_t bolPos = 0;

      index--;
      while (index >= 0) {
         if (output[index] == '(') {
            rowPos = index + 1;
         }
         else if (output[index] == ':' && colPos == NOTFOUND_POS) {
            colPos = index + 1;
         }
         else if (output[index] == '\n') {
            bolPos = index;
            break;
         }

         index--;
      }
      index = output.findSub(errPos, '\n');
      message.copy(output.str() + errPos + 2, index - errPos - 3);
      if (rowPos != NOTFOUND_POS) {
         fileStr.copy(output.str() + bolPos + 1, rowPos - bolPos - 2);
         if (colPos != NOTFOUND_POS) {
            rowStr.copy(output.str() + rowPos, colPos - rowPos - 1);
            colStr.copy(output.str() + colPos, errPos - colPos - 1);
         }
      }
      else {
         fileStr.clear();
         colStr.clear();
         rowStr.clear();
      }
      
      log->addMessage(*message, *fileStr, *rowStr, *colStr);
   }
}

void IDEController :: highlightError(IDEModel* model, int row, int column, path_t path)
{
   PathString fullPath(*model->projectModel.projectPath, path);

   NotificationStatus status = NONE_CHANGED;
   openFile(model, *fullPath, status);

   model->viewModel()->setErrorLine(row, column, true);

   _notifier->notify(NOTIFY_IDE_CHANGE, status | FRAME_ACTIVATE);
}

void IDEController :: onCompilationCompletion(IDEModel* model, int exitCode, 
   text_str output, ErrorLogBase* log)
{
   if (exitCode == 0) {
      model->status = IDEStatus::CompiledSuccessfully;

      _notifier->notify(NOTIFY_IDE_CHANGE, IDE_STATUS_CHANGED);
   }
   else {
      if (exitCode == -1) {
         model->status = IDEStatus::CompiledWithWarnings;
      }
      else model->status = IDEStatus::CompiledWithErrors;
      _notifier->notify(NOTIFY_IDE_CHANGE, IDE_STATUS_CHANGED);

      displayErrors(model, output, log);
   }
}

bool IDEController :: doCompileProject(DialogBase& dialog, IDEModel* model)
{
   onCompilationStart(model);

   if (!doSaveProject(dialog, model, false)) {
      onCompilationBreak(model);

      return false;
   }

   return projectController.doCompileProject(model->projectModel, DebugAction::None);
}

void IDEController :: refreshDebugContext(ContextBrowserBase* contextBrowser, IDEModel* model)
{
   projectController.refreshDebugContext(contextBrowser);

   _notifier->notifySelection(NOTIFY_REFRESH, model->ideScheme.debugWatch);
}

bool IDEController :: onClose(DialogBase& dialog, IDEModel* model)
{
   return doCloseAll(dialog, model);
}

void IDEController :: onDebuggerStop(IDEModel* model)
{
   model->sourceViewModel.clearTraceLine();

   model->status = IDEStatus::DebuggerStopped;
   _notifier->notify(NOTIFY_IDE_CHANGE, IDE_STATUS_CHANGED | FRAME_CHANGED);
}

void IDEController :: onProgramStop(IDEModel* model)
{
   PathString path(*model->projectModel.paths.configPath);

   saveConfig(model, *path);
}

void IDEController :: onStatusChange(IDEModel* model, IDEStatus newStatus)
{
   model->status = newStatus;
   _notifier->notify(NOTIFY_IDE_CHANGE, IDE_STATUS_CHANGED);
}
