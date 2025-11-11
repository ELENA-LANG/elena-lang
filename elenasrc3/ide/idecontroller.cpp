//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller implementation File
//                                             (C)2005-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifdef _MSC_VER

#include <tchar.h>

#else

#define _T(x) x

#endif

#include "idecontroller.h"
#include "eng/messages.h"

using namespace elena_lang;

constexpr auto MAX_RECENT_FILES = 9;
constexpr auto MAX_RECENT_PROJECTS = 9;

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
      case PlatformType::FreeBSD_x86_64:
         return FREEBSD_X86_64_KEY;
      case PlatformType::Linux_PPC64le:
         return LINUX_PPC64le_KEY;
      case PlatformType::Linux_ARM64:
         return LINUX_ARM64_KEY;
      default:
         return nullptr;
   }
}

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

inline void saveSetting(ConfigFile& config, ustr_t xpath, int value)
{
   String<char, 15> number;
   number.appendInt(value);

   config.appendSetting(xpath, number.str());
}

inline void saveSetting(ConfigFile& config, ustr_t xpath, ustr_t value)
{
   config.appendSetting(xpath, value.str());
}

inline void removeSetting(ConfigFile& config, ustr_t xpath)
{
   config.removeSetting(xpath);
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
      for (auto ref_it = model.referencePaths.start(); !ref_it.eof(); ++ref_it) {
         ustr_t extPackage = ref_it.key();
         if (NamespaceString::isIncluded(extPackage, ns)) {
            fullPath.copy(*model.projectPath);
            PathUtil::combineCanonicalized(fullPath, *ref_it);
            fullPath.combine(path);

            return;
         }
      }

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
         for (auto ref_it = model->referencePaths.start(); !ref_it.eof(); ++ref_it) {
            PathString extPath(model->projectPath);
            PathUtil::combineCanonicalized(extPath, *ref_it);
            size_t extPathLen = extPath.length();
            if (PathUtil::compare(sourcePath, *extPath, extPathLen)) {
               name.copy(ref_it.key());

               subPath.copySubPath(*extPath + projectPathLen, true);

               _debugController.resolveNamespace(name);

               return sourcePath + extPathLen + 1;
            }
         }

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
   path_t arguments = model.getArguments();

   if (!target.empty()) {
      PathString exePath(*model.projectPath, target);
      PathUtil::makeCorrectExePath(exePath);

      // provide the whole command line including the executable path and name
      PathString commandLine(exePath);
      commandLine.append(_T(" "));
      commandLine.append(arguments);

      bool withPersistentConsole = model.withPersistentConsole && (model.singleSourceProject || (*model.profile).endsWith("console"));
      bool includeAppPath2Paths = model.includeAppPath2PathsTemporally;
      bool debugMode = model.getDebugMode();
      if (debugMode) {
         if (!_debugController.start(exePath.str(), commandLine.str(), debugMode, { withPersistentConsole, includeAppPath2Paths })) {
            result.noDebugFile = true;

            return false;
         }
      }
      else {
         if (!_debugController.start(exePath.str(), commandLine.str(), false, { withPersistentConsole, includeAppPath2Paths })) {
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
   for (auto it = projectModel.sources.start(); !it.eof(); ++it) {
      PathString source(*projectModel.projectPath, *it);

      PathString module;
      module.copySubPath(*source, true);

      name.copy(projectModel.getPackage());
      name.pathToName(*module + projectPathLen);          // get a full name

      _debugController.resolveNamespace(name);

      DebugInfoProvider::defineModulePath(*name, module, *projectModel.projectPath, projectModel.getOutputPath(), _T("nl"));

      if (module.length() != 0) {
         if (_compareFileModifiedTime(*source, *module))
            return true;
      }
      else return true;
   }

   return false;
}

bool ProjectController :: onDebugAction(ProjectModel& model, SourceViewModel& sourceModel, DebugAction action,
   DebugActionResult& result, bool withoutPostponeAction)
{
   if (!_debugController.isStarted()) {
      bool toRecompile = model.autoRecompile && !withoutPostponeAction;
      if (isOutaged(model, sourceModel)) {
         if (toRecompile) {
            if (!doCompileProject(model, action))
               return false;
         }
         else result.outaged = true;

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

   path_t outputPath = model.getOutputPath();
   if (outputPath.empty())
      outputPath = *model.projectPath;

   cmdLine.append(model.getPackage());
   cmdLine.append(_T("=\"\""));
   cmdLine.append(outputPath);
   cmdLine.append(_T("\"\""));

   cmdLine.append("]]\"");
   cmdLine.append(" -i");

   return _vmProcess->start(*appPath, *cmdLine, *model.paths.appPath, false, 0);
}

void ProjectController :: stopVMConsole()
{
   if (_vmProcess)
      _vmProcess->stop(0);
}

bool ProjectController :: compileProject(ProjectModel& model, int postponedAction)
{
   PathString appPath(model.paths.appPath);
   appPath.combine(*model.paths.compilerPath);

   PathString cmdLine(*model.paths.compilerPath);   
   cmdLine.append(" -w");
   cmdLine.appendInt(model.warningLevel);
   cmdLine.append(" ");

   if (!model.profile.empty()) {
      cmdLine.append(" -l");
      cmdLine.append(*model.profile);
      cmdLine.append(" ");
   }

   cmdLine.append(*model.projectFile);

   PathString curDir;
   curDir.append(*model.projectPath);

   return _outputProcess->start(*appPath, *cmdLine, *model.projectPath, true, postponedAction);
}

bool ProjectController :: compileSingleFile(ProjectModel& model, int postponedAction)
{
   path_t singleProjectFile = model.sources.get(1);

   PathString appPath(model.paths.appPath);
   appPath.combine(*model.paths.compilerPath);

   PathString cmdLine(*model.paths.compilerPath);
   cmdLine.append(" -w");
   cmdLine.appendInt(model.warningLevel);
   cmdLine.append(" ");
   cmdLine.append(singleProjectFile);

   if (!model.templateName.empty()) {
      cmdLine.append(" -t");
      cmdLine.append(*model.templateName);
   }

   if (model.strictType == -1) {
      cmdLine.append(" -xs");
   }
   else if (model.strictType == -1) {
      cmdLine.append(" -xs-");
   }

   PathString curDir;
   curDir.append(*model.projectPath);

   return _outputProcess->start(*appPath, *cmdLine, *model.projectPath, true, postponedAction);
}

bool ProjectController :: doCompileProject(ProjectModel& model, DebugAction postponedAction)
{
   if (model.singleSourceProject) {
      return compileSingleFile(model, (int)postponedAction);
   }
   else if (!model.name.empty()) {
      return compileProject(model, (int)postponedAction);
   }
   else return false;
}

inline void validateValue(int& value, int minValue, int maxValue)
{
   if (value < minValue) {
      value = minValue;
   }
   else if (value > maxValue)
      value = maxValue;
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

      // by loading options, extract warning info
      size_t wIndex = (*model.options).findStr("-w");
      if (wIndex != NOTFOUND_POS) {
         model.warningLevel = model.options[wIndex + 2] - '0';
         validateValue(model.warningLevel, 0, 3);

         model.options.cut(wIndex, 3);
      }
      else model.warningLevel = 1;
   }

   auto outputOption = config.selectNode(configRoot, OUTPUT_SUB_CATEGORY);
   if (!outputOption.isNotFound()) {
      outputOption.readContent(value);

      model.outputPath.copy(value.str());
   }

   model.strictType = loadSetting(config, STRICT_TYPE_SETTING, 1);

   DynamicString<char> subNs;
   DynamicString<char> path;

   // load references
   ConfigFile::Collection references;
   if (config.select(configRoot, REFERENCE_CATEGORY, references)) {
      for (auto r_it = references.start(); !r_it.eof(); ++r_it) {
         // add source file
         ConfigFile::Node node = *r_it;
         node.readContent(path);

         if (!node.readAttribute("key", subNs)) {
            subNs.clear();
         }

         PathString filePath(path.str());
         model.referencePaths.add(subNs.str(), (*filePath).clone());
      }
   }

   // load source files
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

   IdentifierString options(*model.options);
   if (model.warningLevel != 1) {
      if (options.length() > 0)
         options.append(" ");

      options.append("-w");
      options.appendInt(model.warningLevel);
   }

   auto optionsOption = config.selectNode(platformRoot, OPTIONS_SUB_CATEGORY);
   if (!optionsOption.isNotFound()) {
      optionsOption.saveContent(*options);
   }
   else {
      optionsOption = config.selectNode(root, OPTIONS_SUB_CATEGORY);
      if (!optionsOption.isNotFound()) {
         optionsOption.saveContent(*options);
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

   if (model.strictType == FLAG_UNDEFINED) {
      removeSetting(config, STRICT_TYPE_SETTING);
   }
   else saveSetting(config, STRICT_TYPE_SETTING, model.strictType);

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

   model.sources.clear();
   model.singleSourceProject = false;

   setProjectPath(model, projectFile);

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

      if (model.profileList.count() > 0) {
         // select profile automatically if required
         if (model.profile.empty())
            model.profile.copy(*model.profileList.start());

         ConfigFile::Node profileRoot = projectConfig.selectNode<ustr_t>(root, PROFILE_CATEGORY, *model.profile, [](ustr_t key, ConfigFile::Node& node)
            {
               return node.compareAttribute("key", key);
            });

         loadConfig(model, projectConfig, root);
         if (!profileRoot.isNotFound())
            loadConfig(model, projectConfig, profileRoot);

         if (!platformRoot.isNotFound()) {
            ConfigFile::Node profilePlatformRoot = projectConfig.selectNode<ustr_t>(platformRoot, PROFILE_CATEGORY, *model.profile, [](ustr_t key, ConfigFile::Node& node)
               {
                  return node.compareAttribute("key", key);
               });

            loadConfig(model, projectConfig, platformRoot);
            loadConfig(model, projectConfig, profilePlatformRoot);

         }
      }
      else {
         loadConfig(model, projectConfig, root);
         loadConfig(model, projectConfig, platformRoot);
      }

      status |= STATUS_FRAME_VISIBILITY_CHANGED;
   }

   return status;
}

void ProjectController :: setProjectPath(ProjectModel& model, path_t projectFile)
{
   FileNameString src(projectFile, true);
   FileNameString name(projectFile);

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

   model.profileList.clear();
   model.profile.clear();

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

   model.templateName.clear();

   model.sources.add((*src).clone());

   IdentifierString tmp(*name);
   model.package.copy(*tmp);
   model.target.copy(*tmp);

   //model.profile

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

   // HOTFIX : make sure the file is not included
   if (model.sources.retrieveIndex<path_t>(*relPath, [](path_t arg, path_t current)
      {
         return current.compare(arg);
      }) == -1)
   {
      model.sources.add((*relPath).clone());
      model.addedSources.add((*relPath).clone());

      model.notSaved = true;
   }
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

bool IDEController :: loadConfig(IDEModel* model, path_t path, GUISettinngs& guiSettings)
{
   model->projectModel.paths.configPath.copy(path);

   ConfigFile config;
   if (config.load(path, FileEncoding::UTF8)) {
      model->appMaximized = loadSetting(config, MAXIMIZED_SETTINGS, -1) != 0;
      model->sourceViewModel.schemeIndex = loadSetting(config, SCHEME_SETTINGS, 1);
      model->projectModel.withPersistentConsole = loadSetting(config, PERSISTENT_CONSOLE_SETTINGS, -1) != 0;
#ifdef _MSC_VER
      model->projectModel.includeAppPath2PathsTemporally = loadSetting(config, INCLIDE_PATH2ENV_SETTINGS, 0) != 0;
#endif
      model->rememberLastPath = loadSetting(config, LASTPATH_SETTINGS, -1) != 0;
      model->rememberLastProject = loadSetting(config, LASTPROJECT_SETTINGS, -1) != 0;
      model->sourceViewModel.highlightSyntax = loadSetting(config, HIGHLIGHTSYNTAX_SETTINGS, -1) != 0;
      model->sourceViewModel.highlightBrackets = loadSetting(config, HIGHLIGHTBRACKETS_SETTINGS, -1) != 0;
      model->sourceViewModel.lineNumbersVisible = loadSetting(config, LINENUMBERS_SETTINGS, -1) != 0;
      model->sourceViewModel.scrollOffset = loadSetting(config, VSCROLL_SETTINGS, 1);
      model->sourceViewModel.settings.tabSize = loadSetting(config, TABSIZE_SETTINGS, 3);
      model->projectModel.autoRecompile = loadSetting(config, AUTO_RECOMPILE_SETTING, -1) != 0;
      model->autoSave = loadSetting(config, AUTO_SAVE_SETTING, -1) != 0;

      guiSettings.withLargeToolbar = loadSetting(config, LARGETOOLBAR_SETTINGS, -1) != 0;
      guiSettings.withTabAboverscore = loadSetting(config, TABABOVESCORE_SETTINGS, -1) != 0;

      // load font size
      int fontSize = loadSetting(config, FONTSIZE_SETTINGS, 12);
      IdentifierString fontName;
      loadSetting(config, FONTNAME_SETTINGS, fontName);
      if (fontName.empty()) {
         fontName.copy(DEFAULT_FONTNAME);
      }

      model->sourceViewModel.fontInfo = { *fontName, fontSize };

      loadRecentFiles(config, RECENTFILES_SETTINGS, model->projectModel.lastOpenFiles);
      loadRecentFiles(config, RECENTPROJECTS_SETTINGS, model->projectModel.lastOpenProjects);

      model->sourceViewModel.refreshSettings();

      return true;
   }
   else {
      return false;
   }
}

void IDEController :: saveConfig(IDEModel* model, path_t configPath, GUISettinngs& guiSettings)
{
   ConfigFile config(ROOT_NODE);

   saveSetting(config, MAXIMIZED_SETTINGS, model->appMaximized);
   saveSetting(config, SCHEME_SETTINGS, model->sourceViewModel.schemeIndex);
   saveSetting(config, PERSISTENT_CONSOLE_SETTINGS, model->projectModel.withPersistentConsole);
#ifdef _MSC_VER
   saveSetting(config, INCLIDE_PATH2ENV_SETTINGS, model->projectModel.includeAppPath2PathsTemporally);
#endif
   saveSetting(config, LASTPATH_SETTINGS, model->rememberLastPath);
   saveSetting(config, LASTPROJECT_SETTINGS, model->rememberLastProject);
   saveSetting(config, HIGHLIGHTSYNTAX_SETTINGS, model->sourceViewModel.highlightSyntax);
   saveSetting(config, HIGHLIGHTBRACKETS_SETTINGS, model->sourceViewModel.highlightBrackets);
   saveSetting(config, LINENUMBERS_SETTINGS, model->sourceViewModel.lineNumbersVisible);
   saveSetting(config, VSCROLL_SETTINGS, model->sourceViewModel.scrollOffset);
   saveSetting(config, TABSIZE_SETTINGS, model->sourceViewModel.settings.tabSize);

   saveSetting(config, AUTO_RECOMPILE_SETTING, model->projectModel.autoRecompile);
   saveSetting(config, AUTO_SAVE_SETTING, model->autoSave);

   saveSetting(config, FONTSIZE_SETTINGS, model->sourceViewModel.fontInfo.size);
   IdentifierString fontName(model->sourceViewModel.fontInfo.name.str());
   saveSetting(config, FONTNAME_SETTINGS, *fontName);

   saveSetting(config, LARGETOOLBAR_SETTINGS, guiSettings.withLargeToolbar);
   saveSetting(config, TABABOVESCORE_SETTINGS, guiSettings.withTabAboverscore);

   saveRecentFiles(config, RECENTFILE_SETTINGS, model->projectModel.lastOpenFiles);
   saveRecentFiles(config, RECENTPROJECTS_SETTINGS, model->projectModel.lastOpenProjects);

   config.save(*model->projectModel.paths.configPath, FileEncoding::UTF8);
}

void IDEController :: init(IDEModel* model, int& status)
{
   status |= STATUS_STATUS_CHANGED | STATUS_FRAME_VISIBILITY_CHANGED | STATUS_LAYOUT_CHANGED;

   if (model->rememberLastProject && model->projectModel.lastOpenProjects.count() > 0) {
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

void IDEController :: addToRecentProjects(IDEModel* model, path_t path)
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
   int retVal = projectController.openProject(model->projectModel, projectFile);
   if (retVal)
      addToRecentProjects(model, projectFile);

   return retVal;
}

bool IDEController :: doOpenFile(IDEModel* model, PathList& files)
{
   int projectStatus = STATUS_NONE;

   for (auto it = files.start(); !it.eof(); ++it) {
      if(openFile(model, *it, projectStatus)) {
         removeDuplicate(model->projectModel.lastOpenFiles, *it);

         while (model->projectModel.lastOpenFiles.count() >= MAX_RECENT_FILES)
            model->projectModel.lastOpenFiles.cut(model->projectModel.lastOpenFiles.end());

         model->projectModel.lastOpenFiles.insert((*it).clone());
      }
   }

   notifyOnModelChange(projectStatus);

   return projectStatus != 0;
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

bool IDEController :: ifFileUnnamed(IDEModel* model, int index)
{
   auto docView = index == -1 ? model->sourceViewModel.DocView() : model->sourceViewModel.getDocument(index);

   return docView != nullptr && docView->isUnnamed();
}

bool IDEController :: ifFileNotSaved(IDEModel* model, int index)
{
   auto docView = index == -1 ? model->sourceViewModel.DocView() : model->sourceViewModel.getDocument(index);

   return docView != nullptr && (docView->isModified());
}

bool IDEController::ifProjectNotSaved(IDEModel* model)
{
   return model->projectModel.notSaved;
}

bool IDEController :: ifProjectUnnamed(IDEModel* model)
{
   return model->projectModel.projectPath.empty();
}

bool IDEController :: doSaveFile(IDEModel* model, int index, bool forcedSave, path_t filePath)
{
   auto docView = index == -1 ? model->sourceViewModel.DocView() : model->sourceViewModel.getDocument(index);
   if (!docView || docView->isReadOnly())
      return false;

   if (!filePath.empty()) {
      NamespaceString sourceNameStr;
      projectController.defineSourceName(&model->projectModel, filePath, sourceNameStr);

      sourceController.renameSource(&model->sourceViewModel, nullptr, *sourceNameStr, filePath);

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

void IDEController :: doNewProject(IDEModel* model)
{
   int projectStatus = STATUS_NONE;
   projectStatus |= projectController.newProject(model->projectModel);

   notifyOnModelChange(projectStatus);
}

void IDEController :: doOpenProject(IDEModel* model, path_t path)
{
   int projectStatus = STATUS_NONE;

   if (PathUtil::checkExtension(path, "l")) {
      openFile(model, path, projectStatus);
   }
   else projectStatus |= openProject(model, path);

   notifyOnModelChange(projectStatus);
}

bool IDEController :: doSaveProject(IDEModel* model, path_t newPath)
{
   int projectStatus = STATUS_NONE;

   if (!newPath.empty()) {
      projectController.setProjectPath(model->projectModel, newPath);
   }
   
   projectStatus |= projectController.saveProject(model->projectModel);

   notifyOnModelChange(projectStatus);

   return true;
}

bool IDEController :: closeProject(IDEModel* model, int& status)
{
   if (closeAll(model, status)) {
      status |= projectController.closeProject(model->projectModel);

      return true;
   }
   else return false;
}

//bool IDEController :: doCloseProject(FileDialogBase& dialog, FileDialogBase& projectDialog, MessageDialogBase& mssgDialog, IDEModel* model)
//{
//   int projectStatus = STATUS_NONE;
//
//   if(!closeProject(dialog, projectDialog, mssgDialog, model, projectStatus))
//      return false;
//
//   notifyOnModelChange(projectStatus);
//
//   return true;
//}
//
//bool IDEController :: saveFile(FileDialogBase& dialog, IDEModel* model, int index, bool forcedMode)
//{
//   auto docView = model->sourceViewModel.getDocument(index);
//
//   return doSaveFile(dialog, model, false, forcedMode);
//}

bool IDEController :: closeFile(IDEModel* model, int index, int& status)
{
   auto docView = index == -1 ? model->sourceViewModel.DocView() : model->sourceViewModel.getDocument(index);

   sourceController.closeSource(&model->sourceViewModel, index, true, status);

   return true;
}

bool IDEController :: doCloseFile(IDEModel* model, int index)
{
   int projectStatus = STATUS_NONE;

   if (index == -1)
      index = model->sourceViewModel.getCurrentIndex();

   if (index > 0) {
      bool retVal = closeFile(model, index, projectStatus);

      notifyOnModelChange(projectStatus);

      return retVal;
   }

   return false;
}

bool IDEController :: closeAll(IDEModel* model, int& status)
{
   while (model->sourceViewModel.getDocumentCount() > 0) {
      if (!closeFile(model, 1, status))
         return false;
   }

   return true;
}

bool IDEController :: closeAllButActive(IDEModel* model, int& status)
{
   int index = model->sourceViewModel.getCurrentIndex();
   for (int i = 1; i < index; i++) {
      if (!closeFile(model, 1, status))
         return false;
   }

   int count = model->sourceViewModel.getDocumentCount();
   for (int i = 2; i <= count; i++) {
      if (!closeFile(model, 2, status))
         return false;
   }

   return true;
}

//bool IDEController :: saveAll(FileDialogBase& dialog, IDEModel* model, bool forcedMode)
//{
//   for (pos_t i = 0; i < model->sourceViewModel.getDocumentCount(); i++) {
//      if (!saveFile(dialog, model, i + 1, forcedMode))
//         return false;
//   }
//
//   return true;
//}

bool IDEController :: doCloseAll(IDEModel* model, bool closeProjectMode)
{
   int projectStatus = STATUS_NONE;
   
   if (closeProjectMode) {
      if (!closeProject(model, projectStatus))
         return false;
   }
   else if (!closeAll(model, projectStatus))
      return false;

   notifyOnModelChange(projectStatus);
   return true;
}

bool IDEController :: doCloseAllButActive(IDEModel* model)
{
   int projectStatus = STATUS_NONE;
   if (closeAllButActive(model, projectStatus)) {
      notifyOnModelChange(projectStatus);

      return true;
   }

   return false;
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

void IDEController :: doDebugAction(IDEModel* model, DebugAction action,
   MessageDialogBase& mssgDialog, bool withoutPostponeAction)
{
   if (model->running)
      return;

   DebugActionResult result = {};
   if (projectController.onDebugAction(model->projectModel, model->sourceViewModel,
      action, result, withoutPostponeAction))
   {
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

   TextString message;
   TextString fileStr, rowStr, colStr;
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
   if (openFile(model, *fullPath, projectStatus)) {
      DocumentChangeStatus docStatus = { test(projectStatus, STATUS_FRAME_CHANGED) };
      model->viewModel()->setErrorLine(row, column, true, docStatus);

      TextViewModelEvent event = { projectStatus, docStatus };
      _notifier->notify(&event);
   }
}

void IDEController :: onCompilationCompletion(IDEModel* model, int exitCode,
   text_str output, ErrorLogBase* log)
{
   model->running = false;

   if (exitCode == EXIT_SUCCESS) {
      model->status = IDEStatus::CompiledSuccessfully;

      notifyOnModelChange(STATUS_STATUS_CHANGED | STATUS_PROJECT_REFRESH);
   }
   else {
      if (exitCode != EXIT_FAILURE) {
         model->status = IDEStatus::CompiledWithWarnings;
      }
      else model->status = IDEStatus::CompiledWithErrors;

      displayErrors(model, output, log);

      notifyOnModelChange(STATUS_STATUS_CHANGED | STATUS_PROJECT_REFRESH | STATUS_WITHERRORS);
   }
}

bool IDEController :: doCompileProject(IDEModel* model)
{
   onCompilationStart(model);

   return projectController.doCompileProject(model->projectModel, DebugAction::None);
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

bool IDEController :: onClose(IDEModel* model)
{
   projectController.stopVMConsole();

   return true;
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

void IDEController :: onIDEStop(IDEModel* model, GUISettinngs& guiSettings)
{
   PathString path(*model->projectModel.paths.configPath);

   saveConfig(model, *path, guiSettings);
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
   bool prevHighlightSyntax = model->viewModel()->highlightSyntax;
   bool prevHighlightBrackets = model->viewModel()->highlightBrackets;

   if(editorDialog.showModal()) {
      if (prevSchemeIndex != model->viewModel()->schemeIndex || prevHighlightSyntax != model->viewModel()->highlightSyntax
         || prevHighlightBrackets != model->viewModel()->highlightBrackets)
      {
         notifyOnModelChange(STATUS_FRAME_CHANGED | STATUS_COLORSCHEME_CHANGED);
      }
   }
}

void IDEController :: doConfigureFontSettings(FontDialogBase& editorDialog, IDEModel* model)
{
   if (editorDialog.selectFont(model->viewModel()->fontInfo)) {
      notifyOnModelChange(STATUS_FRAME_CHANGED | STATUS_COLORSCHEME_CHANGED);
   }
}

void IDEController :: doConfigureIDESettings(IDESettingsBase& ideDialog, IDEModel* model)
{
   if (ideDialog.showModal()) {
   }
}

void IDEController :: doConfigureDebuggerSettings(DebuggerSettingsBase& ideDialog, IDEModel* model)
{
   if (ideDialog.showModal()) {
   }
}

void IDEController :: onDebuggerNoSource(MessageDialogBase& mssgDialog, IDEModel* model)
{
   model->running = false;

   auto result = mssgDialog.question(QUESTION_NOSOURCE_CONTINUE);

   if (result == MessageDialogBase::Answer::Yes)
      doDebugAction(model, DebugAction::StepInto, mssgDialog, false);
}

void IDEController :: onDocSelection(IDEModel* model, int index)
{
   notifyOnModelChange(STATUS_FRAME_CHANGED);
}
