//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains the declaration of the base class implementing
//      ELENA Project interface.
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef projectH
#define projectH 1

#include "libman.h"

namespace _ELENA_
{

// --- Project warning levels
const int WARNING_LEVEL_1 = 1;
const int WARNING_LEVEL_2 = 2;
const int WARNING_LEVEL_3 = 4;

const int WARNING_MASK_1 = 1;
const int WARNING_MASK_2 = 3;
const int WARNING_MASK_3 = 7;

// --- Project list types ---
//typedef String<wchar16_t, IDENTIFIER_LEN>      ProjectParam;
typedef Dictionary2D<int, ident_t>    ProjectSettings;
typedef Map<ident_t, ident_c*>        Sources;

typedef Map<ident_t, ident_c*> :: Iterator                                                    SourceIterator;
typedef _Iterator<ProjectSettings::VItem, _MapItem<ident_t, ProjectSettings::VItem>, ident_t> ForwardIterator;

// --- ELENA Project options ---
enum ProjectSetting
{
   opNone                  = 0x0000,

   // compiler options
   opAppPath		         = 0x0001,
   opProjectPath           = 0x0002,
   opLibPath               = 0x0003,
   opNamespace             = 0x0004,
   opTarget                = 0x0006,
   opOutputPath            = 0x0008,
   opEntry                 = 0x0009,
   opDebugMode             = 0x000A,
   opTemplate              = 0x000C,
   opThreadMax             = 0x0013,
   opDebugSubjectInfo      = 0x0014,

   // linker options
   opImageBase             = 0x0020,
   opSectionAlignment      = 0x0021,
   opFileAlignment         = 0x0022,
   opGCMGSize              = 0x0024,
   opSizeOfStackReserv     = 0x0026,
   opSizeOfStackCommit     = 0x0027,
   opSizeOfHeapReserv      = 0x0028,
   opSizeOfHeapCommit      = 0x0029,
   opPlatform              = 0x002A,      // defines the project platform type
   opGCYGSize              = 0x002B,

   // compiler engine options
   opWarnOnUnresolved      = 0x0041,
   opWarnOnWeakUnresolved  = 0x0042,
//   opWarnOnSignature       = 0x0043,

   // compiler optimization options
   opL0                    = 0x0050,   // byte-code optimization
   opL1                    = 0x0051,   // source-code optimization

   opPrimitives            = 0x0060,
   opForwards              = 0x0061,
   opSources               = 0x0062,
   opTemplates             = 0x0063,
   opExternals             = 0x0064,
   opWinAPI                = 0x0065   // used only for WIN32
};

// --- ModuleInfo ---
struct ModuleInfo
{
   _Module* codeModule;
   _Module* debugModule;

   ModuleInfo()
   {
      codeModule = debugModule = NULL;
   }

   ModuleInfo(_Module* codeModule, _Module* debugModule)
   {
      this->codeModule = codeModule;
      this->debugModule = debugModule;
   }
};

// --- Project ---

class Project
{
protected:
   bool            _hasWarning;
   int             _numberOfWarnings;
   int             _warningMasks;

   LibraryManager  _loader;
   ProjectSettings _settings;
   Sources         _sources;

   virtual ConfigCategoryIterator getCategory(_ConfigFile& config, ProjectSetting setting) = 0;
   virtual ident_t getOption(_ConfigFile& config, ProjectSetting setting) = 0;

   bool loadOption(_ConfigFile& config, ProjectSetting setting);
   void loadIntOption(_ConfigFile& config, ProjectSetting setting);
   void loadIntOption(_ConfigFile& config, ProjectSetting setting, int minValue, int maxValue);
   void loadHexOption(_ConfigFile& config, ProjectSetting setting);
   void loadAlignedIntOption(_ConfigFile& config, ProjectSetting setting, int alignment);
   void loadBoolOption(_ConfigFile& config, ProjectSetting setting);
   bool loadPathOption(_ConfigFile& config, ProjectSetting setting, path_t path);

   void loadCategory(_ConfigFile& config, ProjectSetting setting, path_t configPath);
   void loadSourceCategory(_ConfigFile& config, path_t configPath);
   void loadPrimitiveCategory(_ConfigFile& config, path_t configPath);
   void loadForwardCategory(_ConfigFile& config);

public:
   virtual int getDefaultEncoding() = 0;

   // project
   virtual int IntSetting(ProjectSetting key, int defaultValue = 0)
   {
      return _settings.get(key, defaultValue);
   }

   virtual ident_t StrSetting(ProjectSetting key)
   {
      return _settings.get(key, DEFAULT_STR);
   }

   virtual bool BoolSetting(ProjectSetting key)
   {
      return (_settings.get(key, 0) != 0);
   }

//   virtual bool testSetting(ProjectSetting key)
//   {
//      return _settings.exist(key);
//   }

   SourceIterator getSourceIt()
   {
      return _sources.start();
   }

   ForwardIterator getForwardIt()
   {
      return _settings.getIt(opForwards);
   }

   ident_t resolveExternalAlias(ident_t alias, bool& stdCall);

   virtual void printInfo(const char* msg, ident_t value) = 0;

////   virtual void raiseError(const char* msg) = 0;
   virtual void raiseError(ident_t msg, ident_t path, int row, int column, ident_t terminal = NULL) = 0;
   virtual void raiseError(ident_t msg, ident_t value) = 0;

   virtual void raiseErrorIf(bool throwExecption, ident_t msg, ident_t identifier) = 0;

   virtual void raiseWarning(ident_t msg, ident_t path, int row, int column, ident_t terminal = NULL) = 0;
   virtual void raiseWarning(ident_t msg, ident_t path) = 0;

////   virtual void loadForward(const wchar16_t* forward, const wchar16_t* reference);
   virtual void loadConfig(_ConfigFile& config, path_t configPath);

   virtual void initLoader()
   {
      // if library path is set we need to set the loader root as well
      if (!emptystr(StrSetting(opLibPath))) {
         Path libPath;
         Path::loadPath(libPath, StrSetting(opLibPath));

         _loader.setRootPath(libPath);
      }
         
      // if package is set we need to set the loader package as well
      Path outputPath;
      Path::loadPath(outputPath, StrSetting(opProjectPath));
      Path::combinePath(outputPath, StrSetting(opOutputPath));

      _loader.setPackage(StrSetting(opNamespace), outputPath);
   }

   virtual ident_t resolveForward(ident_t forward);

   // loader
   virtual _Module* loadModule(ident_t package, bool silentMode);

   virtual _Module* resolveModule(ident_t referenceName, ref_t& reference, bool silentMode = false);
   virtual _Module* resolveCore(ref_t reference, bool silentMode = false);

   bool HasWarnings() const { return _hasWarning; }

   virtual int getTabSize() { return 4; }

   int getWarningMask() const { return _warningMasks; }

   bool indicateWarning()
   {
      if (_warningMasks == 0)
         return false;

      _hasWarning = true;

      if (_numberOfWarnings > 0) {
         _numberOfWarnings--;
         return true;
      }
      else return false;
   }

   virtual _Module* createModule(ident_t name);
   virtual _Module* createDebugModule(ident_t name);

   virtual void saveModule(_Module* module, ident_t extension);

   Project();
   virtual ~Project() {}
};

} // _ELENA_

#endif // projectH
