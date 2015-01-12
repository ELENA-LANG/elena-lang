//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA Library manager.
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef libmanH
#define libmanH 1

namespace _ELENA_
{

// --- LibraryManager ---

class LibraryManager : public _LibraryManager
{
   typedef Map<const wchar16_t*, tchar_t*> AliasMap;

   Path              _rootPath;
   IdentifierString  _package;
   Path              _packagePath;

   ModuleMap         _modules;
   ModuleMap         _debugModules;
   ModuleMap         _binaries;

   AliasMap          _binaryAliases;

public:
   const tchar_t* getRootPath() const { return _rootPath; }

   const wchar16_t* getPackage() const
   {
      return _package;
   }

   void setRootPath(const tchar_t* root)
   {
      _rootPath.copy(root);
   }
   void setPackage(const wchar16_t* package, const tchar_t* path)
   {
      _package.copy(package);
      _packagePath.copy(path);
   }
   void setPackage(const wchar16_t* package)
   {
      _package.copy(package);
   }

   void nameToPath(const wchar16_t* moduleName, Path& path, const tchar_t* extension)
   {
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

   void addPrimitiveAlias(const wchar16_t* alias, const tchar_t* path)
   {
      _binaryAliases.erase(alias);

      _binaryAliases.add(alias, StringHelper::clone(path));
   }

   void addCoreAlias(const tchar_t* path)
   {
      _binaryAliases.add(NULL, StringHelper::clone(path));
   }

   _Module* createModule(const wchar16_t* package, LoadResult& result);

   _Module* loadNative(const wchar16_t* package, LoadResult& result);
   _Module* loadModule(const wchar16_t* package, LoadResult& result, bool readOnly = true);
   _Module* loadDebugModule(const wchar16_t* package, LoadResult& result);

   bool loadCore(LoadResult& result);

   _Module* resolveNative(const wchar16_t* referenceName, LoadResult& result, ref_t& reference);
   _Module* resolveCore(ref_t reference, LoadResult& result);
   virtual _Module* resolveModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference);
   virtual _Module* resolveDebugModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference);

   LibraryManager();
   LibraryManager(const tchar_t* root, const wchar16_t* package);
   virtual ~LibraryManager() {}
};

} // _ELENA_

#endif // libmanH
