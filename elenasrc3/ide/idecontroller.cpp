//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller implementation File
//                                             (C)2005-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <tchar.h>

#include "idecontroller.h"
#include "eng/messages.h"

using namespace elena_lang;

constexpr auto MAX_RECENT_FILES = 10;
constexpr auto MAX_RECENT_PROJECTS = 10;

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

void SourceViewController :: newSource(TextViewModelBase* model, ustr_t caption, bool included, bool autoSelect, int& status)
{
   IdentifierString tabName("unnamed");

   bool empty = model->empty;

   newDocument(model, caption, included);

   if (empty)
      status |= STATUS_FRAME_VISIBILITY_CHANGED;

   if (autoSelect) {
      int index = model->getDocumentIndex(caption);

      if (selectDocument(model, index)) {
         status |= STATUS_FRAME_CHANGED;
      }
   }
}

bool SourceViewController :: openSource(TextViewModelBase* model, ustr_t caption, path_t sourcePath, 
   FileEncoding encoding, bool included, bool autoSelect, int& status)
{
   bool empty = model->empty;

   if (openDocument(model, caption, sourcePath, encoding, included)) {
      if (empty)
         status |= STATUS_FRAME_VISIBILITY_CHANGED;

      if (autoSelect) {
         int index = model->getDocumentIndex(caption);

         if (selectDocument(model, index))
            status |= STATUS_FRAME_CHANGED;
      }

      return true;
   }
   return false;
}

void SourceViewController :: renameSource(TextViewModelBase* model, ustr_t oldName, ustr_t newName, path_t newSourcePath)
{
   model->renameDocumentView(oldName, newName, newSourcePath);
}

void SourceViewController :: closeSource(TextViewModelBase* model, int index, bool autoSelect, int& status)
{
   if (index > 0) {
      int count = model->getDocumentCount();

      closeDocument(model, index, status);

      if (autoSelect && !model->empty) {
         bool changed = false;
         if (index == count) {
            changed = selectDocument(model, count - 1);
         }
         else changed = selectDocument(model, index);

         if (changed)
            status |= STATUS_FRAME_CHANGED;
      }

      if (model->empty)
         status |= STATUS_FRAME_VISIBILITY_CHANGED;
   }
}

void SourceViewController :: saveSource(TextViewModelBase* model, ustr_t name)
{
   int index = model->getDocumentIndex(name);

   auto docInfo = model->getDocument(index);
   path_t path = model->getDocumentPath(index);

   if (docInfo) {
      docInfo->save(path);
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

path_t ProjectController :: retrieveSourceName(ProjectModel* model, path_t sourcePath, NamespaceString& name, PathString& subPath)
{
   size_t projectPathLen = model->projectPath.length();

   PathString path;
   path.copySubPath(sourcePath, true);

   if (!model->projectPath.empty() && (*path).compareSub(*model->projectPath, 0, projectPathLen)) {
      name.copy(model->getPackage());
      if (path.length() > projectPathLen) {
         name.pathToName(*path + projectPathLen);
         subPath.copySubPath(*path + projectPathLen, true);

         _debugController.resolveNamespace(name);
      }
      return sourcePath + projectPathLen;
   }
   else {
      path_t rootPath = *model->paths.librarySourceRoot;
      size_t rootPathLen = rootPath.length();

      if (!rootPath.empty() && PathUtil::compare(sourcePath, rootPath, rootPathLen)) {
         name.pathToName(sourcePath + rootPathLen);
         subPath.copySubPath(*path + rootPathLen, true);

         _debugController.resolveNamespace(name);

         size_t rootNsPos = (*name).find('\'', name.length());
         subPath.cut(0, rootNsPos + 1);

         return sourcePath + rootPathLen + rootNsPos + 1;
      }
      else {
         FileNameString fileName(sourcePath);
         IdentifierString tmp(*fileName);

         name.copy(*tmp);
      }
   }

   return sourcePath;
}

void ProjectController :: defineSourceName(ProjectModel* model, path_t path, NamespaceString& retVal)
{
   if (path.empty()) {
      retVal.copy("undefined");
   }
   else {
      PathString subPath;
      retrieveSourceName(model, path, retVal, subPath);
      retVal.append(':');

      if (subPath.length() > 0) {
         IdentifierString subStr(*subPath);
         retVal.append(*subStr);
      }

      FileNameString fileName(path, true);
      IdentifierString tmp(*fileName);
      retVal.append(*tmp);
   }
}

bool ProjectController :: startDebugger(ProjectModel& model, DebugActionResult& result)
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
            result.noDebugFile = true;            

            return false;
         }

      }
      else {
         if (!_debugController.start(exePath.str(), commandLine.str(), false/*, _breakpoints */)) {
            //notifyCompletion(NOTIFY_DEBUGGER_RESULT, ERROR_RUN_NEED_RECOMPILE);

            return false;
         }
      }

      return true;
   }
   else {
      result.targetMissing = true;      

      return false;
   }
}

bool ProjectController :: isOutaged(ProjectModel& projectModel, SourceViewModel& sourceModel)
{
   if (sourceModel.isAnyDocumentModified()) {
      return false;
   }

   size_t projectPathLen = projectModel.projectPath.length();

   NamespaceString name;
   PathString rootPath(*projectModel.projectPath, projectModel.getOutputPath());
   for (auto it = projectModel.sources.start(); !it.eof(); ++it) {
      PathString source(*rootPath, *it);

      PathString module;
      module.copySubPath(*source, true);

      name.copy(projectModel.getPackage());
      name.pathToName(*module + projectPathLen);          // get a full name

      _debugController.resolveNamespace(name);

      ReferenceName::nameToPath(module, *name);
      module.append(_T(".nl"));

      if (name.length() != 0) {
         if (_compareFileModifiedTime(*source, *module))
            return false;
      }
      else return false;
   }

   return true;
}

bool ProjectController :: onDebugAction(ProjectModel& model, SourceViewModel& sourceModel, DebugAction action, DebugActionResult& result)
{
   if (!_debugController.isStarted()) {
      bool toRecompile = model.autoRecompile;
      if (!isOutaged(model, sourceModel)) {
         result.outaged = true;

         if (toRecompile) {
            if (!doCompileProject(model, action))
               return false;
         }
         return false;
      }
      if (!startDebugger(model, result))
         return false;
   }
   return true;
}

void ProjectController :: doDebugAction(ProjectModel& model, SourceViewModel& sourceModel , DebugAction action)
{
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

void ProjectController :: doDebugStop(ProjectModel& model)
{
   _debugController.stop();
}

void ProjectController :: runToCursor(ProjectModel& model, SourceViewModel& sourceModel)
{
   auto currentDoc = sourceModel.DocView();
   if (currentDoc != nullptr) {
      int index = sourceModel.getCurrentIndex();
      ustr_t currentSource = sourceModel.getDocumentName(index);
      path_t currentPath = sourceModel.getDocumentPath(index);

      NamespaceString ns;
      PathString subPath;
      currentPath = retrieveSourceName(&model, currentPath, ns, subPath);

      FileNameString properName(currentPath, true);
      subPath.combine(*properName);
      
      IdentifierString pathStr(*subPath);
      _debugController.runToCursor(*ns, *pathStr, currentDoc->getCaret().y + 1);
   }
}

bool ProjectController :: startVMConsole(ProjectModel& model)
{
   PathString appPath(model.paths.appPath);
   appPath.combine(*model.paths.vmTerminalPath);

   PathString cmdLine(*model.paths.vmTerminalPath);
   cmdLine.append(" \"[[ #use ");

   cmdLine.append(model.getPackage());
   cmdLine.append(_T("=\"\""));
   cmdLine.append(*model.projectPath);
   cmdLine.append(_T("\""));

   cmdLine.append("]]\""); 
   cmdLine.append(" -i");

   return _vmProcess->start(*appPath, *cmdLine, *model.paths.appPath, false);
}

void ProjectController :: stopVMConsole()
{
   _vmProcess->stop(0);
}

bool ProjectController :: compileProject(ProjectModel& model)
{
   PathString appPath(model.paths.appPath);
   appPath.combine(*model.paths.compilerPath);

   PathString cmdLine(*model.paths.compilerPath);
   cmdLine.append(" -w3 "); // !! temporal

   if (!model.profile.empty()) {
      cmdLine.append(" -l");
      cmdLine.append(*model.profile);
      cmdLine.append(" ");
   }

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
   cmdLine.append(" -w3 "); // !! temporal
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

   auto templateOption = config.selectNode(configRoot, TEMPLATE_SUB_CATEGORY);
   if (!templateOption.isNotFound()) {
      templateOption.readContent(value);

      model.templateName.copy(value.str());
   }

   auto nsOption = config.selectNode(configRoot, NAMESPACE_SUB_CATEGORY);
   if (!nsOption.isNotFound()) {
      nsOption.readContent(value);

      model.package.copy(value.str());
   }

   auto optionsOption = config.selectNode(configRoot, OPTIONS_SUB_CATEGORY);
   if (!optionsOption.isNotFound()) {
      optionsOption.readContent(value);

      model.options.copy(value.str());
   }

   auto outputOption = config.selectNode(configRoot, OUTPUT_SUB_CATEGORY);
   if (!outputOption.isNotFound()) {
      outputOption.readContent(value);

      model.outputPath.copy(value.str());
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

inline void removeSource(ConfigFile& config, ConfigFile::Node root, ustr_t current)
{
   DynamicString<char> path;

   ConfigFile::Collection modules;
   if (config.select(root, MODULE_CATEGORY, modules)) {
      for (auto m_it = modules.start(); !m_it.eof(); ++m_it) {
         ConfigFile::Node moduleNode = *m_it;

         ConfigFile::Collection files;
         if (config.select(moduleNode, "*", files)) {
            for (auto it = files.start(); !it.eof(); ++it) {
               // add source file
               ConfigFile::Node node = *it;
               node.readContent(path);

               if (current.compare(path.str())) {
                  node.remove();

                  break;
               }
            }
         }
      }
   }
}

void ProjectController :: saveConfig(ProjectModel& model, ConfigFile& config, ConfigFile::Node root, ConfigFile::Node platformRoot)
{
   auto templateOption = config.selectNode(platformRoot, TEMPLATE_SUB_CATEGORY);
   if (!templateOption.isNotFound()) {
      templateOption.saveContent(*model.templateName);
   }
   else {
      templateOption = config.selectNode(root, TEMPLATE_SUB_CATEGORY);
      if (!templateOption.isNotFound()) {
         templateOption.saveContent(*model.templateName);
      }
      else config.appendSetting(TEMPLATE_CATEGORY, *model.templateName);
   }

   auto nsOption = config.selectNode(platformRoot, NAMESPACE_SUB_CATEGORY);
   if (!nsOption.isNotFound()) {
      templateOption.saveContent(*model.package);
   }
   else {
      nsOption = config.selectNode(root, NAMESPACE_SUB_CATEGORY);
      if (!nsOption.isNotFound()) {
         nsOption.saveContent(*model.package);
      }
      else config.appendSetting(NAMESPACE_CATEGORY, *model.package);
   }

   auto optionsOption = config.selectNode(platformRoot, OPTIONS_SUB_CATEGORY);
   if (!optionsOption.isNotFound()) {
      optionsOption.saveContent(*model.options);
   }
   else {
      optionsOption = config.selectNode(root, OPTIONS_SUB_CATEGORY);
      if (!optionsOption.isNotFound()) {
         optionsOption.saveContent(*model.options);
      }
      else config.appendSetting(OPTIONS_CATEGORY, *model.options);
   }

   auto targetOption = config.selectNode(platformRoot, TARGET_SUB_CATEGORY);
   if (!targetOption.isNotFound()) {
      targetOption.saveContent(*model.target);
   }
   else {
      targetOption = config.selectNode(root, TARGET_SUB_CATEGORY);
      if (!targetOption.isNotFound()) {
         targetOption.saveContent(*model.target);
      }
      else config.appendSetting(TARGET_CATEGORY, *model.target);
   }

   // remove source files
   for (auto it = model.removeSources.start(); !it.eof(); ++it) {
      IdentifierString pathStr(*it);

      removeSource(config, root, *pathStr);
   }
   model.removeSources.clear();

   // adding source files
   for (auto it = model.addedSources.start(); !it.eof(); ++it) {
      IdentifierString pathStr(*it);

      config.appendSetting(FILE_CATEGORY, *pathStr);
   }

   model.addedSources.clear();
}

int ProjectController :: newProject(ProjectModel& model)
{
   model.sources.clear();

   model.empty = false;
   model.name.copy("unnamed");
   model.package.copy("unnamed");
   model.notSaved = true;

   return STATUS_PROJECT_CHANGED;
}

void ProjectController :: loadProfileList(ProjectModel& model, ConfigFile& config, ConfigFile::Node root)
{
   ConfigFile::Collection profiles;
   if (config.select(root, PROFILE_CATEGORY, profiles)) {
      for (auto it = profiles.start(); !it.eof(); ++it) {
         ConfigFile::Node profileNode = *it;

         DynamicString<char> key;
         profileNode.readAttribute("key", key);

         if (model.profileList.retrieveIndex<ustr_t>(key.str(), [](ustr_t arg, ustr_t current)
            {
               return current.compare(arg);
            }) == -1) 
         {
            model.profileList.add(ustr_t(key.str()).clone());
         }
      }
   }
}

int ProjectController :: openProject(ProjectModel& model, path_t projectFile)
{
   int status = STATUS_PROJECT_CHANGED;

   ustr_t key = getPlatformName(_platform);

   setProjectPath(model, projectFile);

   model.singleSourceProject = false;

   ConfigFile projectConfig;
   if (projectConfig.load(projectFile, FileEncoding::UTF8)) {
      DynamicString<char> value;

      ConfigFile::Node root = projectConfig.selectRootNode();

      ConfigFile::Node nsNode = projectConfig.selectNode(NAMESPACE_CATEGORY);
      if (!nsNode.isNotFound()) {
         nsNode.readContent(value);
         model.package.copy(value.str());
      }

      // select platform configuration
      ConfigFile::Node platformRoot = projectConfig.selectNode<ustr_t>(PLATFORM_CATEGORY, key, [](ustr_t key, ConfigFile::Node& node)
         {
            return node.compareAttribute("key", key);
         });

      loadProfileList(model, projectConfig, root);
      loadProfileList(model, projectConfig, platformRoot);

      loadConfig(model, projectConfig, root);
      loadConfig(model, projectConfig, platformRoot);

      status |= STATUS_FRAME_VISIBILITY_CHANGED;
   }

   return status;
}

void ProjectController :: setProjectPath(ProjectModel& model, path_t projectFile)
{
   FileNameString src(projectFile, true);
   FileNameString name(projectFile);

   model.sources.clear();

   model.empty = false;
   model.name.copy(*name);
   model.projectFile.copy(*src);
   model.projectPath.copySubPath(projectFile, true);
}

int ProjectController :: saveProject(ProjectModel& model)
{
   ustr_t key = getPlatformName(_platform);

   PathString path(*model.projectPath, *model.projectFile);
   ConfigFile projectConfig;

   bool existing = projectConfig.load(*path, FileEncoding::UTF8);
   if (!existing)
      projectConfig.create();

   ConfigFile::Node root = projectConfig.selectRootNode();
   // select platform configuration
   ConfigFile::Node platformRoot = projectConfig.selectNode<ustr_t>(PLATFORM_CATEGORY, key, [](ustr_t key, ConfigFile::Node& node)
      {
         return node.compareAttribute("key", key);
      });

   saveConfig(model, projectConfig, root, platformRoot);

   projectConfig.save(*path, FileEncoding::UTF8);

   model.notSaved = false;

   return STATUS_PROJECT_CHANGED;
}

int ProjectController :: closeProject(ProjectModel& model)
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

   return STATUS_PROJECT_CHANGED;
}

int ProjectController :: openSingleFileProject(ProjectModel& model, path_t singleProjectFile)
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

   return STATUS_PROJECT_CHANGED;
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

void ProjectController :: refreshDebugContext(ContextBrowserBase* contextBrowser, size_t param, addr_t address)
{
   _debugController.readContext(contextBrowser, (void*)param, address, 4);
}

bool ProjectController :: toggleBreakpoint(ProjectModel& model, SourceViewModel& sourceModel, int row, DocumentChangeStatus& status)
{
   auto currentDoc = sourceModel.DocView();
   if (currentDoc != nullptr) {
      if (row == -1)
         row = currentDoc->getCaret().y + 1;

      int index = sourceModel.getCurrentIndex();
      ustr_t currentSource = sourceModel.getDocumentName(index);
      path_t currentPath = sourceModel.getDocumentPath(index);

      NamespaceString ns;
      PathString subPath;
      currentPath = retrieveSourceName(&model, currentPath, ns, subPath);

      bool addMode = true;
      IdentifierString pathStr(currentSource + currentSource.find(':') + 1);
      for(auto it = model.breakpoints.start(); !it.eof(); ++it) {
         auto bm = *it;
         if (bm->row == row && bm->module.compare(*ns) && bm->source.compare(*pathStr)) {
            if (_debugController.isStarted())
               _debugController.toggleBreakpoint(bm, false);

            model.breakpoints.cut(bm);
            addMode = false;
            break;
         }
      }
      
      if (addMode) {
         Breakpoint* bm = new Breakpoint(row, *pathStr, *ns);

         model.breakpoints.add(bm);

         if (_debugController.isStarted())
            _debugController.toggleBreakpoint(bm, true);

         currentDoc->addMarker(row, STYLE_BREAKPOINT, false, true, status);
      }
      else {
         currentDoc->removeMarker(row, STYLE_BREAKPOINT, status);
      }

      return true;
   }

   return false;
}

void ProjectController :: loadBreakpoints(ProjectModel& model)
{
   for (auto it = model.breakpoints.start(); !it.eof(); ++it) {
      _debugController.toggleBreakpoint(*it, true);
   }
}

bool ProjectController :: isFileIncluded(ProjectModel& model, path_t filePath)
{
   if (model.singleSourceProject)
      return false;

   PathString relPath(filePath);
   _pathHelper->makePathRelative(relPath, *model.projectPath);

   for (auto it = model.sources.start(); !it.eof(); ++it) {
      PathString source(*it);

      if ((*relPath).compare(*source)) {
         return true;
      }
   }

   return false;
}

void ProjectController :: includeFile(ProjectModel& model, path_t filePath)
{
   PathString relPath(filePath);
   _pathHelper->makePathRelative(relPath, *model.projectPath);

   model.sources.add((*relPath).clone());
   model.addedSources.add((*relPath).clone());

   model.notSaved = true;
}

void ProjectController :: excludeFile(ProjectModel& model, path_t filePath)
{
   PathString relPath(filePath);
   _pathHelper->makePathRelative(relPath, *model.projectPath);

   model.sources.cut(*relPath);
   model.removeSources.add((*relPath).clone());

   model.notSaved = true;
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

inline void loadSetting(ConfigFile& config, ustr_t xpath, IdentifierString& retVal)
{
   // read target type; merge it with platform if required
   ConfigFile::Node targetType = config.selectNode(xpath);
   if (!targetType.isNotFound()) {
      DynamicString<char> key;
      targetType.readContent(key);

      retVal.copy(key.str());
   }
   else retVal.clear();
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

inline void loadCollectionKey(ConfigFile& config, ConfigFile::Node& rootNode, ustr_t xpath, StringList& keyList)
{
   DynamicString<char> key;

   ConfigFile::Collection list;
   if (config.select(rootNode, xpath, list)) {
      for (auto m_it = list.start(); !m_it.eof(); ++m_it) {
         ConfigFile::Node keyNode = *m_it;

         keyNode.readAttribute("key", key);

         keyList.add(ustr_t(key.str()).clone());
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

void IDEController :: loadSystemConfig(IDEModel* model, path_t path, ustr_t typeXPath, ustr_t platformXPath)
{
   ConfigFile config;
   if (config.load(path, FileEncoding::UTF8)) {
      ConfigFile::Node platformRoot = config.selectNode<ustr_t>(PLATFORM_CATEGORY, platformXPath, [](ustr_t key, ConfigFile::Node& node)
         {
            return node.compareAttribute("key", key);
         });

      loadCollectionKey(config, platformRoot, typeXPath, model->projectModel.projectTypeList);
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
      loadRecentFiles(config, RECENTPROJECTS_SETTINGS, model->projectModel.lastOpenProjects);

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
   saveRecentFiles(config, RECENTPROJECTS_SETTINGS, model->projectModel.lastOpenProjects);

   config.save(*model->projectModel.paths.configPath, FileEncoding::UTF8);
}

void IDEController :: init(IDEModel* model, int& status)
{
   status |= STATUS_STATUS_CHANGED | STATUS_FRAME_VISIBILITY_CHANGED | STATUS_LAYOUT_CHANGED;

   if (model->projectModel.lastOpenProjects.count() > 0) {
      PathString path(model->projectModel.lastOpenProjects.get(1));

      if (PathUtil::checkExtension(*path, "l")) {
         if (openFile(model, *path, status)) {
            model->changeStatus(IDEStatus::Ready);
         }
      }
      else {
         int retVal = openProject(model, *path);
         if (retVal) {
            status |= (retVal | STATUS_DOC_READY);
         }
      }
   }
   else status |= STATUS_PROJECT_CHANGED;
}

void IDEController :: traceStep(SourceViewModel* sourceModel, bool found, int row)
{
   int projectStatus = STATUS_DEBUGGER_STOPPED;
   DocumentChangeStatus docStatus = {};

   sourceModel->clearTraceLine(docStatus);

   if (found) {
      sourceModel->setTraceLine(row, true, docStatus);

      projectStatus |= STATUS_DEBUGGER_STEP;
   }
   else projectStatus |= STATUS_DEBUGGER_NOSOURCE;

   TextViewModelEvent event = { projectStatus, docStatus };
   _notifier->notify(&event);
}

void IDEController :: traceStart(ProjectModel* model)
{
   onDebuggerHook(model);
}

void IDEController :: traceFinish(SourceViewModel* sourceModel)
{
   notifyOnModelChange(STATUS_DEBUGGER_FINISHED);
}

bool IDEController :: selectSource(ProjectModel* model, SourceViewModel* sourceModel,
   ustr_t ns, path_t sourcePath)
{
   PathString fullPath;
   projectController.defineFullPath(*model, ns, sourcePath, fullPath);

   int projectStatus = STATUS_NONE;
   if (openFile(sourceModel, model, *fullPath, projectStatus)) {
      notifyOnModelChange(projectStatus);

      return true;
   }
   return false;
}

void IDEController :: doNewFile(IDEModel* model)
{
   int projectStatus = STATUS_NONE;
   NamespaceString sourceNameStr;
   projectController.defineSourceName(&model->projectModel, nullptr, sourceNameStr);

   sourceController.newSource(&model->sourceViewModel, *sourceNameStr, false, true, projectStatus);

   notifyOnModelChange(projectStatus);
}

inline void removeDuplicate(ProjectPaths& lastOpenFiles, path_t value)
{
   for (auto it = lastOpenFiles.start(); !it.eof(); ++it) {
      if (value.compare(*it)) {
         lastOpenFiles.cut(it);

         return;
      }
   }
}

inline void addToRecentProjects(IDEModel* model, path_t path)
{
   removeDuplicate(model->projectModel.lastOpenProjects, path);

   while (model->projectModel.lastOpenProjects.count() >= MAX_RECENT_PROJECTS)
      model->projectModel.lastOpenProjects.cut(model->projectModel.lastOpenProjects.end());

   model->projectModel.lastOpenProjects.insert(path.clone());
}

bool IDEController :: openFile(IDEModel* model, path_t sourceFile, int& status)
{
   if (model->projectModel.name.empty()) {
      status |= projectController.openSingleFileProject(model->projectModel, sourceFile);

      addToRecentProjects(model, sourceFile);
   }

   return openFile(&model->sourceViewModel, &model->projectModel, sourceFile, status);
}

bool IDEController :: openFile(SourceViewModel* model, ProjectModel* projectModel, path_t sourceFile, int& status)
{
   ustr_t sourceName = model->getDocumentNameByPath(sourceFile);
   int index = sourceName.empty() ? 0 : model->getDocumentIndex(sourceName);

   if (index > 0) {
      if (sourceController.selectDocument(model, index)) {
         status |= STATUS_FRAME_CHANGED;

         return true;
      }

      return false;
   }
   else {
      NamespaceString sourceNameStr;
      projectController.defineSourceName(projectModel, sourceFile, sourceNameStr);

      sourceName = *sourceNameStr;
   }

   bool retVal = sourceController.openSource(model, sourceName, sourceFile,
      defaultEncoding, projectController.isFileIncluded(*projectModel, sourceFile), true, status);

   return retVal;
}

void IDEController :: notifyOnModelChange(int projectStatus)
{
   DocumentChangeStatus docStatus = { test(projectStatus, STATUS_FRAME_CHANGED) };

   TextViewModelEvent event = { projectStatus, docStatus };
   _notifier->notify(&event);
}

bool IDEController :: doOpenProjectSourceByIndex(IDEModel* model, int index)
{
   int projectStatus = STATUS_NONE;

   path_t sourcePath = projectController.getSourceByIndex(model->projectModel, index);
   if (!sourcePath.empty()) {
      PathString fullPath(*model->projectModel.projectPath, sourcePath);

      if(openFile(model, *fullPath, projectStatus)) {         
         notifyOnModelChange(projectStatus);

         return true;
      }
   }
   return false;
}

int IDEController :: openProject(IDEModel* model, path_t projectFile)
{
   return projectController.openProject(model->projectModel, projectFile);
}

void IDEController :: doOpenFile(FileDialogBase& dialog, IDEModel* model)
{
   int projectStatus = STATUS_NONE;

   List<path_t, freepath> files(nullptr);
   if (dialog.openFiles(files)) {
      for (auto it = files.start(); !it.eof(); ++it) {
         if(openFile(model, *it, projectStatus)) {
            removeDuplicate(model->projectModel.lastOpenFiles, *it);

            while (model->projectModel.lastOpenFiles.count() >= MAX_RECENT_FILES)
               model->projectModel.lastOpenFiles.cut(model->projectModel.lastOpenFiles.end());

            model->projectModel.lastOpenFiles.insert((*it).clone());
         }
      }

      notifyOnModelChange(projectStatus);
   }
}

void IDEController :: doOpenFile(IDEModel* model, path_t path)
{
   int projectStatus = STATUS_NONE;

   if (openFile(model, path, projectStatus)) {
      removeDuplicate(model->projectModel.lastOpenFiles, path);

      while (model->projectModel.lastOpenFiles.count() >= MAX_RECENT_FILES)
         model->projectModel.lastOpenFiles.cut(model->projectModel.lastOpenFiles.end());

      model->projectModel.lastOpenFiles.insert(path.clone());
   }

   notifyOnModelChange(projectStatus);
}

bool IDEController :: doSaveFile(FileDialogBase& dialog, IDEModel* model, bool saveAsMode, bool forcedSave)
{
   auto docView = model->sourceViewModel.DocView();
   if (!docView || docView->isReadOnly())
      return false;

   if (docView->isUnnamed() || saveAsMode) {
      PathString path;
      if (!dialog.saveFile(_T("l"), path))
         return false;

      NamespaceString sourceNameStr;
      projectController.defineSourceName(&model->projectModel, *path, sourceNameStr);

      sourceController.renameSource(&model->sourceViewModel, nullptr, *sourceNameStr, *path);

      forcedSave = true;
   }

   if (forcedSave) {
      sourceController.saveSource(&model->sourceViewModel, nullptr);

      DocumentChangeStatus docStatus = { };
      docView->refresh(docStatus);

      TextViewModelEvent event = { STATUS_NONE, docStatus };
      _notifier->notify(&event);
   }
      
   return true;
}

bool IDEController :: doSaveAll(FileDialogBase& dialog, FileDialogBase& projectDialog, IDEModel* model)
{
   if (!saveAll(dialog, model, true)) {
      return true;
   }

   if (model->projectModel.notSaved) {
      return doSaveProject(projectDialog, model, false);
   }

   return false;
}

void IDEController :: doNewProject(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog,
   ProjectSettingsBase& prjSettingDialog, IDEModel* model)
{
   int projectStatus = STATUS_NONE;
   if (!closeProject(dialog, projectDialog, mssgDialog, model, projectStatus))
      return;

   projectStatus |= projectController.newProject(model->projectModel);

   if (prjSettingDialog.showModal()) {
      notifyOnModelChange(projectStatus);
   }
}

bool IDEController :: doOpenProject(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model)
{
   int projectStatus = STATUS_NONE;

   PathString path;
   if (projectDialog.openFile(path)) {
      if (!closeProject(dialog, projectDialog, mssgDialog, model, projectStatus))
         return false;

      int retVal = openProject(model, *path);

      if (retVal) {
         projectStatus |= retVal;

         addToRecentProjects(model, *path);

         projectStatus |= STATUS_DOC_READY;

         notifyOnModelChange(projectStatus);

         return true;
      }
   }

   return false;
}

void IDEController :: doOpenProject(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model, path_t path)
{
   int projectStatus = STATUS_NONE;

   if (!closeProject(dialog, projectDialog, mssgDialog, model, projectStatus))
      return;

   if (PathUtil::checkExtension(path, "l")) {
      openFile(model, path, projectStatus);
   }
   else projectStatus |= openProject(model, path);

   notifyOnModelChange(projectStatus);
}

bool IDEController :: saveProject(FileDialogBase& projectDialog, IDEModel* model, bool saveAsMode, int& status)
{
   if (saveAsMode || model->projectModel.notSaved) {
      if (model->projectModel.projectPath.empty()) {
         PathString path;
         if (!projectDialog.saveFile(_T("prj"), path))
            return false;

         projectController.setProjectPath(model->projectModel, *path);
      }

      status |= projectController.saveProject(model->projectModel);
   }

   return true;
}

bool IDEController :: doSaveProject(FileDialogBase& projectDialog, IDEModel* model, bool saveAsMode)
{
   int projectStatus = STATUS_NONE;
   if (!saveProject(projectDialog, model, saveAsMode, projectStatus))
      return false;

   notifyOnModelChange(projectStatus);

   return true;
}

bool IDEController :: closeProject(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog,
   IDEModel* model, int& status)
{
   if (model->projectModel.notSaved) {
      auto result = mssgDialog.question(QUESTION_SAVEPROJECT_CHANGES);
      if (result == MessageDialogBase::Answer::Cancel) {
         return false;
      }
      else if (result == MessageDialogBase::Answer::Yes) {
         if (!saveProject(projectDialog, model, false, status))
            return false;
      }
   }

   if (closeAll(dialog, mssgDialog, model, status)) {
      status |= projectController.closeProject(model->projectModel);

      return true;
   }
   else return false;
}

bool IDEController :: doCloseProject(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model)
{
   int projectStatus = STATUS_NONE;

   if(!closeProject(dialog, projectDialog, mssgDialog, model, projectStatus))
      return false;

   notifyOnModelChange(projectStatus);

   return true;
}

bool IDEController :: saveFile(FileDialogBase& dialog, IDEModel* model, int index, bool forcedMode)
{
   auto docView = model->sourceViewModel.getDocument(index);

   return doSaveFile(dialog, model, false, forcedMode);
}

bool IDEController :: closeFile(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, 
   int index, int& status)
{
   auto docView = model->sourceViewModel.getDocument(index);
   if (docView->isUnnamed()) {
      if (!doSaveFile(dialog, model, false, true)) {
         auto result = mssgDialog.question(QUESTION_CLOSE_UNSAVED);

         if (result != MessageDialogBase::Answer::Yes)
            return false;
      }
   }
   else if (docView->isModified()) {
      path_t path = model->sourceViewModel.getDocumentPath(index);

      auto result = mssgDialog.question(
         QUESTION_SAVE_FILECHANGES, path);

      if (result == MessageDialogBase::Answer::Cancel) {
         return false;
      }
      else if (result == MessageDialogBase::Answer::Yes) {
         if (!doSaveFile(dialog, model, false, true))
            return false;
      }
   }

   sourceController.closeSource(&model->sourceViewModel, index, true, status);

   return true;
}

bool IDEController :: doCloseFile(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, int index)
{
   int projectStatus = STATUS_NONE;

   if (index > 0) {
      bool retVal = closeFile(dialog, mssgDialog, model, index, projectStatus);

      notifyOnModelChange(projectStatus);

      return retVal;
   }

   return false;
}

bool IDEController :: doCloseFile(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model)
{
   auto docView = model->sourceViewModel.DocView();
   if (docView) {
      return doCloseFile(dialog, mssgDialog, model, model->sourceViewModel.getCurrentIndex());
   }
   return false;
}

bool IDEController :: closeAll(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, 
   int& status)
{
   while (model->sourceViewModel.getDocumentCount() > 0) {
      if (!closeFile(dialog, mssgDialog, model, 1, status))
         return false;
   }

   return true;
}

bool IDEController :: closeAllButActive(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model, 
   int& status)
{
   int index = model->sourceViewModel.getCurrentIndex();
   for (int i = 1; i < index; i++) {
      if (!closeFile(dialog, mssgDialog, model, 1, status))
         return false;
   }

   int count = model->sourceViewModel.getDocumentCount();
   for (int i = 2; i <= count; i++) {
      if (!closeFile(dialog, mssgDialog, model, 2, status))
         return false;
   }

   return true;
}

bool IDEController :: saveAll(FileDialogBase& dialog, IDEModel* model, bool forcedMode)
{
   for (pos_t i = 0; i < model->sourceViewModel.getDocumentCount(); i++) {
      if (!saveFile(dialog, model, i + 1, forcedMode))
         return false;
   }

   return true;
}

bool IDEController :: doCloseAll(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model, bool closeProjectMode)
{
   int projectStatus = STATUS_NONE;
   if (closeProjectMode) {
      if (!closeProject(dialog, projectDialog, mssgDialog, model, projectStatus))
         return false;
   }
   else if (!closeAll(dialog, mssgDialog, model, projectStatus))
      return false;

   notifyOnModelChange(projectStatus);
   return true;
}

bool IDEController :: doCloseAllButActive(FileDialogBase& dialog, MessageDialogBase& mssgDialog, IDEModel* model)
{
   int projectStatus = STATUS_NONE;
   if (closeAllButActive(dialog, mssgDialog, model, projectStatus)) {
      notifyOnModelChange(projectStatus);

      return true;
   }

   return false;
}

bool IDEController :: doExit(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model)
{
   projectController.stopVMConsole();

   return doCloseAll(dialog, projectDialog, mssgDialog, model, true);
}

void IDEController :: doSelectNextWindow(IDEModel* model)
{
   sourceController.selectNextDocument(&model->sourceViewModel);
}

void IDEController :: doSelectPrevWindow(IDEModel* model)
{
   sourceController.selectPreviousDocument(&model->sourceViewModel);
}

void IDEController :: doSelectWindow(TextViewModelBase* viewModel, path_t path)
{
   ustr_t sourceName = viewModel->getDocumentNameByPath(path);
   int index = sourceName.empty() ? 0 : viewModel->getDocumentIndex(sourceName);

   if (index > 0) {
      int projectStatus = STATUS_NONE;

      if(sourceController.selectDocument(viewModel, index)) {
         projectStatus |= STATUS_FRAME_CHANGED;

         notifyOnModelChange(projectStatus);
      }
   }
}

path_t IDEController :: retrieveSingleProjectFile(IDEModel* model)
{
   if (model->projectModel.sources.count() != 0) {
      return *model->projectModel.sources.start();
   }
   else return nullptr;
}

void IDEController :: doDebugAction(IDEModel* model, DebugAction action, MessageDialogBase& mssgDialog)
{
   if (model->running)
      return;

   DebugActionResult result = {};
   if (projectController.onDebugAction(model->projectModel, model->sourceViewModel, action, result)) {
      model->running = true;

      model->sourceViewModel.setReadOnlyMode(true);

      DocumentChangeStatus docStatus = {};
      model->sourceViewModel.clearTraceLine(docStatus);
      docStatus.readOnlyChanged = true;

      TextViewModelEvent event = { STATUS_DEBUGGER_RUNNING, docStatus };
      _notifier->notify(&event);

      projectController.doDebugAction(model->projectModel, model->sourceViewModel, action);
   }
   else if (model->sourceViewModel.isAnyDocumentModified())
      mssgDialog.info(INFO_RUN_UNSAVED_PROJECT);
   else if (result.outaged) {
      mssgDialog.info(INFO_RUN_OUT_OF_DATE);
   }
   else if (result.targetMissing) {
      mssgDialog.info(INFO_NEED_TARGET);
   }
   else if (result.noDebugFile) {
      mssgDialog.info(INFO_RUN_NEED_RECOMPILE);
   }
}

void IDEController :: doDebugStop(IDEModel* model)
{
   if (projectController.isStarted())
      projectController.doDebugStop(model->projectModel);
}

void IDEController :: onCompilationStart(IDEModel* model)
{
   model->running = true;
   model->status = IDEStatus::Compiling;

   notifyOnModelChange(STATUS_STATUS_CHANGED | STATUS_COMPILING | STATUS_LAYOUT_CHANGED);
}

void IDEController :: displayErrors(IDEModel* model, text_str output, ErrorLogBase* log)
{
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

   int projectStatus = STATUS_FRAME_ACTIVATE;
   openFile(model, *fullPath, projectStatus);

   DocumentChangeStatus docStatus = { test(projectStatus, STATUS_FRAME_CHANGED) };
   model->viewModel()->setErrorLine(row, column, true, docStatus);

   TextViewModelEvent event = { projectStatus, docStatus };
   _notifier->notify(&event);
}

void IDEController :: onCompilationCompletion(IDEModel* model, int exitCode, 
   text_str output, ErrorLogBase* log)
{
   model->running = false;

   if (exitCode == 0) {
      model->status = IDEStatus::CompiledSuccessfully;

      notifyOnModelChange(STATUS_STATUS_CHANGED | STATUS_PROJECT_REFRESH);
   }
   else {
      if (exitCode == -1) {
         model->status = IDEStatus::CompiledWithWarnings;
      }
      else model->status = IDEStatus::CompiledWithErrors;

      displayErrors(model, output, log);

      notifyOnModelChange(STATUS_STATUS_CHANGED | STATUS_PROJECT_REFRESH | STATUS_WITHERRORS);
   }
}

bool IDEController :: doCompileProject(FileDialogBase& dialog, FileDialogBase& projectDialog, IDEModel* model)
{
   if (doSaveProject(projectDialog, model, false)) {
      onCompilationStart(model);

      return projectController.doCompileProject(model->projectModel, DebugAction::None);
   }
   else return false;
}

void IDEController :: doStartVMConsole(IDEModel* model)
{
   projectController.startVMConsole(model->projectModel);
}

void IDEController :: doStopVMConsole()
{
   projectController.stopVMConsole();
}

void IDEController :: doChangeProject(ProjectSettingsBase& prjDialog, IDEModel* model)
{
   if (prjDialog.showModal()) {
      
   }
}

void IDEController :: refreshDebugContext(ContextBrowserBase* contextBrowser, IDEModel* model)
{
   projectController.refreshDebugContext(contextBrowser);
}

void IDEController :: refreshDebugContext(ContextBrowserBase* contextBrowser, IDEModel* model, size_t item, size_t param)
{
   projectController.refreshDebugContext(contextBrowser, item, param);
}

bool IDEController :: onClose(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model)
{
   projectController.stopVMConsole();

   return doCloseAll(dialog, projectDialog, mssgDialog, model, true);
}

void IDEController :: onDebuggerHook(ProjectModel* model)
{
   projectController.loadBreakpoints(*model);
}

void IDEController :: onDebuggerStop(IDEModel* model)
{
   DocumentChangeStatus status = {};

   model->sourceViewModel.clearTraceLine(status);
   model->status = IDEStatus::DebuggerStopped;    

   model->running = false;
   model->sourceViewModel.setReadOnlyMode(false);

   status.readOnlyChanged = true;

   TextViewModelEvent event = { STATUS_STATUS_CHANGED, status };
   _notifier->notify(&event);
}

void IDEController :: onDebuggerStep(IDEModel* model)
{
   model->running = false;
}

void IDEController :: onIDEStop(IDEModel* model)
{
   PathString path(*model->projectModel.paths.configPath);

   saveConfig(model, *path);
}

void IDEController :: toggleBreakpoint(IDEModel* model, int row)
{
   DocumentChangeStatus status = {};

   if (projectController.toggleBreakpoint(model->projectModel, model->sourceViewModel, row, status)) {
      TextViewModelEvent event = { 0, status };
      _notifier->notify(&event);
   }
}

void IDEController :: doInclude(IDEModel* model)
{
   path_t path = model->sourceViewModel.getDocumentPath(model->sourceViewModel.getCurrentIndex());

   projectController.includeFile(model->projectModel, path);

   model->sourceViewModel.DocView()->markAsInclued();

   notifyOnModelChange(STATUS_FRAME_CHANGED | STATUS_PROJECT_CHANGED);
}

void IDEController :: doExclude(IDEModel* model)
{
   path_t path = model->sourceViewModel.getDocumentPath(model->sourceViewModel.getCurrentIndex());

   projectController.excludeFile(model->projectModel, path);

   model->sourceViewModel.DocView()->markAsExcluded();

   notifyOnModelChange(STATUS_FRAME_CHANGED | STATUS_PROJECT_CHANGED);
}

bool IDEController :: doSearch(FindDialogBase& dialog, IDEModel* model)
{
   if (dialog.showModal()) {
      if(!sourceController.findText(model->viewModel(), &model->findModel)) {
         return false;
      }
   }

   return true;
}

bool IDEController :: doReplace(FindDialogBase& dialog, MessageDialogBase& qusetionDialog, IDEModel* model)
{
   if (dialog.showModal()) {
      while (sourceController.findText(model->viewModel(), &model->findModel)) {
         auto result = qusetionDialog.question(REPLACE_TEXT);
         if (result == MessageDialogBase::Cancel)
            return true;
         if (result == MessageDialogBase::Yes) {
            if(!sourceController.replaceText(model->viewModel(), &model->findModel))
               break;
         }
      }
   }

   return false;
}

bool IDEController :: doSearchNext(IDEModel* model)
{
   if (sourceController.findText(model->viewModel(), &model->findModel)) {
      return true;
   }
   return false;
}

void IDEController :: doGoToLine(GotoDialogBase& dialog, IDEModel* model)
{
   auto docView = model->viewModel()->DocView();
   if (docView != nullptr) {
      int row = docView->getCaret().y;
      if (dialog.showModal(row)) {
         sourceController.goToLine(model->viewModel(), row);
      }
   }
}

void IDEController :: doSelectWindow(FileDialogBase& fileDialog, MessageDialogBase& mssgDialog, WindowListDialogBase& dialog, 
   IDEModel* model)
{
   auto retVal = dialog.selectWindow();
   switch (retVal.value2) {
      case WindowListDialogBase::Mode::Activate:
      {
         path_t path = model->sourceViewModel.getDocumentPath(retVal.value1);
         doSelectWindow(model->viewModel(), path);
         break;
      }
      case WindowListDialogBase::Mode::Close:
         doCloseFile(fileDialog, mssgDialog, model, retVal.value1);
         break;
      default:
         break;
   }
}

void IDEController :: doIndent(IDEModel* model)
{
   sourceController.indent(&model->sourceViewModel);
}

void IDEController :: doOutdent(IDEModel* model)
{
   sourceController.outdent(&model->sourceViewModel);
}

void IDEController :: doConfigureEditorSettings(EditorSettingsBase& editorDialog, IDEModel* model)
{
   int prevSchemeIndex = model->viewModel()->schemeIndex;

   if(editorDialog.showModal()) {
      if (prevSchemeIndex != model->viewModel()->schemeIndex) {
         notifyOnModelChange(STATUS_FRAME_CHANGED | STATUS_COLORSCHEME_CHANGED);
      }         
   }
}

void IDEController :: onDebuggerNoSource(MessageDialogBase& mssgDialog, IDEModel* model)
{
   model->running = false;

   auto result = mssgDialog.question(QUESTION_NOSOURCE_CONTINUE);

   if (result == MessageDialogBase::Answer::Yes)
      doDebugAction(model, DebugAction::StepInto, mssgDialog);
}

void IDEController :: onDocSelection(IDEModel* model, int index)
{
   notifyOnModelChange(STATUS_FRAME_CHANGED);
}
