//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//      This file contains the common constants of the command-line
//      compiler and a ELC project class
//                                             (C)2005-2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef elcH
#define elcH 1

#include "config.h"
#include "jitcompiler.h"
#include "parser.h"
#include "project.h"
#include "compiler.h"
#include "compilerscope.h"
#include "errors.h"

// --- ELC common constants ---
#define ELC_REVISION_NUMBER         0x0282

// --- ELC default file names ---
#ifdef _WIN32

constexpr auto SYNTAX_FILE          = "syntax.dat";
constexpr auto RULES_FILE           = "rules.dat";
constexpr auto SOURCERULES_FILE     = "source_rules.dat";

#elif _LINUX

constexpr auto SYNTAX_FILE          = "/usr/share/elena/syntax.dat";
constexpr auto RULES_FILE           = "/usr/share/elena/rules.dat";
constexpr auto SOURCERULES_FILE     = "/usr/share/elena/source_rules.dat";

#endif

// --- ELC command-line parameters ---
#define ELC_PRM_DEBUGINFO           'd'
#define ELC_PRM_FORWARD             'f'
#define ELC_PRM_OUTPUT_PATH         'o'
#define ELC_PRM_LIB_PATH            'p'
#define ELC_PRM_TEMPLATE            't'
#define ELC_PRM_WARNING             'w'
#define ELC_W_WEAKUNRESOLVED        "wwun"
#define ELC_W_LEVEL1                "w1"
#define ELC_W_LEVEL2                "w2"
#define ELC_W_LEVEL3                "w3"
#define ELC_W_OFF                   "w-"
#define ELC_PRM_EXTRA               'x'
#define ELC_PRM_TABSIZE             "xtab"
#define ELC_PRM_PROJECTPATH         "xpath"
#define ELC_PRM_CODEPAGE            "xcp"
#define ELC_PRM_OPTOFF              "xo-"
#define ELC_PRM_OPT1OFF             "xo1-"
#define ELC_PRM_AUTOSTANDARDOFF     "xa-"

// --- ELC config categories ---
#define SOURCE_CATEGORY             "configuration/files/*"
#define FORWARD_CATEGORY            "configuration/forwards/*"
#define PRIMITIVE_CATEGORY          "configuration/primitives/*"
#define TEMPLATE_CATEGORY           "configuration/templates/*"
#define WINAPI_CATEGORY             "configuration/winapi/*"
#define EXTERNALS_CATEGORY          "configuration/externals/*"
#define REFERENCE_CATEGORY          "configuration/references/*"
#define TARGET_CATEGORY             "configuration/targets/*"

// --- ELC config settings ---
#define ELC_DEBUGINFO               "configuration/project/debuginfo"
#define ELC_CLASSSYMBOLLOAD         "configuration/project/classsymbolload"
constexpr auto ELC_EXTDISPATCHER  = "configuration/project/extdispatcher";
#define ELC_TARGET                  "configuration/project/executable"
#define ELC_MG_SIZE                 "configuration/linker/mgsize"
#define ELC_HEAP_COMMIT             "configuration/linker/heapcommit"
#define ELC_HEAP_RESERV             "configuration/linker/heapresrv"
#define ELC_YG_IMAGEBASE            "configuration/linker/imagebase"
#define ELC_LIB_PATH                "configuration/project/libpath"
#define ELC_SYSTEM_THREADMAX        "configuration/system/maxthread"
#define ELC_OUTPUT_PATH             "configuration/project/output"
#define ELC_NAMESPACE               "configuration/project/namespace"
#define ELC_STACK_COMMIT            "configuration/linker/stackcommit"
#define ELC_STACK_RESERV            "configuration/linker/stackresrv"
////#define ELC_PROJECT_START           "start"
#define ELC_PROJECT_TEMPLATE        "configuration/project/template"
#define ELC_PLATFORMTYPE            "configuration/system/platform"
#define ELC_WARNON_WEAKUNRESOLVED   "configuration/project/warn/weakunresolved"
////#define ELC_WARNON_SIGNATURE        "warn:signature"
#define ELC_YG_SIZE                 "configuration/linker/ygsize"
#define ELC_L0                      "configuration/compiler/l0"                // optimization: byte code optimization
#define ELC_L1                      "configuration/compiler/l1"                // optimization: source code optimization
#define ELC_PERM_SIZE               "configuration/linker/permsize"
#define ELC_OPSTACKALIGNMENT        "configuration/compiler/alignment"         // op code stack alignment mode - 4 or 8

#define ELC_TARGET_NAME             "target"
#define ELC_TYPE_NAME               "type"
#define ELC_INCLUDE                 "include"
#define ELC_NAMESPACE_KEY           "namespace"
#define ELC_NAME_KEY                "name"
#define ELC_OPTION                  "option"

#define ELC_MANIFEST_NAME           "configuration/manifest/name"
#define ELC_MANIFEST_VERSION        "configuration/manifest/version"
#define ELC_MANIFEST_AUTHOR         "configuration/manifest/author"

// --- ELC information messages ---
#define ELC_GREETING                "ELENA Command-line compiler %d.%d.%d (C)2005-2021 by Alex Rakov\n"
#define ELC_INTERNAL_ERROR          "Internal error:%s\n"
#define ELC_STARTING                "Project : %s, Platform: %s"
#define ELC_COMPILING               "Compiling..."
#define ELC_LINKING                 "Linking..."
#define ELC_SUCCESSFUL_COMPILATION  "\nSuccessfully compiled\n"
#define ELC_WARNING_COMPILATION     "Compiled with warnings\n"
#define ELC_UNSUCCESSFUL            "Compiled with errors\n"
#define ELC_SUCCESSFUL_LINKING      "Successfully linked\n"
#define ELC_UNKNOWN_PLATFORM        "Unsupported platform\n"
#define ELC_HELP_INFO               "elc {-key} <project file>\n\nkeys: -d<path>   - generates the debug info file\n      -f<f>=<r>  - resolves the forward f as a reference r\n      -o<path>   - sets the output path\n      -p<path>   - inlcudes the path to the library\n      -t<path>   - sets the target executable file name\n      -s<symbol> - resolves the entry forward symbol\n      -wun       - turns on unresolved reference warnings\n      -wX        - turns on warnings with level X=1,2,4\n      -wX-       - turns off warnings with level X=1,2,4\n      -wo-       - turns off optimization\n"

#define ELC_WIN32CONSOLE            "STA Win32 Console"
#define ELC_WIN64CONSOLE            "STA Win64 Console"
#define ELC_WIN32CONSOLEX           "MTA Win32 Console"
#define ELC_WIN32VMCONSOLEX         "STA Win32 VM Console"
//#define ELC_WIN32VMGUI              "STA Win32 VM GUI"
#define ELC_WIN32GUI                "STA Win32 GUI"
//#define ELC_WIN32GUIX               "MTA Win32 GUI"
#define ELC_LINUX32CONSOLE          "STA Linux i386 Console"
#define ELC_LIBRARY                 "library"
#define ELC_UNKNOWN                 "unknown"

// --- ELC error messages ---
constexpr auto ELC_ERR_INVALID_OPTION     = "elc: error 401: Invalid command line parameter '%s'\n";
constexpr auto ELC_ERR_INVALID_PATH       = "elc: error 402: Invalid or none-existing file '%s'\n";
constexpr auto ELC_ERR_INVALID_TEMPLATE   = "elc: error 404: Invalid or none-existing template '%s'\n";
constexpr auto ELC_ERR_INVALID_FORWARD    = "elc: error 404: Invalid forward '%s'\n";

namespace _ELC_
{

// --- ELC type definitions ---
typedef _ELENA_::XmlConfigFile ElcXmlConfigFile;

// --- Command Line Project ---
class Project : public _ELENA_::Project
{
   int _tabSize, _encoding;
   _ELENA_::ProjectSettings _sources;
   _ELENA_::TargetSettings  _targets;

   virtual bool readCategory(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting, _ELENA_::_ConfigFile::Nodes& list)
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

   virtual _ELENA_::ident_t getOption(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting)
   {
      switch (setting) {
         case _ELENA_::opNamespace:
            return config.getSetting(ELC_NAMESPACE);
         case _ELENA_::opGCMGSize:
            return config.getSetting(ELC_MG_SIZE);
         case _ELENA_::opGCYGSize:
            return config.getSetting(ELC_YG_SIZE);
         case _ELENA_::opGCPERMSize:
            return config.getSetting(ELC_PERM_SIZE);
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
         case _ELENA_::opClassSymbolAutoLoad:
            return config.getSetting(ELC_CLASSSYMBOLLOAD);
         case _ELENA_::opExtDispatchers:
            return config.getSetting(ELC_EXTDISPATCHER);
         case _ELENA_::opThreadMax:
            return config.getSetting(ELC_SYSTEM_THREADMAX);
         case _ELENA_::opL0:
            return config.getSetting(ELC_L0);
         case _ELENA_::opL1:
            return config.getSetting(ELC_L1);
            //   //   case _ELENA_::opL2:
            //   //      return config.getSetting(COMPILER_CATEGORY, ELC_L2);
         case _ELENA_::opOpStackAlignment:
            return config.getSetting(ELC_OPSTACKALIGNMENT);
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

   void buildSyntaxTree(_ELENA_::Parser& parser, _ELENA_::FileMapping* source, _ELENA_::ModuleScope& scope, _ELENA_::SyntaxTree& derivationTree)
   {
      _ELENA_::DerivationWriter writer(derivationTree, &scope);
      writer.newNodeDirectly(_ELENA_::lxRoot);

      auto file_it = source->getIt(ELC_INCLUDE);
      while (!file_it.Eof()) {
         _ELENA_::ident_t filePath = *file_it;

         try {
            // based on the target type generate the syntax tree for the file
            _ELENA_::Path fullPath(StrSetting(_ELENA_::opProjectPath));
            fullPath.combine(filePath);

            // parse
            _ELENA_::TextFileReader sourceFile(fullPath.c_str(), getDefaultEncoding(), true);
            if (!sourceFile.isOpened())
               raiseError(errInvalidFile, filePath);

            // declare a namespace
            writer.newNodeDirectly(_ELENA_::lxNamespace);
            writer.newNodeDirectly(_ELENA_::lxSourcePath, filePath);
            writer.closeNodeDirectly();

            parser.parse(&sourceFile, writer, getTabSize());

            writer.closeNodeDirectly();
         }
         catch (_ELENA_::LineTooLong& e)
         {
            raiseError(errLineTooLong, filePath, e.row, 1);
         }
         catch (_ELENA_::InvalidChar& e)
         {
            size_t destLength = 6;

            _ELENA_::String<char, 6> symbol;
            _ELENA_::Convertor::copy(symbol, (_ELENA_::unic_c*) & e.ch, 1, destLength);

            raiseError(errInvalidChar, filePath, e.row, e.column, (const char*)symbol);
         }
         catch (_ELENA_::SyntaxError& e)
         {
            raiseError(e.error, filePath, e.row, e.column, e.token);
         }

         file_it = source->nextIt(ELC_INCLUDE, file_it);
      }

      writer.closeNodeDirectly();
   }

   void buildSyntaxTree(_ELENA_::ScriptParser& parser, _ELENA_::FileMapping* source, _ELENA_::ModuleScope&, _ELENA_::SyntaxTree& derivationTree)
   {
      //_ELENA_::ForwardIterator file_it = source->getIt(ELC_INCLUDE);
      //while (!file_it.Eof()) {
      //   _ELENA_::ident_t filePath = *file_it;

      //   try {
      //      // based on the target type generate the syntax tree for the file
      //      _ELENA_::Path fullPath(StrSetting(_ELENA_::opProjectPath));
      //      fullPath.combine(filePath);

      //      parser.parse(fullPath.c_str(), derivationTree);
      //      // NOTE : source path node should be the first one due to current implementation
      //      derivationTree.readRoot().firstChild().insertNode(_ELENA_::lxSourcePath, filePath);
      //   }
      //   catch (_ELENA_::LineTooLong& e)
      //   {
      //      raiseError(errLineTooLong, filePath, e.row, 1);
      //   }
      //   catch (_ELENA_::InvalidChar& e)
      //   {
      //      size_t destLength = 6;

      //      _ELENA_::String<char, 6> symbol;
      //      _ELENA_::Convertor::copy(symbol, (_ELENA_::unic_c*) & e.ch, 1, destLength);

      //      raiseError(errInvalidChar, filePath, e.row, e.column, (const char*)symbol);
      //   }
      //   catch (_ELENA_::SyntaxError& e)
      //   {
      //      raiseError(e.error, filePath, e.row, e.column, e.token);
      //   }
      //   catch (_ELENA_::ScriptError& e)
      //   {
      //      raiseError(e.error, filePath);
      //   }

      //   file_it = source->nextIt(ELC_INCLUDE, file_it);
      //}
   }

public:
   _ELENA_::Path appPath;
   _ELENA_::IdentifierString projectName;

   _ELENA_::_JITCompiler* createJITCompiler();
   _ELENA_::_JITCompiler* createJITCompiler64();

   virtual void printInfo(const char* msg, _ELENA_::ReferenceInfo param);
   virtual void printInfo(const char* msg, _ELENA_::ident_t param);

   virtual void raiseError(_ELENA_::ident_t msg);
   virtual void raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal = NULL);
   virtual void raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t value);
   virtual void raiseErrorIf(bool throwExecption, _ELENA_::ident_t msg, _ELENA_::ident_t identifier);

   virtual void raiseWarning(int level, _ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal);
   virtual void raiseWarning(int level, _ELENA_::ident_t msg, _ELENA_::ident_t path);

#ifdef _WIN32
   virtual void addSource(_ELENA_::path_t path, _ELENA_::path_t wideNs)
   {
      _ELENA_::IdentifierString ns;
      ns.copyWideStr(wideNs);

      addSource(path, ns.c_str());
   }
#endif // _WIN32
   virtual void addSource(_ELENA_::path_t path, _ELENA_::ident_t ns)
   {
      _ELENA_::Path modulePath;
      _ELENA_::ReferenceNs name(ns);

      // build module namespace
      modulePath.copySubPath(path);
      name.pathToName(modulePath.c_str());

      int key = 0;
      for (auto it = _sources.start(); !it.Eof(); it++) {
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

   bool loadProject(_ELENA_::path_t path)
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

   virtual void addModule(_ELENA_::_ConfigFile::Node moduleNode)
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

   virtual void addTarget(_ELENA_::_ConfigFile::Node moduleNode)
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

   virtual void loadConfig(_ELENA_::_ConfigFile& config, _ELENA_::path_t configPath)
   {
      _ELENA_::Project::loadConfig(config, configPath);
   }

   void loadConfig(_ELENA_::path_t path, bool root = false, bool requiered = true)
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

   void loadGenericConfig(_ELENA_::_ConfigFile& config, _ELENA_::path_t configPath, bool root, bool requiered)
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

   void loadDefaultConfig(_ELENA_::ident_t defaultName)
   {
      auto it = _settings.getIt(_ELENA_::opTemplates);
      if (!it.Eof()) {
         projectName.copy(defaultName);

         _ELENA_::Path templatePath(*it);

         loadConfig(templatePath.c_str());

         _settings.add(_ELENA_::opTarget, defaultName.clone());

         _settings.add(_ELENA_::opNamespace, defaultName.clone());
      }
   }

   void setOption(_ELENA_::path_t value, bool& withoutProject)
   {
      _ELENA_::IdentifierString valueName(value);

      switch ((char)value[0]) {
         case ELC_PRM_TEMPLATE:
         {
            _ELENA_::ident_t templateFile = _settings.get(_ELENA_::opTemplates, valueName.c_str() + 1, DEFAULT_STR);
            if (!_ELENA_::emptystr(templateFile)) {
               _ELENA_::Path templatePath(templateFile);

               loadConfig(templatePath.c_str(), false, false);
               withoutProject = false;
            }
            else raiseError(ELC_ERR_INVALID_TEMPLATE, valueName.c_str() + 1);
            break;
         }
         case ELC_PRM_FORWARD:
         {
            size_t sep = valueName.ident().find('=', NOTFOUND_POS);
            if (sep != NOTFOUND_POS) {
               _ELENA_::ident_t reference = valueName.c_str() + sep + 1;
               _ELENA_::IdentifierString forward(valueName.ident() + 1, sep - 1);

               addForward(forward.c_str(), reference.c_str());
            }
            else raiseError(ELC_ERR_INVALID_FORWARD, valueName.c_str() + 1);
            break;
         }
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
            else if (valueName.compare(ELC_PRM_AUTOSTANDARDOFF)) {
               _settings.add(_ELENA_::opAutoSystemImport, 0);
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

   virtual int getDefaultEncoding() { return _encoding; }

   virtual int getTabSize() { return _tabSize; }

   bool compileSources(_ELENA_::Compiler& compiler, _ELENA_::Parser& parser)
   {
      bool debugMode = BoolSetting(_ELENA_::opDebugMode);

      //_ELENA_::Unresolveds unresolveds(_ELENA_::Unresolved(), NULL);
      _ELENA_::SyntaxTree derivationTree;
      for (_ELENA_::SourceIterator it = _sources.start(); !it.Eof(); it++) {
         _ELENA_::Map<_ELENA_::ident_t, _ELENA_::ProjectSettings::VItem>* source = *it;

         // create module
         _ELENA_::ModuleScope scope(this, &compiler);

         _ELENA_::ident_t name = source->get(ELC_NAMESPACE_KEY);
         compiler.initializeScope(name, scope, debugMode, IntSetting(_ELENA_::opOpStackAlignment));

         printInfo("Parsing %s", name);

         _ELENA_::ident_t target = source->get(ELC_TARGET_NAME);
         int type = !emptystr(target) ? _targets.get(target, ELC_TYPE_NAME, 1) : 1;
         if (type == 1) {
            derivationTree.clear();

            // build derivation tree, recognize scopes and register all symbols
            buildSyntaxTree(parser, source, scope, derivationTree);

            printInfo("Compiling %s", name);

            scope.compile(derivationTree, nullptr, nullptr);
         }
         else if (type == 2) {
            derivationTree.clear();

            // if it is a script file
            _ELENA_::ScriptParser scriptParser;

            // load options
            auto targetInfo = _targets.get(target, (_ELENA_::Map<_ELENA_::ident_t, _ELENA_::TargetSettings::VItem>*)NULL);

            _ELENA_::TargetIterator option_it = targetInfo->getIt(ELC_OPTION);
            while (!option_it.Eof()) {
               if (!scriptParser.setOption(*option_it, StrSetting(_ELENA_::opProjectPath))) {
                  raiseError(errInvalidTargetOption, *option_it);
               }

               option_it = targetInfo->nextIt(ELC_OPTION, option_it);
            }

            // compile script files
            buildSyntaxTree(scriptParser, source, scope, derivationTree);

            printInfo("Compiling %s", name);

            scope.compile(derivationTree, nullptr, nullptr);
         }

         saveModule(scope.module, "nl");

         if (scope.debugModule)
            saveModule(scope.debugModule, "dnl");
      }

      // validate the unresolved forward refereces if unresolved reference warning is enabled
      //compiler.validateUnresolved(unresolveds, *this);

      return !HasWarnings();
   }

   void setCompilerOptions(_ELENA_::Compiler& compiler)
   {
      if (IntSetting(_ELENA_::opL0, -1) != 0) {
         _ELENA_::Path rulesPath(StrSetting(_ELENA_::opAppPath), RULES_FILE);
         _ELENA_::Path sourceRulesPath(StrSetting(_ELENA_::opAppPath), SOURCERULES_FILE);

         _ELENA_::FileReader rulesFile(rulesPath.c_str(), _ELENA_::feRaw, false);
         if (!rulesFile.isOpened()) {
            raiseWarning(0, errInvalidFile, RULES_FILE);
         }
         else compiler.loadRules(&rulesFile);

         _ELENA_::FileReader sourceRulesFile(sourceRulesPath.c_str(), _ELENA_::feRaw, false);
         if (!rulesFile.isOpened()) {
            raiseWarning(0, errInvalidFile, RULES_FILE);
         }
         else compiler.loadSourceRules(&sourceRulesFile);
      }
      if (IntSetting(_ELENA_::opL1, -1) != 0) {
         compiler.turnOnOptimiation(1);
      }
      //if (IntSetting(_ELENA_::opEvenStackMode, 0) != 0) {
      //   compiler.turnOnEvenStack();
      //}
      if (IntSetting(_ELENA_::opAutoSystemImport, -1) != 0) {
         compiler.turnAutoImport(true);
      }
      if (_ELENA_::test(_warningMasks, _ELENA_::WARNING_LEVEL_3)) {
         compiler.turnOnTrackingUnassigned();
      }
   }

   void cleanUp();

   Project();
};

} // _ELC_

#endif // elcH
