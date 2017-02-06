//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//      This file contains the common constants of the command-line
//      compiler and a ELC project class
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elcH
#define elcH 1

#include "project.h"
#include "config.h"
#include "jitcompiler.h"

// --- ELC common constants ---
#define ELC_REVISION_NUMBER             0x001C

// --- ELC default file names ---
#ifdef _WIN32

#define SYNTAX_FILE                 "syntax.dat"
#define RULES_FILE                  "rules.dat"

#else

#define SYNTAX_FILE                 "/usr/share/elena/syntax.dat"
#define RULES_FILE                  "/usr/share/elena/rules.dat"

#endif

// --- ELC command-line parameters ---
#define ELC_PRM_CONFIG              'c'
#define ELC_PRM_DEBUGINFO           'd'
//#define ELC_PRM_SUBJECTINFO         'ds'
#define ELC_PRM_OUTPUT_PATH         'o'
#define ELC_PRM_LIB_PATH            'p'
#define ELC_PRM_TARGET              't'
#define ELC_PRM_WARNING             'w'
#define ELC_W_UNRESOLVED            "wun"
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

// --- ELC config categories ---
#define SOURCE_CATEGORY             "files/*"
#define FORWARD_CATEGORY            "forwards/*"
#define PRIMITIVE_CATEGORY          "primitives/*"
#define TEMPLATE_CATEGORY           "templates/*"
#define WINAPI_CATEGORY             "winapi/*"
#define EXTERNALS_CATEGORY          "externals/*"
#define REFERENCE_CATEGORY          "references/*"
#define TARGET_CATEGORY             "targets/*"

// --- ELC config settings ---
#define ELC_DEBUGINFO               "project/debuginfo"
#define ELC_SUBJECTINFO             "project/subjectinfo"
#define ELC_CLASSSYMBOLLOAD         "project/classsymbolload"
#define ELC_TARGET                  "project/executable"
#define ELC_MG_SIZE                 "linker/mgsize"
#define ELC_HEAP_COMMIT             "linker/heapcommit"
#define ELC_HEAP_RESERV             "linker/heapresrv"
#define ELC_YG_IMAGEBASE            "linker/imagebase"
#define ELC_LIB_PATH                "project/libpath"
#define ELC_SYSTEM_THREADMAX        "system/maxthread"
#define ELC_OUTPUT_PATH             "project/output"
#define ELC_NAMESPACE               "project/namespace"
#define ELC_STACK_COMMIT            "linker/stackcommit"
#define ELC_STACK_RESERV            "linker/stackresrv"
//#define ELC_PROJECT_START           "start"
#define ELC_PROJECT_TEMPLATE        "project/template"
#define ELC_PLATFORMTYPE            "system/platform"
#define ELC_WARNON_UNRESOLVED       "project/warn/unresolved"
#define ELC_WARNON_WEAKUNRESOLVED   "project/warn/weakunresolved"
//#define ELC_WARNON_SIGNATURE        "warn:signature"
#define ELC_YG_SIZE                 "linker/ygsize"
#define ELC_L0                      "compiler/l0"                // optimization: byte code optimization
#define ELC_L1                      "compiler/l1"                // optimization: source code optimization

#define ELC_TARGET_NAME             "target"
#define ELC_TYPE_NAME               "type"
#define ELC_INCLUDE                 "include"
#define ELC_NAMESPACE_KEY           "namespace"
#define ELC_NAME_KEY                "name"
#define ELC_OPTION                  "option"

#define ELC_MANIFEST_NAME           "manifest/name"
#define ELC_MANIFEST_VERSION        "manifest/version"
#define ELC_MANIFEST_AUTHOR         "manifest/author"

// --- ELC information messages ---
#define ELC_GREETING                "ELENA Command-line compiler %d.%d.%d (C)2005-2017 by Alex Rakov\n"
#define ELC_INTERNAL_ERROR          "Internal error:%s\n"
#define ELC_STARTING                "Project : %s, Platform: %s"
#define ELC_COMPILING               "Compiling..."
#define ELC_LINKING                 "Linking..."
#define ELC_SUCCESSFUL_COMPILATION  "\nSuccessfully compiled\n"
#define ELC_WARNING_COMPILATION     "Compiled with warnings\n"
#define ELC_UNSUCCESSFUL            "Compiled with errors\n"
#define ELC_SUCCESSFUL_LINKING      "Successfully linked\n"
#define ELC_UNKNOWN_PLATFORM        "Unsupported platform\n"
#define ELC_HELP_INFO               "elc {-key} <project file>\n\nkeys: -d<path>   - generates the debug info file\n      -o<path>   - sets the output path\n      -p<path>   - inlcudes the path to the library\n      -t<path>   - sets the target executable file name\n      -s<symbol> - resolves the entry forward symbol\n      -wun       - turns on unresolved reference warnings\n      -wX        - turns on warnings with level X=1,2,4\n      -wX-       - turns off warnings with level X=1,2,4\n      -wo-       - turns off optimization\n"

#define ELC_WIN32CONSOLE            "STA Win32 Console"
#define ELC_WIN64CONSOLE            "STA Win64 Console"
#define ELC_WIN32CONSOLEX           "MTA Win32 Console"
#define ELC_WIN32VMCONSOLEX         "STA Win32 VM Console"
#define ELC_WIN32GUI                "STA Win32 GUI"
#define ELC_WIN32GUIX               "MTA Win32 GUI"
#define ELC_LINUX32CONSOLE          "STA Linux i386 Console"
#define ELC_LIBRARY                 "library"
#define ELC_UNKNOWN                 "unknown"

// --- ELC error messages ---
#define ELC_ERR_INVALID_OPTION	   "elc: error 401: Invalid command line parameter '%c'\n"
#define ELC_ERR_INVALID_PATH        "elc: error 402: Invalid or none-existing file '%s'\n"
#define ELC_ERR_INVALID_TEMPLATE    "elc: error 404: Invalid or none-existing template '%s'\n"

//#define ELC_WRN_MISSING_VMPATH      "elc: warning 411: Missing project/vmpath\n"

namespace _ELC_
{

// --- ELC type definitions ---
typedef _ELENA_::IniConfigFile ElcConfigFile;
typedef _ELENA_::XmlConfigFile ElcXmlConfigFile;

// --- Command Line Project ---
class Project : public _ELENA_::Project
{
   int _tabSize, _encoding;

   virtual bool readCategory(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting, _ELENA_::_ConfigFile::Nodes& list);
   virtual _ELENA_::ident_t getOption(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting);

public:
   _ELENA_::Path appPath;
   _ELENA_::IdentifierString projectName;

   _ELENA_::_JITCompiler* createJITCompiler();
   _ELENA_::_JITCompiler* createJITCompiler64();

   virtual void printInfo(const char* msg, _ELENA_::ident_t param);

   //virtual void raiseError(const char* msg);
   virtual void raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal = NULL);
   virtual void raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t value);
   virtual void raiseErrorIf(bool throwExecption, _ELENA_::ident_t msg, _ELENA_::ident_t identifier);

   virtual void raiseWarning(_ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal);
   virtual void raiseWarning(_ELENA_::ident_t msg, _ELENA_::ident_t path);

   virtual void addSource(_ELENA_::path_t path);
   bool loadProject(_ELENA_::path_t path);

   virtual void addModule(_ELENA_::_ConfigFile::Node moduleNode);
   virtual void addTarget(_ELENA_::_ConfigFile::Node moduleNode);

   virtual void loadConfig(_ELENA_::_ConfigFile& config, _ELENA_::path_t configPath)
   {
      _ELENA_::Project::loadConfig(config, configPath);
   }

   virtual void loadConfig(_ELENA_::path_t path, bool root = false, bool requiered = true);

   void loadGenericConfig(_ELENA_::_ConfigFile& config, _ELENA_::path_t configPath, bool root, bool requiered);
   void loadIniConfig(_ELENA_::path_t path, bool root, bool requiered);
   void loadXMLConfig(_ELENA_::path_t path, bool root, bool requiered);

   void setOption(_ELENA_::path_t value);

   virtual int getDefaultEncoding() { return _encoding; }

   virtual int getTabSize() { return _tabSize; }

   bool compileSources(_ELENA_::Compiler& compiler, _ELENA_::Parser& parser);

   void cleanUp();

   Project();
};

} // _ELC_

#endif // elcH
