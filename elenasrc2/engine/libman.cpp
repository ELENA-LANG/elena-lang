//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA LibraryManager.
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------
#include "libman.h"
#include "module.h"

using namespace _ELENA_;

#define PMODULE_LEN getlength(PACKAGE_MODULE)

// --- LibraryManager ---

LibraryManager :: LibraryManager()
   : _modules(NULL, freeobj), _binaries(NULL, freeobj), _binaryAliases(NULL, freestr), _debugModules(NULL, freeobj)
{
}

LibraryManager :: LibraryManager(const tchar_t* path, const wchar16_t* package)
   : _rootPath(path), _package(package), _modules(NULL, freeobj), _binaries(NULL, freeobj), _binaryAliases(NULL, freestr)
{
}

_Module* LibraryManager :: createModule(const wchar16_t* package, LoadResult& result)
{
   _Module* module = _modules.get(package);
   if (module) {
      result = lrDuplicate;
   }
   else {
      module = new Module(package);
      result = lrSuccessful;

      _modules.add(package, module);
   }

   return module;
}

_Module* LibraryManager :: loadModule(const wchar16_t* package, LoadResult& result, bool readOnly)
{
   _Module* module = _modules.get(package);
   if (!module) {
      Path path;
      nameToPath(package, path, _T("nl"));

      FileReader  reader(path, feRaw, false);
      if (!readOnly) {
         module = new Module();
         result = ((Module*)module)->load(reader);
      }
      else module = new ROModule(reader, result);

      if (result != lrSuccessful) {
         delete module;

         return NULL;
      }
      else _modules.add(package, module);
   }
   else result = lrSuccessful;

   return module;
}

_Module* LibraryManager :: loadDebugModule(const wchar16_t* package, LoadResult& result)
{
   _Module* module = _debugModules.get(package);
   if (!module) {
      Path path;
      nameToPath(package, path, _T("dnl"));

      FileReader  reader(path, feRaw, false);
      module = new ROModule(reader, result);

      if (result != lrSuccessful) {
         delete module;

         return NULL;
      }
      else _debugModules.add(package, module);
   }
   else result = lrSuccessful;

   return module;
}

bool LibraryManager :: loadPrimitive(const wchar16_t* package, LoadResult& result)
{
   result = lrNotFound;

   AliasMap::Iterator it = _binaryAliases.getIt(package);
   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), package)) {
         FileReader reader(*it, feRaw, false);

         _Module* binary = new ROModule(reader, result);
         _binaries.add(package, binary);

         if(result!=lrSuccessful) {
            return false;
         }
      }

      it++;
   }
               
   return result == lrSuccessful;
}

_Module* LibraryManager :: resolvePrimitive(const wchar16_t* referenceName, LoadResult& result, ref_t& reference)
{
   result = lrNotFound;

   NamespaceName package(referenceName + PMODULE_LEN + 1);
   
   ModuleMap::Iterator it = _binaries.getIt(package);
   // load modules if it is first time usage
   if (it.Eof()) {
      if(!loadPrimitive(package, result)) {
         reference = 0;

         return NULL;
      }

      it = _binaries.getIt(package);
   }

   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), package)) {
         ref_t currentRef = (*it)->mapReference(referenceName, true);
         if (currentRef) {
            reference = currentRef;
            result = lrSuccessful;

            return *it;
         }
      }

      it++;
   }

   return NULL;
}

_Module* LibraryManager :: resolvePredefined(const wchar16_t* package, ref_t reference, LoadResult& result)
{
   result = lrNotFound;

   ModuleMap::Iterator it = _binaries.getIt(package);
   // load modules if it is first time usage
   if (it.Eof()) {
      if(!loadPrimitive(package, result))
         return NULL;

      it = _binaries.getIt(package);
   }

   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), package)) {
         _Memory* current = (*it)->mapSection(reference, true);
         if (current) {
            result = lrSuccessful;
            return *it;
         }
      }
      it++;
   }

   return NULL;
}

_Module* LibraryManager :: resolveModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference)
{
   NamespaceName name(referenceName);

   _Module* module = loadModule(name, result);

   reference = module ? module->mapReference(referenceName, true) : 0;

   return module;
}

_Module* LibraryManager :: resolveDebugModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference)
{
   NamespaceName name(referenceName);

   _Module* module = loadDebugModule(name, result);

   reference = module ? module->mapReference(referenceName, true) : 0;

   return module;
}