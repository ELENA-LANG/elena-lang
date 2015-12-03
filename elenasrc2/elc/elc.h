//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//      This file contains the common constants of the command-line
//      compiler and a ELC project class
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elcH
#define elcH 1

#include "project.h"
#include "config.h"
#include "jitcompiler.h"

// --- ELC common constants ---
#define ELC_REVISION_NUMBER             0x0028

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
#define ELC_PRM_SUBJECTINFO         'ds'
#define ELC_PRM_OUTPUT_PATH         'o'
#define ELC_PRM_LIB_PATH            'p'
#define ELC_PRM_START               's'
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
#define COMPILER_CATEGORY           "compiler"
#define SOURCE_CATEGORY             "files"
#define FORWARD_CATEGORY            "forwards"
#define LINKER_CATEGORY             "linker"
#define PRIMITIVE_CATEGORY          "primitives"
#define PROJECT_CATEGORY            "project"
#define SYSTEM_CATEGORY             "system"
#define TEMPLATE_CATEGORY           "templates"
#define WINAPI_CATEGORY             "winapi"
#define EXTERNALS_CATEGORY          "externals"

// --- ELC config settings ---
#define ELC_DEBUGINFO               "debuginfo"
#define ELC_SUBJECTINFO             "subjectinfo"
#define ELC_PROJECT_ENTRY           "entry"
#define ELC_TARGET                  "executable"
#define ELC_MG_SIZE                 "mgsize"
//#define ELC_HEAP_COMMIT             "heapcommit"
//#define ELC_HEAP_RESERV             "heapresrv"
////#define ELC_YG_IMAGEBASE            "imagebase"
#define ELC_LIB_PATH                "libpath"
#define ELC_SYSTEM_THREADMAX        "maxthread"
#define ELC_OUTPUT_PATH             "output"
#define ELC_NAMESPACE               "namespace"
////#define ELC_STACK_COMMIT            "stackcommit"
////#define ELC_STACK_RESERV            "stackresrv"
////#define ELC_PROJECT_START           "start"
#define ELC_PROJECT_TEMPLATE        "template"
#define ELC_PLATFORMTYPE            "platform"
#define ELC_WARNON_UNRESOLVED       "warn:unresolved"
//#define ELC_WARNON_SIGNATURE        "warn:signature"
#define ELC_YG_SIZE                 "ygsize"
#define ELC_L0                      "l0"                // optimization: byte code optimization
#define ELC_L1                      "l1"                // optimization: source code optimization

// --- ELC information messages ---
#define ELC_GREETING                "ELENA command-line compiler %d.%d.%d (C)2005-2015 by Alex Rakov\n"
#define ELC_INTERNAL_ERROR          "Internal error:%s\n"
#define ELC_STARTING                "Project : %s, Platform: %s"
#define ELC_COMPILING               "Compiling..."
#define ELC_LINKING                 "Linking..."
#define ELC_SUCCESSFUL_COMPILATION  "Successfully compiled\n"
#define ELC_WARNING_COMPILATION     "Compiled with warnings\n"
#define ELC_UNSUCCESSFUL            "Compiled with errors\n"
#define ELC_SUCCESSFUL_LINKING      "Successfully linked\n"
#define ELC_UNKNOWN_PLATFORM        "Unsupported platform\n"
#define ELC_HELP_INFO               "elc {-key} {<input file>}\n\nkeys: -c<path>   - specifies the project file\n      -d<path>   - generates the debug info file\n      -o<path>   - sets the output path\n      -p<path>   - inlcudes the path to the library\n      -t<path>   - sets the target executable file name\n      -s<symbol> - resolves the entry forward symbol\n      -wun       - turns on unresolved reference warnings\n      -wX        - turns on warnings with level X=1,2,4\n      -wX-       - turns off warnings with level X=1,2,4\n      -wo-       - turns off optimization\n"

#define ELC_WIN32CONSOLE            "win32 console"
#define ELC_WIN32CONSOLEX           "win32 console x"
#define ELC_WIN32VMCONSOLEX         "win32 vm client console"
#define ELC_WIN32GUI                "win32 GUI"
#define ELC_WIN32GUIX               "win32 GUI x"
#define ELC_LINUX32CONSOLE          "linux i386 console"
#define ELC_LIBRARY                 "library"
#define ELC_UNKNOWN                 "unknown"

// --- ELC error messages ---
#define ELC_ERR_INVALID_OPTION	   "elc: error 401: Invalid command line parameter '%c'\n"
#define ELC_ERR_INVALID_PATH        "elc: error 402: Invalid or none-existing file '%s'\n"
#define ELC_ERR_INVALID_TEMPLATE    "elc: error 404: Invalid or none-existing template '%s'\n"

#define ELC_WRN_MISSING_VMPATH      "elc: warning 411: Missing project/vmpath\n"

namespace _ELC_
{

// --- ELC type definitions ---
typedef _ELENA_::IniConfigFile ElcConfigFile;

// --- Command Line Project ---
class Project : public _ELENA_::Project
{
   int _tabSize, _encoding;

   virtual _ELENA_::ConfigCategoryIterator getCategory(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting);
   virtual _ELENA_::ident_t getOption(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting);

public:
   _ELENA_::Path appPath;
   _ELENA_::IdentifierString projectName;

   _ELENA_::_JITCompiler* createJITCompiler();

   virtual void printInfo(const char* msg, _ELENA_::ident_t param);

   //virtual void raiseError(const char* msg);
   virtual void raiseError(const char* msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal);
   virtual void raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t value);
   virtual void raiseErrorIf(bool throwExecption, _ELENA_::ident_t msg, _ELENA_::ident_t identifier);

   virtual void raiseWarning(_ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal);
   virtual void raiseWarning(_ELENA_::ident_t msg, _ELENA_::ident_t path);

   void addSource(_ELENA_::path_t path);

   virtual void loadConfig(_ELENA_::_ConfigFile& config, _ELENA_::path_t configPath)
   {
      _ELENA_::Project::loadConfig(config, configPath);
   }

   virtual void loadConfig(_ELENA_::path_t path, bool root = false, bool requiered = true);

   void setOption(_ELENA_::path_t value);

   virtual int getDefaultEncoding() { return _encoding; }

   //virtual int getTabSize() { return _tabSize; }

   void cleanUp();

   Project();
};

} // _ELC_

#endif // elcH
