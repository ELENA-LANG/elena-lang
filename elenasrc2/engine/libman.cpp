//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA LibraryManager.
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------
#include "libman.h"
#include "module.h"

using namespace _ELENA_;

#define NMODULE_LEN getlength(NATIVE_MODULE)

// --- LibraryManager ---

LibraryManager :: LibraryManager()
   : _modules(nullptr, freeobj), _binaries(NULL, freeobj),
   _binaryPaths(NULL, freestr), _packagePaths(nullptr, freestr), _debugModules(NULL, freeobj)
{
}

LibraryManager :: LibraryManager(path_t root, ident_t package)
   : _rootPath(root), _namespace(package), _modules(NULL, freeobj), _binaries(NULL, freeobj),
   _binaryPaths(NULL, freestr), _packagePaths(NULL, freestr), _debugModules(NULL, freeobj)
{
}

void LibraryManager :: onModuleLoad(_Module* module)
{
   LoaderListeners::Iterator it = _listeners.start();
   while (!it.Eof()) {
      (*it)->onModuleLoad(module);

      it++;
   }
}

_Module* LibraryManager :: createModule(ident_t package, LoadResult& result)
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

_Module* LibraryManager :: loadModule(ident_t package, LoadResult& result, bool readOnly)
{
   _Module* module = _modules.get(package);
   if (!module) {
      Path path;
      nameToPath(package, path, "nl");

      FileReader  reader(path.c_str(), feRaw, false);
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

      onModuleLoad(module);
   }
   else result = lrSuccessful;

   return module;
}

_Module* LibraryManager :: loadDebugModule(ident_t package, LoadResult& result)
{
   _Module* module = _debugModules.get(package);
   if (!module) {
      Path path;
      nameToPath(package, path, "dnl");

      FileReader  reader(path.c_str(), feRaw, false);
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

_Module* LibraryManager :: loadNative(ident_t package, LoadResult& result)
{
   _Module* binary = _binaries.get(package);
   if (!binary) {
      ident_t path = _binaryPaths.get(package);
      if (emptystr(path)) {
         result = lrNotFound;

         return NULL;
      }

      Path filePath(path);
      FileReader reader(filePath.c_str(), feRaw, false);

      binary = new ROModule(reader, result);
      if (result != lrSuccessful) {
         delete binary;

         return NULL;
      }
      else _binaries.add(package, binary);
   }
   else result = lrSuccessful;

   return binary;
}

bool LibraryManager :: loadCore(LoadResult& result)
{
   PathMap::Iterator it = _binaryPaths.start();
   while (!it.Eof()) {
      if (emptystr(it.key())) {
         Path path(*it);

         FileReader reader(path.str(), feRaw, false);

         _Module* binary = new ROModule(reader, result);
         if(result != lrSuccessful) {
            delete binary;

            return false;

         }
         else _binaries.addToTop(NULL, binary);
      }
      it++;
   }
   return true;
}

_Module* LibraryManager :: resolveCore(ref_t reference, LoadResult& result)
{
   result = lrNotFound;

   // load modules if it is first time usage
   if (!_binaries.exist(NULL)) {
      if (!loadCore(result))
         return NULL;
   }

   ModuleMap::Iterator it = _binaries.start();
   while (!it.Eof()) {
      if (emptystr(it.key())) {
         _Memory* current = (*it)->mapSection(reference, true);
         if (current) {
            result = lrSuccessful;
            return *it;
         }
         else it++;
      }
      else break;
   }

   return NULL;
}

_Module* LibraryManager :: resolveNative(ident_t referenceName, LoadResult& result, ref_t& reference)
{
   NamespaceName native(referenceName + NMODULE_LEN + 1);

   _Module* module = loadNative(native, result);

   reference = module ? module->mapReference(referenceName, true) : 0;

   return module;
}

_Module* LibraryManager :: resolveWeakModule(ident_t weakName, LoadResult& result, ref_t& reference)
{
   IdentifierString fullName("'", weakName);

   for (auto it = _modules.start(); !it.Eof(); it++) {
      reference = (*it)->mapReference(fullName, true);
      if (reference) {
         result = lrSuccessful;

         return *it;
      }
   }

   return nullptr;
}

_Module* LibraryManager :: resolveIndirectWeakModule(ident_t weakName, LoadResult& result, ref_t& reference)
{
   IdentifierString relativeName(TEMPLATE_PREFIX_NS, weakName);

   for (auto it = _modules.start(); !it.Eof(); it++) {
      // try to resolve it once again
      IdentifierString properName("'");
      properName.append(weakName.c_str());

      reference = (*it)->mapReference(properName, true);
      if (reference) {
         result = lrSuccessful;

         return *it;
      }

      // if not - load imported modules
      if ((*it)->mapReference(relativeName.c_str(), true)) {
         // get list of nested namespaces
         IdentifierString nsSectionName("'", NAMESPACES_SECTION);
         _Memory* nsSection = (*it)->mapSection((*it)->mapReference(nsSectionName, true) | mskMetaRDataRef, true);
         if (nsSection) {
            MemoryReader nsMetaReader(nsSection);
            while (!nsMetaReader.Eof()) {
               IdentifierString nsProperName("'", nsMetaReader.getLiteral(DEFAULT_STR));
               nsProperName.append("'");
               nsProperName.append(weakName.c_str());

               reference = (*it)->mapReference(nsProperName, true);
               if (reference) {
                  result = lrSuccessful;

                  return *it;
               }
            }
         }

         // get list of imported modules
         IdentifierString sectionName("'", IMPORTS_SECTION);

         _Memory* section = (*it)->mapSection((*it)->mapReference(sectionName, true) | mskMetaRDataRef, true);
         if (section) {
            MemoryReader metaReader(section);
            while (!metaReader.Eof()) {
               ident_t moduleName = metaReader.getLiteral(DEFAULT_STR);

               LoadResult tempResult;
               loadModule(moduleName, tempResult);
            }
         }
      }
   }

   return nullptr;
}

_Module* LibraryManager :: resolve(bool debugModule, ident_t referenceName, LoadResult& result, ref_t& reference)
{
   NamespaceName name(referenceName);
   _Module* module = NULL;
   while (!module && !emptystr(name)) {
      if (debugModule) {
         module = loadDebugModule(name, result);
      }
      else module = loadModule(name, result);

      reference = module ? module->mapReference(referenceName.c_str() + getlength(module->Name()), true) : 0;
      if (reference)
         return module;

      name.trimLastSubNs();
   }

   return nullptr;
}

_Module* LibraryManager :: resolveModule(ident_t referenceName, LoadResult& result, ref_t& reference)
{
   if (NamespaceName::compare(referenceName, NAMESPACE_KEY)) {
      ReferenceName name(referenceName);
      ReferenceNs resolvedName(_namespace, name);

      return resolveModule(resolvedName, result, reference);
   }
   else return resolve(false, referenceName, result, reference);
}

_Module* LibraryManager :: resolveDebugModule(ident_t referenceName, LoadResult& result, ref_t& reference)
{
   return resolve(true, referenceName, result, reference);
}
