//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA JIT Loader.
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LIBMAN_H
#define LIBMAN_H

#include "elena.h"

namespace elena_lang
{
   // --- LibraryProvider ---
   class LibraryProvider : public LibraryLoaderBase
   {
      typedef Map<ustr_t, path_t, allocUStr, freeUStr, freepath> PathMap;
      typedef List<LibraryLoaderListenerBase*> Listeners;

      PathString        _outputPath, _rootPath;

      IdentifierString  _namespace;

      PathMap           _binaryPaths, _packagePaths;
      ModuleMap         _binaries, _modules;

      Listeners         _listeners;

      void nameToPath(ustr_t moduleName, PathString& path);

      bool loadCore(LoadResult& result);
      ModuleBase* loadModule(ustr_t name, LoadResult& result, bool readOnly);

      void onModuleLoad(ModuleBase* module);

      ustr_t resolveTemplateWeakReference(ustr_t referenceName, ForwardResolverBase* forwardResolver);

      ModuleBase* resolveModule(ustr_t referenceName, ref_t& reference, bool silentMode);
      ModuleBase* resolveWeakModule(ustr_t referenceName, ref_t& reference, bool silentMode);
      ModuleBase* resolveIndirectWeakModule(ustr_t referenceName, ref_t& reference, bool silentMode);

   public:
      void setOutputPath(path_t path)
      {
         _outputPath.copy(path);
      }
      void setRootPath(path_t path)
      {
         _rootPath.copy(path);
      }
      void setNamespace(ustr_t ns)
      {
         _namespace.copy(ns);
      }

      void addCorePath(path_t path);
      void addPrimitivePath(ustr_t alias, path_t path);
      void addPackage(ustr_t ns, path_t path);

      ModuleInfo getModule(ReferenceInfo referenceInfo, bool silentMode) override;

      ModuleInfo getWeakModule(ustr_t weakReferenceName, bool silentMode) override;

      SectionInfo getCoreSection(ref_t reference, bool silentMode) override;
      SectionInfo getSection(ReferenceInfo referenceInfo, ref_t mask, bool silentMode) override;
      ClassSectionInfo getClassSections(ReferenceInfo referenceInfo, ref_t vmtMask, ref_t codeMask, 
         bool silentMode) override;

      ReferenceInfo retrieveReferenceInfo(ModuleBase* module, ref_t reference, 
         ForwardResolverBase* forwardResolver) override;
      ReferenceInfo retrieveReferenceInfo(ustr_t referenceName, 
         ForwardResolverBase* forwardResolver) override;

      ModuleBase* createModule(ustr_t name);
      ModuleBase* createDebugModule(ustr_t name);

      bool saveModule(ModuleBase* module);
      bool saveDebugModule(ModuleBase* module);

      LibraryProvider();
   };
}

#endif
