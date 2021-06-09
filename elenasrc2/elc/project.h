//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains the declaration of the base class implementing
//      ELENA Project interface.
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef projectH
#define projectH 1

#include "libman.h"
#include "compilercommon.h"
#include "separser.h"

namespace _ELENA_
{

// --- Project list types ---
typedef Dictionary2D<int, ident_t>           ProjectSettings;
typedef Dictionary2D<ident_t, ident_t>       TargetSettings;
typedef Map<ident_t, ProjectSettings::VItem> FileMapping;

typedef _Iterator<ProjectSettings::VItem, _MapItem<ident_t, ProjectSettings::VItem>, ident_t> ForwardIterator;
typedef _Iterator<ProjectSettings::VItem, _MapItem<int, ProjectSettings::VItem>, int>         SourceIterator;
typedef _Iterator<TargetSettings::VItem, _MapItem<ident_t, TargetSettings::VItem>, ident_t>   TargetIterator;

// --- ELENA Project options ---
enum ProjectSetting
{
   opNone                  = 0x0000,

//   // compiler options
   opAppPath		         = 0x0001,
   opProjectPath           = 0x0002,
   opLibPath               = 0x0003,
   opNamespace             = 0x0004,
   opTarget                = 0x0006,
   opOutputPath            = 0x0008,
   opDebugMode             = 0x000A,
   opTemplate              = 0x000C,
   opThreadMax             = 0x0013,
   opClassSymbolAutoLoad   = 0x0015,
   //opTapeEntry             = 0x0016,
   opExtDispatchers        = 0x0017,

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
   opGCPERMSize            = 0x002C,
   opOpStackAlignment      = 0x002D,

   // compiler engine options
   opWarnOnWeakUnresolved  = 0x0042,
   opAutoSystemImport      = 0x0043,

   // compiler optimization options
   opL0                    = 0x0050,   // byte-code optimization
   opL1                    = 0x0051,   // source-code optimization

   opPrimitives            = 0x0060,
   opForwards              = 0x0061,
   opSources               = 0x0062,  
   opTemplates             = 0x0063,
   opExternals             = 0x0064,
   opWinAPI                = 0x0065,   // used only for WIN32
   opReferences            = 0x0066,
   opTargets               = 0x0067,   // compiler targets

   // compiler manfifest
   opManifestName          = 0x0070,
   opManifestVersion       = 0x0071,
   opManifestAuthor        = 0x0072,
};

// --- Project ---

class Project : public _ProjectManager
{
protected:
   bool            _hasWarning;
   int             _numberOfWarnings;
   int             _warningMasks;

   LibraryManager  _loader;

   ProjectSettings _settings;

   virtual bool readCategory(_ConfigFile& config, ProjectSetting setting, _ConfigFile::Nodes& list) = 0;
   virtual ident_t getOption(_ConfigFile& config, ProjectSetting setting) = 0;

   bool loadOption(_ConfigFile& config, ProjectSetting setting);
   void loadIntOption(_ConfigFile& config, ProjectSetting setting);
   void loadIntOption(_ConfigFile& config, ProjectSetting setting, int minValue, int maxValue);
   void loadHexOption(_ConfigFile& config, ProjectSetting setting);
   void loadAlignedIntOption(_ConfigFile& config, ProjectSetting setting, int alignment);
   void loadBoolOption(_ConfigFile& config, ProjectSetting setting);
   bool loadPathOption(_ConfigFile& config, ProjectSetting setting, path_t path);

   void loadCategory(_ConfigFile& config, ProjectSetting setting, path_t configPath);
   void loadSourceCategory(_ConfigFile& config);
   void loadPrimitiveCategory(_ConfigFile& config, path_t configPath);
   void loadForwardCategory(_ConfigFile& config);
   void loadTargetCategory(_ConfigFile& config);

public:
   // project
   virtual int IntSetting(ProjectSetting key, int defaultValue = 0)
   {
      return _settings.get(key, defaultValue);
   }

   virtual ident_t StrSetting(ProjectSetting key) const
   {
      return _settings.get(key, DEFAULT_STR);
   }

   virtual bool BoolSetting(ProjectSetting key) const
   {
      return (_settings.get(key, 0) != 0);
   }

//   virtual bool testSetting(ProjectSetting key)
//   {
//      return _settings.exist(key);
//   }

   ForwardIterator getForwardIt()
   {
      return _settings.getIt(opForwards);
   }

   ident_t resolvePrimitive(ident_t alias) const
   {
      return _loader.resolvePrimitive(alias);
   }

   virtual ident_t resolveExternalAlias(ident_t alias, bool& stdCall);

   virtual void addSource(path_t path, ident_t ns) = 0;
   virtual void addModule(_ConfigFile::Node moduleNode) = 0;
   virtual void addTarget(_ConfigFile::Node moduleNode) = 0;

   virtual void loadConfig(_ConfigFile& config, path_t configPath);

   void initLoader()
   {
      // if library path is set we need to set the loader root as well
      if (!emptystr(StrSetting(opLibPath))) {
         Path libPath(StrSetting(opLibPath));

         _loader.setRootPath(libPath.c_str());
      }

      // if package is set we need to set the loader package as well
      Path outputPath(StrSetting(opProjectPath), StrSetting(opOutputPath));

      _loader.setNamespace(StrSetting(opNamespace), outputPath.c_str());

      // add references to the additional libraries
      for (auto it = _settings.getIt(opReferences); !it.Eof(); it++) {
         _loader.addPackage(it.key(), *it);
      }
   }

   void addLoaderListener(_JITLoaderListener* listener)
   {
      _loader.addListener(listener);
   }

   virtual ident_t resolveForward(ident_t forward);
   virtual bool addForward(ident_t forward, ident_t reference);

   // loader
   virtual _Module* loadModule(ident_t package, bool silentMode);

   virtual _Module* resolveWeakModule(ident_t weakReferenceName, ref_t& reference, bool silentMode = false);
   virtual _Module* resolveModule(ident_t referenceName, ref_t& reference, bool silentMode = false);
   virtual _Module* resolveCore(ref_t reference, bool silentMode = false);

   virtual bool HasWarnings() const { return _hasWarning; }

//   virtual int getTabSize() { return 4; }

   virtual ident_t getManinfestName()
   {
      return StrSetting(opManifestName);
   }

   virtual ident_t getManinfestVersion()
   {
      return StrSetting(opManifestVersion);
   }

   virtual ident_t getManinfestAuthor()
   {
      return StrSetting(opManifestAuthor);
   }

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

   virtual ident_t Namespace() const
   {
      return StrSetting(opNamespace);
   }
//   virtual bool WarnOnWeakUnresolved() const
//   {
//      return BoolSetting(opWarnOnWeakUnresolved);
//   }
//
////   void compile(ident_t sourceFile, Compiler& compiler, ScriptParser parser, ModuleInfo& moduleInfo, Unresolveds& unresolved);

   Project();
   virtual ~Project() {}
};

} // _ELENA_

#endif // projectH
