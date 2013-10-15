//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA LibraryManager.
//
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------
#include "libman.h"
#include "module.h"

using namespace _ELENA_;

#define PMODULE_LEN getlength(PACKAGE_MODULE)

//inline const TCHAR* getName(const TCHAR* name, const TCHAR* package)
//{
//   if (name[0]=='$')
//      name++;
//
//   if (!emptystr(package) && compstr(name, pa/.)) {
//      name += getlength(package) + 1;
//   }
//   return name;
//}

// --- LibraryManager ---

LibraryManager :: LibraryManager()
   : _modules(NULL, freeobj), _binaries(NULL, freeobj), _binaryAliases(NULL, freestr)
{
}

LibraryManager :: LibraryManager(const _path_t* path, const wchar16_t* package)
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

_Module* LibraryManager :: loadPrimitive(const wchar16_t* package, LoadResult& result)
{   
   _Module* binary = _binaries.get(package);
   if (!binary) {
      const _path_t* path = _binaryAliases.get(package);

      if (path) {
         FileReader reader(path, feRaw, false);

         binary = new ROModule(reader, result);
         if(result!=lrSuccessful) {
            delete binary;

            return NULL;
         }
         _binaries.add(package, binary);
      }
      else result = lrNotFound;
   }
   else result = lrSuccessful;

   return binary;
}

_Module* LibraryManager :: resolvePrimitive(const wchar16_t* referenceName, LoadResult& result, ref_t& reference)
{
   NamespaceName package(referenceName + PMODULE_LEN + 1);

   _Module* binary = loadPrimitive(package, result);

   reference = binary ? binary->mapReference(referenceName, true) : 0;

   return binary;
}

_Module* LibraryManager :: resolveModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference)
{
   NamespaceName name(referenceName);

   _Module* module = loadModule(name, result);

   reference = module ? module->mapReference(referenceName, true) : 0;

   return module;
}
