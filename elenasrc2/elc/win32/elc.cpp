//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the win32 command-line compiler
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#define __MSVCRT_VERSION__ 0x0800

#include "elena.h"
// --------------------------------------------------------------------------
#include "elc.h"
#include "constants.h"
#include "errors.h"
#include "compilerlogic.h"
#include "linker.h"
#include "image.h"
#include "x86jitcompiler.h"
#include "amd64jitcompiler.h"
#include "derivation.h"

#include <stdarg.h>
#include <windows.h>

// --- getAppPath ---

void getAppPath(_ELENA_::Path& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(NULL, path, MAX_PATH);

   appPath.copySubPath(path);
   appPath.lower();
}

// --- Print ---

void print(const wchar_t* wstr, ...)
{
   va_list argptr;
   va_start(argptr, wstr);

   vwprintf(wstr, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

void print(const char* str, ...)
{
   va_list argptr;
   va_start(argptr, str);

   vprintf(str, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

// --- ImageHelper ---

class ImageHelper : public _ELENA_::ExecutableImage::_Helper
{
   _ELENA_::Linker* _linker;

public:
   _ELENA_::ref_t tls_directory;

   virtual void beforeLoad(_ELENA_::_JITCompiler* compiler, _ELENA_::ExecutableImage& image)
   {
      _ELENA_::Project* project = image.getProject();
      _ELENA_::_JITLoader* loader = dynamic_cast<_ELENA_::_JITLoader*>(&image);

      // compile TLS section if it is a multi-threading app
      if (project->IntSetting(_ELENA_::opThreadMax) > 1) {
         _linker->prepareTLS(image, compiler->allocateTLSVariable(loader), tls_directory);

         // load GC thread table, should be allocated before static roots
         // thread table contains TLS reference
         compiler->allocateThreadTable(loader, project->IntSetting(_ELENA_::opThreadMax));
      }
      else compiler->allocateTLSVariable(loader);
   }

   virtual void afterLoad(_ELENA_::ExecutableImage& image)
   {
      _ELENA_::Project* project = image.getProject();

      _ELENA_::Section* debug = image.getDebugSection();

      // fix up debug section if required
      if (debug->Length() > 8) {
         debug->writeDWord(0, debug->Length());
         debug->addReference(image.getDebugEntryPoint(), 4);

         // save subject info if enabled
         _ELENA_::MemoryWriter debugWriter(debug);
         if (project->BoolSetting(_ELENA_::opDebugSubjectInfo)) {
            image.saveActionNames(&debugWriter);
         }
         else debugWriter.writeDWord(0);
      }
      else debug->clear();
   }

   ImageHelper(_ELENA_::Linker* linker)
   {
      this->_linker = linker;
      this->tls_directory = 0;
   }
};

// --- Project ---

_ELC_::Project :: Project()
{
   getAppPath(appPath);
   _settings.add(_ELENA_::opAppPath, _ELENA_::IdentifierString::clonePath(appPath.c_str()));
   _settings.add(_ELENA_::opNamespace, ((_ELENA_::ident_t)"unnamed").clone());

   _tabSize = 4;
   _encoding = _ELENA_::feUTF8;

   // !! temporally
   _settings.add(_ELENA_::opDebugSubjectInfo, -1);
}

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal)
{
   if (_ELENA_::emptystr(path))
      // !! HOTFIX
      path = "autogeneated";

   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wPath(path);
   _ELENA_::WideString wTerminal(terminal);

   print(wMsg, (const wchar_t*)wPath, row, column, (const wchar_t*)wTerminal);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t identifier)
{
   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wParam(identifier);

   print(wMsg, (const wchar_t*)wParam);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg)
{
   _ELENA_::WideString wMsg(msg);

   print(wMsg);

   throw _ELENA_::_Exception();
}

void _ELC_::Project::printInfo(const char* msg, _ELENA_::ReferenceInfo referenceInfo)
{
   if (referenceInfo.isRelative()) {
      _ELENA_::IdentifierString fullName(referenceInfo.module->Name(), referenceInfo.referenceName);

      printInfo(msg, fullName.c_str());
   }
   else printInfo(msg, referenceInfo.referenceName);
}

void _ELC_::Project :: printInfo(const char* msg, _ELENA_::ident_t value)
{
   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wParam(value);

   print(wMsg, (const wchar_t*)wParam);
}

void _ELC_::Project::raiseErrorIf(bool throwExecption, _ELENA_::ident_t msg, _ELENA_::ident_t identifier)
{
   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wParam(identifier);

   print(wMsg, (const wchar_t*)wParam);

   if (throwExecption)
      throw _ELENA_::_Exception();
}

void _ELC_::Project::raiseWarning(int level, _ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal)
{
   if (!_ELENA_::test(_warningMasks, level))
      return;

   if (!indicateWarning())
      return;

   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wPath(path);
   _ELENA_::WideString wTerminal(terminal);

   print(wMsg, (const wchar_t*)wPath, row, column, (const wchar_t*)wTerminal);
}

void _ELC_::Project::raiseWarning(int level, _ELENA_::ident_t msg, _ELENA_::ident_t path)
{
   if (!_ELENA_::test(_warningMasks, level))
      return;

   if (!indicateWarning())
      return;

   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wPath(path);

   print(wMsg, (const wchar_t*)wPath);
}

bool _ELC_::Project :: readCategory(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting, _ELENA_::_ConfigFile::Nodes& list)
{
   switch (setting) {
      case _ELENA_::opTemplates:
         return config.select(TEMPLATE_CATEGORY, list);
      case _ELENA_::opPrimitives:
         return config.select(PRIMITIVE_CATEGORY, list);
      case _ELENA_::opSources:
         return config.select(SOURCE_CATEGORY, list);
      case _ELENA_::opForwards:
         return config.select(FORWARD_CATEGORY, list);
      case _ELENA_::opExternals:
         return config.select(EXTERNALS_CATEGORY, list);
      case _ELENA_::opReferences:
         return config.select(REFERENCE_CATEGORY, list);
      case _ELENA_::opWinAPI:
         return config.select(WINAPI_CATEGORY, list);
      case _ELENA_::opTargets:
         return config.select(TARGET_CATEGORY, list);
      default:
         return false;
   }
}

_ELENA_::ident_t _ELC_::Project::getOption(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting)
{
   switch (setting) {
      case _ELENA_::opNamespace:
         return config.getSetting(ELC_NAMESPACE);
      case _ELENA_::opGCMGSize:
         return config.getSetting(ELC_MG_SIZE);
      case _ELENA_::opGCYGSize:
         return config.getSetting(ELC_YG_SIZE);
      case _ELENA_::opSizeOfStackReserv:
         return config.getSetting(ELC_STACK_RESERV);
      case _ELENA_::opSizeOfStackCommit:
         return config.getSetting(ELC_STACK_COMMIT);
      case _ELENA_::opSizeOfHeapReserv:
         return config.getSetting(ELC_HEAP_RESERV);
      case _ELENA_::opSizeOfHeapCommit:
         return config.getSetting(ELC_HEAP_COMMIT);
      case _ELENA_::opImageBase:
         return config.getSetting(ELC_YG_IMAGEBASE);
      case _ELENA_::opPlatform:
         return config.getSetting(ELC_PLATFORMTYPE);
      case _ELENA_::opTarget:
         return config.getSetting(ELC_TARGET);
      case _ELENA_::opLibPath:
         return config.getSetting(ELC_LIB_PATH);
      case _ELENA_::opOutputPath:
         return config.getSetting(ELC_OUTPUT_PATH);
      case _ELENA_::opWarnOnWeakUnresolved:
         return config.getSetting(ELC_WARNON_WEAKUNRESOLVED);
//         //   case _ELENA_::opWarnOnSignature:
//   //      return config.getSetting(PROJECT_CATEGORY, ELC_WARNON_SIGNATURE);
      case _ELENA_::opDebugMode:
         return config.getSetting(ELC_DEBUGINFO);
      case _ELENA_::opDebugSubjectInfo:
         return config.getSetting(ELC_SUBJECTINFO);
      case _ELENA_::opClassSymbolAutoLoad:
         return config.getSetting(ELC_CLASSSYMBOLLOAD);
      case _ELENA_::opThreadMax:
         return config.getSetting(ELC_SYSTEM_THREADMAX);
      case _ELENA_::opL0:
         return config.getSetting(ELC_L0);
//      case _ELENA_::opL1:
//         return config.getSetting(ELC_L1);
//   //   case _ELENA_::opL2:
//   //      return config.getSetting(COMPILER_CATEGORY, ELC_L2);
      case _ELENA_::opTemplate:
         return config.getSetting(ELC_PROJECT_TEMPLATE);
      case _ELENA_::opManifestName:
         return config.getSetting(ELC_MANIFEST_NAME);
      case _ELENA_::opManifestVersion:
         return config.getSetting(ELC_MANIFEST_VERSION);
      case _ELENA_::opManifestAuthor:
         return config.getSetting(ELC_MANIFEST_AUTHOR);
      default:
         return NULL;
   }
}

void _ELC_::Project :: addTarget(_ELENA_::_ConfigFile::Node moduleNode)
{
   _ELENA_::ident_t name = moduleNode.Attribute(ELC_NAME_KEY);

   _ELENA_::ident_t typeAttr = moduleNode.Attribute(ELC_TYPE_NAME);
   int type = typeAttr.toInt();
   if (type != 0) {
      _targets.add(name, ELC_TYPE_NAME, type);

      // copy options
      _ELENA_::_ConfigFile::Nodes list;
      moduleNode.select(ELC_OPTION, list);

      for (_ELENA_::_ConfigFile::Nodes::Iterator it = list.start(); !it.Eof(); it++) {
         _ELENA_::ident_t val = (*it).Content();

         _targets.add(name, ELC_OPTION, val.clone());
      }

   }
   // !! raise a warning if type is invalid
}

void _ELC_::Project :: addModule(_ELENA_::_ConfigFile::Node moduleNode)
{
   _ELENA_::ReferenceNs name(Namespace());

   name.combine(moduleNode.Attribute(ELC_NAME_KEY));

   int key = _sources.Count() + 1;
   _sources.add(key, ELC_NAMESPACE_KEY, name.ident().clone());

   _ELENA_::ident_t targetAttr = moduleNode.Attribute(ELC_TARGET_NAME);
   if (!_ELENA_::emptystr(targetAttr)) {
      _sources.add(key, ELC_TARGET_NAME, targetAttr.clone());
   }

   _ELENA_::_ConfigFile::Nodes list;
   moduleNode.select(ELC_INCLUDE, list);

   for (_ELENA_::_ConfigFile::Nodes::Iterator it = list.start(); !it.Eof(); it++) {
      _ELENA_::ident_t file = (*it).Content();

      _sources.add(key, ELC_INCLUDE, file.clone());
   }
}

void _ELC_::Project :: addSource(_ELENA_::path_t path)
{
   _ELENA_::Path modulePath;
   _ELENA_::ReferenceNs name(Namespace());

   // build module namespace
   modulePath.copySubPath(path);
   name.pathToName(modulePath.c_str());

   int key = 0;
   for (_ELENA_::SourceIterator it = _sources.start(); !it.Eof(); it++) {
      _ELENA_::ident_t currentName = _sources.get(it.key(), ELC_NAMESPACE_KEY, DEFAULT_STR);
      if (currentName.compare(name)) {
         key = it.key();
         break;
      }
         
   }
   if (key == 0) {
      key = _sources.Count() + 1;

      _sources.add(key, ELC_NAMESPACE_KEY, name.ident().clone());
   }

   _sources.add(key, ELC_INCLUDE, _ELENA_::IdentifierString::clonePath(path));
}

void _ELC_::Project :: cleanUp()
{
   _ELENA_::Path rootPath(StrSetting(_ELENA_::opProjectPath), StrSetting(_ELENA_::opOutputPath));

   for (_ELENA_::SourceIterator it = _sources.start(); !it.Eof(); it++) {
      _ELENA_::ident_t ns = _sources.get(it.key(), ELC_NAMESPACE_KEY, DEFAULT_STR);

      _ELENA_::Path path;

      // remove module
      path.copy(rootPath.c_str());
      _loader.nameToPath(ns, path, "nl");
      _wremove(path);

      // remove debug module
      path.copy(rootPath.c_str());
      _loader.nameToPath(ns, path, "dnl");
      _wremove(path);
   }
}

void _ELC_::Project :: loadConfig(_ELENA_::path_t path, bool root, bool requiered)
{
   ElcXmlConfigFile config;
   _ELENA_::Path configPath;
   
   configPath.copySubPath(path);
   
   if (!config.load(path, _encoding)) {
      raiseErrorIf(requiered, ELC_ERR_INVALID_PATH, _ELENA_::IdentifierString(path));
      return;
   }

   try
   {
      loadGenericConfig(config, configPath.c_str(), root, requiered);
   }
   catch (_ELENA_::XMLException&)
   {
      raiseErrorIf(requiered, ELC_ERR_INVALID_PATH, _ELENA_::IdentifierString(path));
      return;
   }
   
}

void _ELC_::Project :: loadGenericConfig(_ELENA_::_ConfigFile& config, _ELENA_::path_t configPath, bool root, bool requiered)
{
   // load template list
   if (root)
      loadCategory(config, _ELENA_::opTemplates, configPath);

   // load template
   _ELENA_::ident_t projectTemplate = config.getSetting(ELC_PROJECT_TEMPLATE);
   if (!_ELENA_::emptystr(projectTemplate)) {
      _ELENA_::ident_t templateFile = _settings.get(_ELENA_::opTemplates, projectTemplate, DEFAULT_STR);
      if (!_ELENA_::emptystr(templateFile)) {
         _ELENA_::Path templatePath(templateFile);

         loadConfig(templatePath.c_str(), false, false);
      }
      else raiseErrorIf(requiered, ELC_ERR_INVALID_TEMPLATE, projectTemplate);
   }

   // load config
   loadConfig(config, configPath);
}

bool _ELC_::Project :: loadProject(_ELENA_::path_t path)
{
   if (emptystr(projectName)) {
      projectName.copy(_ELENA_::IdentifierString(path));

      loadConfig(path);

      _ELENA_::Path projectPath;
      projectPath.copySubPath(path);
      _settings.add(_ELENA_::opProjectPath, _ELENA_::IdentifierString::clonePath(projectPath.c_str()));

      return true;
   }
   else return false;
}

void _ELC_::Project :: setOption(_ELENA_::path_t value)
{
   _ELENA_::IdentifierString valueName(value);

   switch ((char)value[0]) {
      case ELC_PRM_LIB_PATH:
         _settings.add(_ELENA_::opLibPath, valueName.clone(1));
         break;
      case ELC_PRM_OUTPUT_PATH:
         _settings.add(_ELENA_::opOutputPath, valueName.clone(1));
         break;
      case ELC_PRM_EXTRA:
         if (valueName.compare(ELC_PRM_TABSIZE, 4)) {
            _tabSize = valueName.ident().toInt(4);
         }
         else if (valueName.compare(ELC_PRM_CODEPAGE, 3)) {
            _encoding = valueName.ident().toInt(3);
         }
         else if (valueName.compare(ELC_PRM_PROJECTPATH, _ELENA_::getlength(ELC_PRM_PROJECTPATH))) {
            _settings.add(_ELENA_::opProjectPath, valueName.clone(_ELENA_::getlength(ELC_PRM_PROJECTPATH)));
         }
         else if (valueName.compare(ELC_PRM_OPTOFF)) {
            _settings.add(_ELENA_::opL0, 0);
            _settings.add(_ELENA_::opL1, 0);
         }
         else if (valueName.compare(ELC_PRM_OPT1OFF)) {
            _settings.add(_ELENA_::opL1, 0);
         }
         else raiseError(ELC_ERR_INVALID_OPTION, valueName);
         break;
      case ELC_PRM_WARNING:
         if (valueName.compare(ELC_W_WEAKUNRESOLVED)) {
            _settings.add(_ELENA_::opWarnOnWeakUnresolved, -1);
         }
         else if (valueName.compare(ELC_W_LEVEL1)) {
            _warningMasks = _ELENA_::WARNING_MASK_1;
         }
         else if (valueName.compare(ELC_W_LEVEL2)) {
            _warningMasks = _ELENA_::WARNING_MASK_2;
         }
         else if (valueName.compare(ELC_W_LEVEL3)) {
            _warningMasks = _ELENA_::WARNING_MASK_3;
         }
         else if (valueName.compare(ELC_W_OFF)) {
            _warningMasks = 0;
         }
         else raiseError(ELC_ERR_INVALID_OPTION, valueName);
         break;
//      case ELC_PRM_TARGET:
//         _settings.add(_ELENA_::opTarget, valueName.clone(1));
//         break;
      case ELC_PRM_DEBUGINFO:
         if (valueName.compare(ELC_SUBJECTINFO)) {
            _settings.add(_ELENA_::opDebugSubjectInfo, -1);
         }
         _settings.add(_ELENA_::opDebugMode, -1);
         break;
//      case ELC_PRM_CONFIG:
//      {
//         if (!loadProject(value + 1))
//            raiseError(ELC_ERR_INVALID_OPTION, valueName);
//
//         break;
//      }
      default:
         raiseError(ELC_ERR_INVALID_OPTION, valueName);
   }
}

_ELENA_::_JITCompiler* _ELC_::Project :: createJITCompiler()
{
   return new _ELENA_::x86JITCompiler(BoolSetting(_ELENA_::opDebugMode));
}

_ELENA_::_JITCompiler* _ELC_::Project::createJITCompiler64()
{
   return new _ELENA_::AMD64JITCompiler(BoolSetting(_ELENA_::opDebugMode));
}

inline void retrieveSubNs(_ELENA_::ident_t rootNs, _ELENA_::ident_t moduleNs, _ELENA_::ident_t filePath, _ELENA_::IdentifierString& retVal)
{
   if (!moduleNs.compare(rootNs)) {
      moduleNs = moduleNs + _ELENA_::getlength(rootNs) + 1;
   }
   else moduleNs = NULL;

   size_t start = 0;
   size_t ns_index = moduleNs ? moduleNs.find(start, '\'', getlength(moduleNs)) : NOTFOUND_POS;
   while (ns_index != NOTFOUND_POS) {
      if (moduleNs.compare(filePath, start, ns_index)) {
         start = ns_index + 1;
         ns_index = moduleNs.find(start, '\'', NOTFOUND_POS);

         filePath += start;
      }
      else break;
   }
   size_t index = filePath.findLast(PATH_SEPARATOR);
   if (index != NOTFOUND_POS && index > start) {
      retVal.copy(filePath + start, index - start);
   }
}

inline _ELENA_::SourceFileInfo* _ELC_::Project :: initSourceFileInfo(_ELENA_::CompilerScope& scope, _ELENA_::ident_t filePath)
{
   auto info = new _ELENA_::SourceFileInfo();

   info->tree = new _ELENA_::SyntaxTree();

   retrieveSubNs(StrSetting(_ELENA_::opNamespace), scope.module->Name(), filePath, info->ns);
   //size_t index = filePath.find(PATH_SEPARATOR);
   //if (index != NOTFOUND_POS) {
   //   info->ns.copy(filePath, index);
   //}
   info->path.copy(filePath);

   // declare a namespace
   scope.declareNamespace(info->ns.c_str());

   // add the module itself
   info->importedNs.add(scope.module->Name().clone());

   // system module should be included by default
   if (!scope.module->Name().compare(STANDARD_MODULE)) {
      info->importedNs.add(_ELENA_::ident_t(STANDARD_MODULE).clone());
   }

   return info;
}

void _ELC_::Project::buildSyntaxTree(_ELENA_::ScriptParser& parser, _ELENA_::FileMapping* source, _ELENA_::CompilerScope& scope, _ELENA_::SourceFileList& files)
{
   _ELENA_::ForwardIterator file_it = source->getIt(ELC_INCLUDE);
   while (!file_it.Eof()) {
      _ELENA_::ident_t filePath = *file_it;

      _ELENA_::SourceFileInfo* info = initSourceFileInfo(scope, filePath);

      try {
         // based on the target type generate the syntax tree for the file
         _ELENA_::Path fullPath(StrSetting(_ELENA_::opProjectPath));
         fullPath.combine(filePath);

         // parse
         parser.parse(fullPath.c_str(), *info->tree);

         files.add(info);
      }
      catch (_ELENA_::LineTooLong& e)
      {
         raiseError(errLineTooLong, filePath, e.row, 1);
      }
      catch (_ELENA_::InvalidChar& e)
      {
         size_t destLength = 6;

         _ELENA_::String<char, 6> symbol;
         _ELENA_::Convertor::copy(symbol, (_ELENA_::unic_c*)&e.ch, 1, destLength);

         raiseError(errInvalidChar, filePath, e.row, e.column, (const char*)symbol);
      }
      catch (_ELENA_::SyntaxError& e)
      {
         raiseError(e.error, filePath, e.row, e.column, e.token);
      }
      catch (_ELENA_::ScriptError& e)
      {
         raiseError(e.error, filePath);
      }

      file_it = source->nextIt(ELC_INCLUDE, file_it);
   }
}

void _ELC_::Project :: buildSyntaxTree(_ELENA_::Parser& parser, _ELENA_::FileMapping* source, _ELENA_::CompilerScope& scope, _ELENA_::SourceFileList& files)
{
   _ELENA_::ForwardIterator file_it = source->getIt(ELC_INCLUDE);
   while (!file_it.Eof()) {
      _ELENA_::ident_t filePath = *file_it;

      _ELENA_::SourceFileInfo* info = initSourceFileInfo(scope, filePath);

      try {
         // based on the target type generate the syntax tree for the file
         _ELENA_::Path fullPath(StrSetting(_ELENA_::opProjectPath));
         fullPath.combine(filePath);
      
         // parse
         _ELENA_::TextFileReader sourceFile(fullPath.c_str(), getDefaultEncoding(), true);
         if (!sourceFile.isOpened())
            raiseError(errInvalidFile, filePath);
      
         _ELENA_::DerivationWriter writer(*info->tree);
         writer.begin();
         parser.parse(&sourceFile, writer, getTabSize());      
         writer.close();

         _ELENA_::DerivationTransformer transformer(*info->tree);
         transformer.recognize(scope, info->path, info->ns, &info->importedNs);

         files.add(info);
      }
      catch (_ELENA_::LineTooLong& e)
      {
         raiseError(errLineTooLong, filePath, e.row, 1);
      }
      catch (_ELENA_::InvalidChar& e)
      {
         size_t destLength = 6;
      
         _ELENA_::String<char, 6> symbol;
         _ELENA_::Convertor::copy(symbol, (_ELENA_::unic_c*)&e.ch, 1, destLength);
      
         raiseError(errInvalidChar, filePath, e.row, e.column, (const char*)symbol);
      }
      catch (_ELENA_::SyntaxError& e)
      {
         raiseError(e.error, filePath, e.row, e.column, e.token);
      }

      file_it = source->nextIt(ELC_INCLUDE, file_it);
   }

   // generate the module members
   for (auto it = files.start(); !it.Eof(); it++) {
      _ELENA_::SyntaxTree* derivationTree = (*it)->tree;
      _ELENA_::SyntaxTree* syntaxTree = new _ELENA_::SyntaxTree();

      _ELENA_::SyntaxWriter syntaxWriter(*syntaxTree);
      _ELENA_::DerivationTransformer transformer(*derivationTree);
      syntaxWriter.newNode(_ELENA_::lxRoot);
      transformer.generate(syntaxWriter, scope, (*it)->path, (*it)->ns, &(*it)->importedNs);
      syntaxWriter.closeNode();

      (*it)->tree = syntaxTree;

      freeobj(derivationTree);
   }
}

bool _ELC_::Project :: compileSources(_ELENA_::Compiler& compiler, _ELENA_::Parser& parser)
{
   bool debugMode = BoolSetting(_ELENA_::opDebugMode);

   //_ELENA_::Unresolveds unresolveds(_ELENA_::Unresolved(), NULL);
   for (_ELENA_::SourceIterator it = _sources.start(); !it.Eof(); it++) {
      _ELENA_::Map<_ELENA_::ident_t, _ELENA_::ProjectSettings::VItem>* source = *it;

      // create module
      _ELENA_::CompilerScope scope(this);

      _ELENA_::ident_t name = source->get(ELC_NAMESPACE_KEY);
      compiler.initializeScope(name, scope, debugMode);

      _ELENA_::ident_t target = source->get(ELC_TARGET_NAME);
      int type = !emptystr(target) ? _targets.get(target, ELC_TYPE_NAME, 1) : 1;
      if (type == 1) {
         _ELENA_::SourceFileList files(NULL, _ELENA_::freeobj);
         
         printInfo("Parsing %s", name);

         // build derivation tree, recognize scopes and register all symbols
         buildSyntaxTree(parser, source, scope, files);

         printInfo("Compiling %s", name);

         scope.compile(compiler, files, NULL);
      }
      else if (type == 2) {
         // if it is a script file
         _ELENA_::ScriptParser scriptParser;

         // load options
         _ELENA_::Map<_ELENA_::ident_t, _ELENA_::TargetSettings::VItem>* targetInfo = _targets.get(target, (_ELENA_::Map<_ELENA_::ident_t, _ELENA_::TargetSettings::VItem>*)NULL);

         _ELENA_::TargetIterator option_it = targetInfo->getIt(ELC_OPTION);
         while (!option_it.Eof()) {
            if (!scriptParser.setOption(*option_it, StrSetting(_ELENA_::opProjectPath))) {
               raiseError(errInvalidTargetOption, *option_it);
            }

            option_it = targetInfo->nextIt(ELC_OPTION, option_it);
         }

         // compile script files
         _ELENA_::SourceFileList files(NULL, _ELENA_::freeobj);

         buildSyntaxTree(scriptParser, source, scope, files);

         scope.compile(compiler, files, NULL);
      }

      saveModule(scope.module, "nl");

      if (scope.debugModule)
         saveModule(scope.debugModule, "dnl");
   }

   // validate the unresolved forward refereces if unresolved reference warning is enabled
   //compiler.validateUnresolved(unresolveds, *this);

   return !HasWarnings();
}

//bool _ELC_::Project :: compileSources(_ELENA_::Compiler& compiler)
//{
//   return compiler.run(*this, BoolSetting(_ELENA_::opDebugMode));
//}

void setCompilerOptions(_ELC_::Project& project, _ELENA_::Compiler& compiler)
{
   if (project.IntSetting(_ELENA_::opL0, -1) != 0) {
      _ELENA_::Path rulesPath(project.StrSetting(_ELENA_::opAppPath), RULES_FILE);

      _ELENA_::FileReader rulesFile(rulesPath.c_str(), _ELENA_::feRaw, false);
      if (!rulesFile.isOpened()) {
         project.raiseWarning(0, errInvalidFile, RULES_FILE);
      }
      else compiler.loadRules(&rulesFile);
   }
   if (project.IntSetting(_ELENA_::opL1, -1) != 0) {
      compiler.turnOnOptimiation(1);
   }
}

// --- Main function ---

const char* showPlatform(int platform)
{
   if (platform == _ELENA_::ptWin32Console) {
      return ELC_WIN32CONSOLE;
   }
   else if (platform == _ELENA_::ptWin64Console) {
      return ELC_WIN64CONSOLE;
   }
   else if (platform == _ELENA_::ptWin32ConsoleX) {
      return ELC_WIN32CONSOLEX;
   }
   else if (platform == _ELENA_::ptVMWin32Console) {
      return ELC_WIN32VMCONSOLEX;
   }
   else if (platform == _ELENA_::ptWin32GUI) {
      return ELC_WIN32GUI;
   }
   else if (platform == _ELENA_::ptVMWin32GUI) {
      return ELC_WIN32VMGUI;
   }
   else if (platform == _ELENA_::ptWin32GUIX) {
      return ELC_WIN32GUIX;
   }
   else if (platform == _ELENA_::ptLibrary) {
      return ELC_LIBRARY;
   }
   else return ELC_UNKNOWN;
}

int main()
{
   int argc;
   wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

   int    exitCode = 0;
   _ELC_::Project project;

   try {
      print(ELC_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELC_REVISION_NUMBER);

      if (argc < 2) {
         // show help if no parameters proveded
         print(ELC_HELP_INFO);
         return -3;
      }

      // Initializing..
      _ELENA_::Path configPath(project.appPath.c_str(), DEFAULT_CONFIG);

      project.loadConfig(configPath.c_str(), true, false);

      // Initializing..
      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0]=='-') {
            project.setOption(argv[i] + 1);
         }
         else if (_ELENA_::Path::checkExtension(argv[i], "prj")) {
            project.loadProject(argv[i]);
         }
         else project.addSource(argv[i]);
      }

      project.initLoader();

      int platform = project.IntSetting(_ELENA_::opPlatform);

      // Greetings
      print(ELC_STARTING, (const char*)project.projectName, showPlatform(platform));

      // Cleaning up
      print("Cleaning up...");
      project.cleanUp();

      // Compiling..
      print(ELC_COMPILING);

      _ELENA_::Path syntaxPath(project.StrSetting(_ELENA_::opAppPath), SYNTAX_FILE);
      _ELENA_::FileReader syntaxFile(syntaxPath.c_str(), _ELENA_::feRaw, false);
      if (!syntaxFile.isOpened())
         project.raiseErrorIf(true, errInvalidFile, SYNTAX_FILE);

      // compile normal project
      bool result = false;
      _ELENA_::CompilerLogic elenaLogic;
      _ELENA_::Compiler compiler(&elenaLogic);
      _ELENA_::Parser parser(&syntaxFile);
      setCompilerOptions(project, compiler);

      result = project.compileSources(compiler, parser);

      if (result)
         print(ELC_SUCCESSFUL_COMPILATION);
      else {
         exitCode = -1;
         print(ELC_WARNING_COMPILATION);
      }

      // Linking..
      if (platform == _ELENA_::ptWin32Console) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);
         linker.run(project, image, (_ELENA_::ref_t) - 1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin64Console) {
         print(ELC_LINKING);

         _ELENA_::Linker linker(true);
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler64(), helper);
         linker.run(project, image, (_ELENA_::ref_t) - 1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin32ConsoleX) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);

         linker.run(project, image, helper.tls_directory);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptVMWin32Console) {
         print(ELC_LINKING);

         _ELENA_::VirtualMachineClientImage image(
            &project, project.createJITCompiler());

         _ELENA_::Linker linker;
         linker.run(project, image, (_ELENA_::ref_t) - 1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin32GUI) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);
         linker.run(project, image, (_ELENA_::ref_t) - 1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin32GUIX) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);
         linker.run(project, image, helper.tls_directory);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptVMWin32GUI) {
         print(ELC_LINKING);

         _ELENA_::VirtualMachineClientImage image(
            &project, project.createJITCompiler(), true);

         _ELENA_::Linker linker;
         linker.run(project, image, (_ELENA_::ref_t) - 1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptLibrary) {
         // no linking for the library
      }
      else print(ELC_UNKNOWN_PLATFORM);
   }
   catch(_ELENA_::InternalError& e) {
      print(_ELENA_::WideString(ELC_INTERNAL_ERROR), (const wchar_t*)_ELENA_::WideString(e.message));
      exitCode = -2;

      project.cleanUp();
   }
   catch(_ELENA_::JITUnresolvedException& ex)
   {
      project.printInfo(errUnresovableLink, ex.referenceInfo);
      print(ELC_UNSUCCESSFUL);
      exitCode = -2;

      project.cleanUp();
   }
   catch(_ELENA_::JITConstantExpectedException& ex)
   {
      project.printInfo(errConstantExpectedLink, ex.referenceInfo);
      print(ELC_UNSUCCESSFUL);
      exitCode = -2;

      project.cleanUp();
   }
   catch(_ELENA_::_Exception&) {
      print(ELC_UNSUCCESSFUL);
      exitCode = -2;

      project.cleanUp();
   }
   return exitCode;
}
