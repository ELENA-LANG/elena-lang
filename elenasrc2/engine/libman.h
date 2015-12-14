//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA Library manager.
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef libmanH
#define libmanH 1

namespace _ELENA_
{

// --- LibraryManager ---

class LibraryManager : public _LibraryManager
{
   typedef Map<ident_t, ident_c*> PathMap;

   Path              _rootPath;
   IdentifierString  _namespace;

   ModuleMap         _modules;
   ModuleMap         _debugModules;
   ModuleMap         _binaries;

   PathMap           _binaryPaths;
   PathMap           _packagePaths;

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

   void addPackage(ident_t package, path_t path)
   {
      _packagePaths.add(package, IdentifierString::clonePath(path));
   }
   void addPackage(ident_t package, ident_t path)
   {
      _packagePaths.add(package, StringHelper::clone(path));
   }

   void nameToPath(ident_t moduleName, Path& path, ident_t extension)
   {
      Path ext;
      Path::loadPath(ext, extension);

      PathMap::Iterator it = _packagePaths.start();
      while (!it.Eof()) {
         // if the module belongs to the current project
         if (NamespaceName::isIncluded(it.key(), moduleName)) {
            Path::loadPath(path, *it);
            path.nameToPath(moduleName, ext);

            return;
         }
         it++;
      }

      // otherwise it is the global library module
      path.copy(_rootPath);
      path.nameToPath(moduleName, ext);
   }

   void addPrimitivePath(ident_t alias, path_t path)
   {
      _binaryPaths.erase(alias);
      _binaryPaths.add(alias, IdentifierString::clonePath(path));
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
   virtual _Module* resolveDebugModule(ident_t referenceName, LoadResult& result, ref_t& reference);

   LibraryManager();
   LibraryManager(path_t root, ident_t package);
   virtual ~LibraryManager() {}
};

} // _ELENA_

#endif // libmanH
