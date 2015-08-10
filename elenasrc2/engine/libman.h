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
   typedef Map<ident_t, ident_c*> AliasMap;

   Path              _rootPath;
   IdentifierString  _package;
   Path              _packagePath;

   ModuleMap         _modules;
   ModuleMap         _debugModules;
   ModuleMap         _binaries;

   AliasMap          _binaryAliases;

public:
//   const tchar_t* getRootPath() const { return _rootPath; }
//
   ident_t getPackage() const
   {
      return _package;
   }

   void setRootPath(path_t root)
   {
      _rootPath.copy(root);
   }
   void setPackage(ident_t package, path_t path)
   {
      _package.copy(package);
      _packagePath.copy(path);
   }
   void setPackage(ident_t package)
   {
      _package.copy(package);
   }

   void nameToPath(ident_t moduleName, Path& path, ident_t extension)
   {
      Path ext;
      Path::loadPath(ext, extension);

      // if the module belongs to the current project package
      if (NamespaceName::isIncluded(_package, moduleName))
      {
         path.copy(_packagePath);
         path.nameToPath(moduleName, ext);
      }
      // if it is the library module
      else {
         path.copy(_rootPath);
         path.nameToPath(moduleName, ext);
      }
   }

   void addPrimitiveAlias(ident_t alias, path_t path)
   {
      _binaryAliases.erase(alias);
      _binaryAliases.add(alias, IdentifierString::clonePath(path));
   }

   void addCoreAlias(path_t path)
   {
      _binaryAliases.add(NULL, IdentifierString::clonePath(path));
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
