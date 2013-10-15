//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA Library manager.
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef libmanH
#define libmanH 1

namespace _ELENA_
{

// --- LibraryManager ---

class LibraryManager
{
   typedef Map<const wchar16_t*, _path_t*> AliasMap;

   Path              _rootPath;
   IdentifierString  _package;
   Path              _packagePath;

   ModuleMap         _modules;
   ModuleMap         _binaries;

   AliasMap          _binaryAliases;

public:
   const _path_t* getRootPath() const { return _rootPath; }

   void setRootPath(const _path_t* root)
   {
      _rootPath.copy(root);
   }
   void setPackage(const wchar16_t* package, const _path_t* path)
   {
      _package.copy(package);
      _packagePath.copy(path);
   }
   void setPackage(const wchar16_t* package)
   {
      _package.copy(package);
   }

   void nameToPath(const wchar16_t* moduleName, Path& path, const _path_t* extension)
   {
//      bool isStandard = ConstantIdentifier::compare(moduleName, STANDARD_MODULE);

      // if the module belongs to the current project package
      if (StringHelper::compare(moduleName, _package))
      {
         path.copy(_packagePath);
         path.nameToPath(moduleName, extension);
      }
      // if it is the library module
      else {
         path.copy(_rootPath);
         path.nameToPath(moduleName, extension);
      }
   }

   const _path_t* getPrimitiveAlias(const wchar16_t* alias)
   {
      return _binaryAliases.get(alias);
   }

   void addPrimitiveAlias(const wchar16_t* alias, const _path_t* path)
   {
      _binaryAliases.erase(alias);
      _binaryAliases.add(alias, StringHelper::clone(path));
   }

   _Module* createModule(const wchar16_t* package, LoadResult& result);

   _Module* loadModule(const wchar16_t* package, LoadResult& result, bool readOnly = true);
   _Module* loadPrimitive(const wchar16_t* package, LoadResult& result);

   _Module* resolvePrimitive(const wchar16_t* referenceName, LoadResult& result, ref_t& reference);
   _Module* resolveModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference);

   LibraryManager();
   LibraryManager(const _path_t* root, const wchar16_t* package);
   virtual ~LibraryManager() {}
};

} // _ELENA_

#endif // libmanH
