//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA Library manager.
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef libmanH
#define libmanH 1

namespace _ELENA_
{

typedef _ELENA_::List<_JITLoaderListener*> LoaderListeners;

// --- LibraryManager ---

class LibraryManager : public _LibraryManager
{
   typedef Map<ident_t, char*> PathMap;

   Path              _rootPath;
   IdentifierString  _namespace;

   ModuleMap         _modules;         // Note : assumed to be linked list by resolveIndirectWeakModule
   ModuleMap         _debugModules;
   ModuleMap         _binaries;

   PathMap           _binaryPaths;
   PathMap           _packagePaths;

   LoaderListeners   _listeners;

   void onModuleLoad(_Module* module);

   _Module* resolve(bool debugModule, ident_t referenceName, LoadResult& result, ref_t& reference);

public:
   void setRootPath(path_t root)
   {
      _rootPath.copy(root);
   }

   ident_t getNamespace() const
   {
      return _namespace;
   }
   void setNamespace(ident_t package, path_t path)
   {
      _namespace.copy(package);
      _packagePaths.add(package, IdentifierString::clonePath(path));
   }
   void setNamespace(ident_t package)
   {
      _namespace.copy(package);
      _packagePaths.add(package, NULL);
   }

#ifdef _WIN32
   void addPackage(ident_t package, path_t path)
   {
      _packagePaths.add(package, IdentifierString::clonePath(path));
   }
#endif // _WIN32

   void addPackage(ident_t package, ident_t path)
   {
      _packagePaths.add(package, path.clone());
   }

   void nameToPath(ident_t moduleName, Path& path, ident_t extension)
   {
      Path ext(extension);

      PathMap::Iterator it = _packagePaths.start();
      while (!it.Eof()) {
         // if the module belongs to the current project
         if (NamespaceName::isIncluded(it.key(), moduleName)) {
            path.copy(*it);
            path.nameToPath(moduleName, ext.c_str());

            return;
         }
         it++;
      }

      // otherwise it is the global library module
      path.copy(_rootPath.c_str());
      path.nameToPath(moduleName, ext.c_str());
   }

   void addPrimitivePath(ident_t alias, path_t path)
   {
      _binaryPaths.erase(alias);
      _binaryPaths.add(alias, IdentifierString::clonePath(path));
   }
   ident_t resolvePrimitive(ident_t alias) const
   {
      return _binaryPaths.get(alias);
   }

   void addCorePath(path_t path)
   {
      _binaryPaths.add(NULL, IdentifierString::clonePath(path));
   }

   _Module* createModule(ident_t package, LoadResult& result);

   _Module* loadNative(ident_t package, LoadResult& result);
   _Module* loadModule(ident_t package, LoadResult& result, bool readOnly = true);
   _Module* loadDebugModule(ident_t package, LoadResult& result);

   bool loadCore(LoadResult& result);

   _Module* resolveNative(ident_t referenceName, LoadResult& result, ref_t& reference);
   _Module* resolveCore(ref_t reference, LoadResult& result);
   virtual _Module* resolveModule(ident_t referenceName, LoadResult& result, ref_t& reference);
   virtual _Module* resolveWeakModule(ident_t weakName, LoadResult& result, ref_t& reference);
   virtual _Module* resolveIndirectWeakModule(ident_t weakName, LoadResult& result, ref_t& reference);
   virtual _Module* resolveDebugModule(ident_t referenceName, LoadResult& result, ref_t& reference);

   void addListener(_JITLoaderListener* listener)
   {
      _listeners.add(listener);

      // notify the listener on already loaded modules
      ModuleMap::Iterator it = _modules.start();
      while (!it.Eof()) {
         onModuleLoad(*it);

         it++;
      }
   }

   LibraryManager();
   LibraryManager(path_t root, ident_t package);
   virtual ~LibraryManager() {}
};

} // _ELENA_

#endif // libmanH
