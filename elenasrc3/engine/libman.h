//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA JIT Loader.
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LIBMAN_H
#define LIBMAN_H

#include "elena.h"

namespace elena_lang
{
   // --- LibraryProvider ---
   class LibraryProvider : public LibraryLoaderBase, public LibraryProviderBase
   {
      typedef Map<ustr_t, path_t, allocUStr, freeUStr, freepath> PathMap;
      typedef List<LibraryLoaderListenerBase*> Listeners;

      PathString        _outputPath, _rootPath;

      IdentifierString  _namespace;

      PathMap           _binaryPaths, _packagePaths;
      ModuleMap         _binaries, _modules, _debugModules;
      Forwards          _templates;

      Listeners         _listeners;

      void nameToPath(ustr_t moduleName, PathString& path, ustr_t extension);

      bool loadCore(LoadResult& result);
      ModuleBase* loadModule(ustr_t name, LoadResult& result, bool readOnly);
      ModuleBase* loadDebugModule(ustr_t name, LoadResult& result);

      void onModuleLoad(ModuleBase* module);

      ustr_t resolveTemplateWeakReference(ustr_t referenceName, ForwardResolverBase* forwardResolver);

      ModuleBase* resolveModule(ustr_t referenceName, ref_t& reference, bool silentMode, bool debugModule);
      ModuleBase* resolveWeakModule(ustr_t referenceName, ref_t& reference, bool silentMode);

      void loadTemplateForwards(ModuleBase* module, ref_t reference);

   public:
      enum class ModuleRequestResult
      {
         NotFound,
         AlreadyLoaded,
         Loaded
      };

      ustr_t Namespace() override
      {
         return *_namespace;
      }

      path_t OutputPath() override
      {
         return *_outputPath;
      }

      void setOutputPath(path_t path)
      {
         _outputPath.copy(path);
      }
      void setRootPath(path_t path) override
      {
         _rootPath.copy(path);
      }
      void setNamespace(ustr_t ns)
      {
         _namespace.copy(ns);
      }

      void addCorePath(path_t path) override;
      void addPrimitivePath(ustr_t alias, path_t path) override;
      void addPackage(ustr_t ns, path_t path) override;

      void resolvePath(ustr_t ns, PathString& path) override;

      ModuleInfo getModule(ReferenceInfo referenceInfo, bool silentMode) override;
      ModuleInfo getDebugModule(ReferenceInfo referenceInfo, bool silentMode) override;

      ModuleInfo getWeakModule(ustr_t weakReferenceName, bool silentMode) override;

      SectionInfo getCoreSection(ref_t reference, bool silentMode) override;
      SectionInfo getSection(ReferenceInfo referenceInfo, ref_t mask, ref_t metaMask, bool silentMode) override;
      ClassSectionInfo getClassSections(ReferenceInfo referenceInfo, ref_t vmtMask, ref_t codeMask, 
         bool silentMode) override;

      ReferenceInfo retrieveReferenceInfo(ModuleBase* module, ref_t reference, ref_t mask,
         ForwardResolverBase* forwardResolver) override;
      ReferenceInfo retrieveReferenceInfo(ustr_t referenceName, 
         ForwardResolverBase* forwardResolver) override;

      void loadDistributedSymbols(ustr_t virtualSymbolName, ModuleInfoList& list) override;

      ModuleBase* createModule(ustr_t name);
      ModuleBase* createDebugModule(ustr_t name);

      bool saveModule(ModuleBase* module);
      bool saveDebugModule(ModuleBase* module);

      void retrievePath(ModuleBase* module, PathString& path);

      ModuleBase* loadModule(ustr_t name);

      ModuleRequestResult loadModuleIfRequired(ustr_t name);

      void addListener(LibraryLoaderListenerBase* listener)
      {
         _listeners.add(listener);

         // notify the listener on already loaded modules
         ModuleMap::Iterator it = _modules.start();
         while (!it.eof()) {
            onModuleLoad(*it);

            ++it;
         }
      }

      void loadDistributedSymbols(ModuleBase* module, ustr_t virtualSymbolName, ModuleInfoList& list);

      LibraryProvider();
   };
}

#endif
