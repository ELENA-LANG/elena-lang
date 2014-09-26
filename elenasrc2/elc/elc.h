//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//      This file contains the common constants of the command-line
//      compiler and a ELC project class
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elcH
#define elcH 1

#include "project.h"
#include "config.h"
#include "jitcompiler.h"

// --- ELC default file names ---
#ifdef _WIN32

#define SYNTAX_FILE                 "syntax.dat"
#define RULES_FILE                  "rules.dat"

#else

#define SYNTAX_FILE                 "/usr/share/elena/syntax.dat"
#define RULES_FILE                  "rules.dat"

#endif

// --- ELC common constants ---
#define ELC_BUILD_NUMBER             0x001D

// --- ELC command-line parameters ---
#define ELC_PRM_CONFIG              'c'
#define ELC_PRM_DEBUGINFO           'd'
//#define ELC_PRM_ENTRY               'e'
//#define ELC_PRM_PACKAGE             'g'
//#define ELC_PRM_LIBRARY             'l'
//#define ELC_PRM_MAP                 'm'
#define ELC_PRM_OUTPUT_PATH         'o'
#define ELC_PRM_LIB_PATH            'p'
//#define ELC_PRM_START               's'
#define ELC_PRM_TARGET              't'
#define ELC_PRM_WARNING             'w'
#define ELC_W_UNRESOLVED            "wun"
#define ELC_W_WEAKUNRESOLVED        "wwun"
#define ELC_PRM_EXTRA               'x'
#define ELC_PRM_TABSIZE             "xtab"
#define ELC_PRM_PROJECTPATH         "xpath"
#define ELC_PRM_CODEPAGE            "xcp"
#define ELC_PRM_OPTOFF              "xo-"
#define ELC_PRM_SYMBOLEMBEDOFF      "xembed-"

// --- ELC config categories ---
#define COMPILER_CATEGORY           "compiler"
#define SOURCE_CATEGORY             "files"
#define FORWARD_CATEGORY            "forwards"
#define LINKER_CATEGORY             "linker"
#define PRIMITIVE_CATEGORY          "primitives"
#define PROJECT_CATEGORY            "project"
#define SYSTEM_CATEGORY             "system"
#define TEMPLATE_CATEGORY           "templates"

// --- ELC config settings ---
#define ELC_DEBUGINFO               "debuginfo"
#define ELC_PROJECT_ENTRY           "entry"
#define ELC_TARGET                  "executable"
#define ELC_MG_SIZE                 "mgsize"
#define ELC_GC_OBJSIZE              "objsize"
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
#define ELC_VM_PATH                 "vmpath"
#define ELC_WARNON_UNRESOLVED       "warn:unresolved"
//#define ELC_WARNON_SIGNATURE        "warn:signature"
#define ELC_YG_SIZE                 "ygsize"
#define ELC_L0                      "l0"                // optimization: byte code optimization
#define ELC_SYMBOLINFO              "symbolembed"

// --- ELC information messages ---
#define ELC_GREETING                "ELENA command-line compiler %d.%d.%d (C)2005-2014 by Alex Rakov\n"
#define ELC_INTERNAL_ERROR          "Internal error:%s\n"
#define ELC_COMPILING               "Compiling..."
#define ELC_LINKING                 "Linking..."
#define ELC_SUCCESSFUL_COMPILATION  "Successfully compiled\n"
#define ELC_WARNING_COMPILATION     "Compiled with warnings\n"
#define ELC_UNSUCCESSFUL            "Compiled with errors\n"
#define ELC_SUCCESSFUL_LINKING      "Successfully linked\n"
#define ELC_HELP_INFO               "elc {-key} {<input file>}\n\nkeys: -c<path>   - specifies the project file\n      -d<path>   - generates the debug info file\n      -e<symbol> - resolves the entry forward symbol\n      -g<name>   - specifies the package name\n      -lstd      - sets standard module flag\n      -m<path>   - generates the map file\n      -o<path>   - sets the output path\n      -p<path>   - inlcudes the path to the library\n      -t<path>   - sets the target executable file name\n      -wun       - sets on the unresolved warnings\n      -xguit     - sets GUI application type\n"

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
   virtual const char* getOption(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting);

public:
   _ELENA_::Path appPath;

   _ELENA_::_JITCompiler* createJITCompiler();

   virtual void printInfo(const char* msg, const char* value);
   virtual void printInfo(const char* msg, const wchar16_t* param);

   virtual void raiseError(const char* msg);
   virtual void raiseError(const char* msg, const tchar_t* path, int row, int column, const wchar16_t* terminal);
   virtual void raiseError(const char* msg, const char* value);
   virtual void raiseError(const char* msg, const wchar16_t* value);

   virtual void raiseErrorIf(bool throwExecption, const char* msg, const tchar_t* path);

   virtual void raiseWarning(const char* msg, const tchar_t* path, int row, int column, const wchar16_t* terminal);
   virtual void raiseWarning(const char* msg, const tchar_t* path);

   void addSource(const tchar_t* path);

   virtual void loadConfig(_ELENA_::_ConfigFile& config, const tchar_t* configPath)
   {
      _ELENA_::Project::loadConfig(config, configPath);
   }

   virtual void loadConfig(const tchar_t* path, bool root = false, bool requiered = true);

   void setOption(const tchar_t* value);

   virtual int getDefaultEncoding() { return _encoding; }

   virtual int getTabSize() { return _tabSize; }

   void cleanUp();

   Project();
};

} // _ELC_

#endif // elcH
