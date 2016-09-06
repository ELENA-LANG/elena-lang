//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA Project interface.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------
#include "project.h"
#include "errors.h"
//#include "module.h"

using namespace _ELENA_;

#define NMODULE_LEN getlength(NATIVE_MODULE)

inline ident_t getLoadError(LoadResult result)
{
   switch(result)
   {
   case lrDuplicate:
      return errDuplicatedModule;
   case lrNotFound:
      return errUnknownModule;
   case lrWrongStructure:
      return errInvalidModule;
   case lrWrongVersion:
      return errInvalidModuleVersion;
   case lrCannotCreate:
      return errCannotCreate;
   default:
      return NULL;
   }
}

// --- Project ---

Project :: Project()
   : _sources(NULL, freestr)
{
   _hasWarning = false;
//   _numberOfWarnings = 100;
//   _warningMasks = WARNING_MASK_1;
}

bool Project :: loadOption(_ConfigFile& config, ProjectSetting setting)
{
   ident_t param = getOption(config, setting);
   if (!emptystr(param)) {
      _settings.add(setting, param.clone());

      return true;
   }
   else return false;
}

void Project :: loadIntOption(_ConfigFile& config, ProjectSetting setting)
{
   ident_t value = getOption(config, setting);
   if (value) {
      _settings.add(setting, value.toInt());
   }
}

void Project :: loadHexOption(_ConfigFile& config, ProjectSetting setting)
{
   ident_t value = getOption(config, setting);
   if (value) {
      _settings.add(setting, (int)value.toLong(16));
   }
}

void Project :: loadIntOption(_ConfigFile& config, ProjectSetting setting, int minValue, int maxValue)
{
   ident_t value = getOption(config, setting);
   if (value) {
      int intValue = value.toInt();
      if (minValue > intValue)
         intValue = minValue;
      else if (maxValue < intValue)
         intValue = maxValue;

      _settings.add(setting, intValue);
   }
}

void Project :: loadAlignedIntOption(_ConfigFile& config, ProjectSetting setting, int alignment)
{
   ident_t value = getOption(config, setting);
   if (value) {
      _settings.add(setting, align(value.toInt(), alignment));
   }
}

void Project :: loadBoolOption(_ConfigFile& config, ProjectSetting setting)
{
   ident_t value = getOption(config, setting);
   if (value) {
      if (value.toInt() != 0)
         _settings.add(setting, -1);
   }
}

bool Project :: loadPathOption(_ConfigFile& config, ProjectSetting setting, path_t rootPath)
{
   ident_t value = getOption(config, setting);
   if (value) {
      Path path(rootPath, value);

      _settings.add(setting, IdentifierString::clonePath(path));
      return true;
   }
   else return false;
}

void Project :: loadCategory(_ConfigFile& config, ProjectSetting setting, path_t path)
{
   IdentifierString key;

   ConfigCategoryIterator it = getCategory(config, setting);
   while (!it.Eof()) {
      // copy line key
      key.copy(it.key());

      // copy value or key if the value is absent
      ident_t value = *it;
      if (emptystr(value))
         value = it.key();

      // add path if provided
      if (!emptystr(path)) {
         Path filePath(path, value);

         _settings.add(setting, key, IdentifierString::clonePath(filePath));
      }
      else _settings.add(setting, key, value.clone());

      it++;
   }
}

void Project :: loadForwardCategory(_ConfigFile& config)
{
   ConfigCategoryIterator it = getCategory(config, opForwards);
   while (!it.Eof()) {
      _settings.add(opForwards, it.key(), ((ident_t)*it).clone());

      it++;
   }
}

//void Project :: loadPrimitiveCategory(_ConfigFile& config, path_t path)
//{
//   ConfigCategoryIterator it = getCategory(config, opPrimitives);
//   while (!it.Eof()) {
//      // copy value or key if the value is absent
//      ident_t value = *it;
//      if (emptystr(value))
//         value = it.key();
//
//      // add path if provided
//      Path filePath(path);
//      // if path starts with tilda - skip path
//      if (value[0] == '~') {         
//         Path::loadPath(filePath, value + 1);
//      }
//      else Path::combinePath(filePath, value);
//
//      if (StringHelper::compare(it.key(), CORE_ALIAS)) {
//         _loader.addCorePath(filePath);
//      }
//      else _loader.addPrimitivePath(it.key(), filePath);
//
//      it++;
//   }
//}

void Project :: loadSourceCategory(_ConfigFile& config, path_t path)
{
   ConfigCategoryIterator it = getCategory(config, opSources);
   while (!it.Eof()) {
      // add path if provided
      Path filePath(path, it.key());

      _sources.add(it.key(), IdentifierString::clonePath(filePath));

      it++;
   }
}

void Project :: loadConfig(_ConfigFile& config, path_t configPath)
{
   // load project settings (if setting is absent previous value is used)
   loadOption(config, opNamespace);
   loadPathOption(config, opLibPath, configPath);
   loadPathOption(config, opTarget, configPath);
   loadOption(config, opOutputPath);
//   loadBoolOption(config, opWarnOnUnresolved);
//   //loadBoolOption(config, opWarnOnSignature);
   loadBoolOption(config, opDebugMode);
   loadBoolOption(config, opDebugSubjectInfo);
   loadBoolOption(config, opClassSymbolAutoLoad);
//   loadOption(config, opTemplate);

   // load compiler settings
   loadIntOption(config, opThreadMax, 0, 60);

   // load linker settings
   loadHexOption(config, opGCMGSize);
   loadHexOption(config, opGCYGSize);
   loadIntOption(config, opPlatform);

   loadIntOption(config, opSizeOfStackReserv);
   loadIntOption(config, opSizeOfStackCommit);
   loadIntOption(config, opSizeOfHeapReserv);
   loadIntOption(config, opSizeOfHeapCommit);
   loadAlignedIntOption(config, opImageBase, 0x400000);

//   // load compiler engine options
//   loadIntOption(config, opL0);
//
//   // load primitive aliases
//   loadPrimitiveCategory(config, configPath);
//
////   // load external aliases
////   loadCategory(config, opExternals, NULL);
////   loadCategory(config, opWinAPI, NULL);
   loadCategory(config, opReferences, configPath);

   // load sources
   loadSourceCategory(config, configPath);

   // load forwards
   loadForwardCategory(config);

//   // load manifest info
//   loadOption(config, opManifestName);
//   loadOption(config, opManifestVersion);
//   loadOption(config, opManifestAuthor);
}

//////void Project :: loadForward(const wchar16_t* forward, const wchar16_t* reference)
//////{
//////   ReferenceNs fwd(forward);
//////
//////   _settings.add(opForwards, fwd, StringHelper::clone(reference));
//////}
////
////_Module* Project :: loadModule(ident_t package, bool silentMode)
////{
////   LoadResult result = lrNotFound;
////   _Module* module = _loader.loadModule(package, result);
////   if (result != lrSuccessful) {
////      if (!silentMode) {
////         raiseError(getLoadError(result), package);
////      }
////
////      return NULL;
////   }
////   else return module;
////}
//
//_Module* Project::createModule(ident_t name)
//{
//   LoadResult result = lrNotFound;
//   _Module* module = _loader.createModule(name, result);
//
//   if (result != lrSuccessful && result != lrDuplicate) {
//      raiseError(getLoadError(lrCannotCreate), name);
//
//      return NULL;
//   }
//   else return module;
//}
//
//_Module* Project :: createDebugModule(ident_t name)
//{
//   return new Module(name);
//}
//
//void Project :: saveModule(_Module* module, ident_t extension)
//{
//   ident_t name = module->Name();
//   Path path;
//   _loader.nameToPath(name, path, extension);
//
//   Path outputPath;
//   Path::loadPath(outputPath, StrSetting(opProjectPath));
//   Path::combinePath(outputPath, StrSetting(opOutputPath));
//
//   Path::create(outputPath, path);
//
//   FileWriter writer(path, feRaw, false);
//   if(!module->save(writer))
//      raiseError(getLoadError(lrCannotCreate), IdentifierString(path));
//}

ident_t Project :: resolveForward(ident_t forward)
{
   return _settings.get(opForwards, forward, DEFAULT_STR);
}

_Module* Project :: resolveModule(ident_t referenceName, ref_t& reference, bool silentMode)
{
   while (isWeakReference(referenceName)) {
      referenceName = resolveForward(referenceName);
   }

   if (emptystr(referenceName))
      return NULL;

   LoadResult result = lrNotFound;
   _Module* module = NULL;
   if (referenceName.compare(NATIVE_MODULE, NMODULE_LEN) && referenceName[NMODULE_LEN]=='\'') {
      module = _loader.resolveNative(referenceName, result, reference);
   }
   else module = _loader.resolveModule(referenceName, result, reference);

   if (result != lrSuccessful) {
      if (!silentMode)
         raiseError(getLoadError(result), referenceName);

      return NULL;
   }
   else return module;
}

_Module* Project :: resolveCore(ref_t reference, bool silentMode)
{
   LoadResult result = lrNotFound;
   _Module* module = _loader.resolveCore(reference, result);

   if (result != lrSuccessful) {
      if (!silentMode)
         raiseError(getLoadError(result), CORE_ALIAS);

      return NULL;
   }
   else return module;
}

//ident_t Project::resolveExternalAlias(ident_t alias, bool& stdCall)
//{
//   ident_t dll = _settings.get(opWinAPI, alias, DEFAULT_STR);
//   if (!emptystr(dll)) {
//      stdCall = true;
//
//      return dll;
//   }
//   else return _settings.get(opExternals, alias, DEFAULT_STR);
//}
