//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA LibraryManager.
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------
#include "libman.h"

#include <utility>

#include "module.h"
#include "langcommon.h"

using namespace elena_lang;

inline int getLoadError(LoadResult result)
{
   switch (result) {
      //case LoadResult::lrDuplicate:
      //   return errDuplicatedModule;
      case LoadResult::NotFound:
         return errUnknownModule;
      case LoadResult::WrongStructure:
         return errInvalidModule;
      case LoadResult::WrongVersion:
         return errInvalidModuleVersion;
      //case LoadResult::CannotCreate:
      //   return errCannotCreate;
      default:
         return errCannotCreate;
   }
}

// --- LibraryProvider ---

LibraryProvider :: LibraryProvider()
   : _binaryPaths(nullptr),
      _packagePaths(nullptr),
      _binaries(nullptr),
      _modules(nullptr),
      _debugModules(nullptr),
      _listeners(nullptr)
{
}

void LibraryProvider :: nameToPath(ustr_t moduleName, PathString& path, ustr_t extension)
{
   for (auto it = _packagePaths.start(); !it.eof(); ++it) {
      // if the module belongs to the package
      if (NamespaceString::isIncluded(it.key(), moduleName)) {
         path.copy(*it);
         ReferenceName::nameToPath(path, moduleName);
         path.appendExtension(extension);

         return;
      }
   }

   // otherwise it is the global library module
   path.copy(*_rootPath);
   ReferenceName::nameToPath(path, moduleName);
   path.appendExtension(extension);
}

bool LibraryProvider :: loadCore(LoadResult& result)
{
   for (auto it = _binaryPaths.start(); !it.eof(); ++it) {
      if (it.key().empty()) {
         FileReader reader(*it, FileRBMode, FileEncoding::Raw, false);

         LoadResult result = LoadResult::NotFound;
         auto module = new ROModule(reader, result);
         if (result != LoadResult::Successful) {
            delete module;

            return false;
         }
         else _binaries.addToTop(nullptr, module);
      }
   }
   return true;
}

void LibraryProvider :: resolvePath(ustr_t moduleName, PathString& path)
{
   nameToPath(moduleName, path, "nl");
}

ModuleBase* LibraryProvider :: loadModule(ustr_t name, LoadResult& result, bool readOnly)
{
   ModuleBase* module = _modules.get(name);
   if (module == nullptr) {
      PathString path;
      nameToPath(name, path, "nl");

      FileReader reader(*path, FileRBMode, FileEncoding::Raw, false);
      if (!readOnly) {
         module = new Module();
         result = static_cast<Module*>(module)->load(reader);
      }
      else module = new ROModule(reader, result);

      if (result != LoadResult::Successful) {
         delete module;

         return nullptr;
      }
      else _modules.add(name, module);

      onModuleLoad(module);
   }
   else result = LoadResult::Successful;

   return module;
}

ModuleBase* LibraryProvider :: loadDebugModule(ustr_t name, LoadResult& result)
{
   ModuleBase* module = _debugModules.get(name);
   if (!module) {
      PathString path;
      nameToPath(name, path, "dnl");

      FileReader reader(*path, FileRBMode, FileEncoding::Raw, false);
      module = new ROModule(reader, result);

      if (result != LoadResult::Successful) {
         delete module;

         return nullptr;
      }
      else _debugModules.add(name, module);
   }
   else result = LoadResult::Successful;

   return module;
}

void LibraryProvider :: onModuleLoad(ModuleBase* module)
{
   _listeners.forEach<ModuleBase*>(module, [](ModuleBase* module, LibraryLoaderListenerBase* loader)
   {
      loader->onLoad(module);
   });
}

ustr_t LibraryProvider :: resolveTemplateWeakReference(ustr_t referenceName, ForwardResolverBase* forwardResolver)
{
   ustr_t resolvedName = forwardResolver->resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
   if (resolvedName.empty()) {
      if (referenceName.endsWith(CLASSCLASS_POSTFIX)) {
         // HOTFIX : class class reference should be resolved simultaneously with class one
         IdentifierString classReferenceName(referenceName, referenceName.length() - getlength(CLASSCLASS_POSTFIX));

         classReferenceName.copy(resolveTemplateWeakReference(*classReferenceName, forwardResolver));
         classReferenceName.append(CLASSCLASS_POSTFIX);

         forwardResolver->addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, *classReferenceName);

         return forwardResolver->resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }

      // COMPILER MAGIC : try to find a template implementation
      auto resolved = getWeakModule(referenceName + TEMPLATE_PREFIX_NS_LEN, true);
      if (resolved.module != nullptr) {
         ustr_t resolvedReferenceName = resolved.module->resolveReference(resolved.reference);
         if (isWeakReference(resolvedReferenceName)) {
            IdentifierString fullName(resolved.module->name(), resolvedReferenceName);

            forwardResolver->addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, *fullName);
         }
         else forwardResolver->addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, resolvedReferenceName);

         referenceName = forwardResolver->resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }
      else throw JITUnresolvedException(referenceName);
   }
   else referenceName = resolvedName;

   return referenceName;
}

ModuleBase* LibraryProvider :: loadModule(ustr_t name)
{
   LoadResult result = LoadResult::NotFound;
   ModuleBase* module = loadModule(name, result, true);
   if (result == LoadResult::Successful) {
      return module;
   }
   else return nullptr;
}

ModuleBase* LibraryProvider :: resolveModule(ustr_t referenceName, ref_t& reference, bool silentMode, bool debugModule)
{
   if (referenceName.empty())
      return nullptr;

   if (NamespaceString::compareNs(referenceName, ROOT_MODULE)) {
      ReferenceName name;
      ReferenceName::copyProperName(name, referenceName);

      ReferenceName resolvedName(*_namespace, *name);

      return resolveModule(*resolvedName, reference, silentMode, debugModule);
   }

   NamespaceString ns(referenceName);
   ModuleBase* module = nullptr;
   while (module == nullptr && !ns.empty()) {
      LoadResult result = LoadResult::NotFound;
      module = debugModule ? loadDebugModule(*ns, result) : loadModule(*ns, result, true);
      if (result != LoadResult::Successful) {
         if (result == LoadResult::NotFound) {

         }
         else if (!silentMode) {
            throw InternalError(getLoadError(result));
         }
         else return nullptr;
      }

      reference = module ? module->mapReference(referenceName + module->name().length(), true) : 0;
      if (reference)
         return module;

      ns.trimLastSubNs();
   }

   return nullptr;
}

ModuleBase* LibraryProvider :: resolveWeakModule(ustr_t weakName, ref_t& reference, bool silentMode)
{
   IdentifierString fullName("'", weakName);

   for (auto it = _modules.start(); !it.eof(); ++it) {
      reference = (*it)->mapReference(*fullName, true);
      if (reference)
         return *it;
   }

   if (!silentMode)
      throw InternalError(getLoadError(LoadResult::NotFound));

   return nullptr;
}

ModuleBase* LibraryProvider :: resolveIndirectWeakModule(ustr_t weakName, ref_t& reference, bool silentMode)
{
   IdentifierString relativeName(TEMPLATE_PREFIX_NS, weakName);

   for (auto it = _modules.start(); !it.eof(); ++it) {
      // try to resolve it once again
      IdentifierString properName("'", weakName);

      reference = (*it)->mapReference(*properName, true);
      if (reference)
         return *it;

      // if not - load imported modules
      if ((*it)->mapReference(*relativeName, true)) {
         // get list of nested namespaces
         IdentifierString nsSectionName("'", NAMESPACES_SECTION);
         auto nsSection = (*it)->mapSection((*it)->mapReference(*nsSectionName, true) | mskLiteralListRef, true);
         if (nsSection) {
            MemoryReader nsReader(nsSection);
            while (!nsReader.eof()) {
               IdentifierString nsProperName("'");
               nsReader.appendString(nsProperName);
               nsProperName.append("'");
               nsProperName.append(weakName.str());

               reference = (*it)->mapReference(*nsProperName, true);
               if (reference)
                  return *it;
            }
         }

         // get list of imported modules
         IdentifierString importSectionName("'", IMPORTS_SECTION);
         auto importSection = (*it)->mapSection((*it)->mapReference(*importSectionName, true) | mskLiteralListRef, true);
         if (importSection) {
            MemoryReader importReader(importSection);
            while (!importReader.eof()) {
               IdentifierString moduleName;
               importReader.readString(moduleName);

               LoadResult tempResult;
               loadModule(*moduleName, tempResult, true);
            }
         }
      }
   }

   return nullptr;
}

void LibraryProvider :: addCorePath(path_t path)
{
   _binaryPaths.add(nullptr, path.clone());
}

void LibraryProvider :: addPrimitivePath(ustr_t alias, path_t path)
{
   _binaryPaths.add(alias, path.clone());
}

void LibraryProvider :: addPackage(ustr_t ns, path_t path)
{
   _packagePaths.add(ns, path.clone());
}

ModuleInfo LibraryProvider :: getModule(ReferenceInfo referenceInfo, bool silentMode)
{
   ModuleInfo info;

   if (referenceInfo.isRelative()) {
      info.module = referenceInfo.module;
      info.reference = referenceInfo.module->mapReference(referenceInfo.referenceName, true);
   }
   else info.module = resolveModule(referenceInfo.referenceName, info.reference, silentMode, false);

   return info;
}

ModuleInfo LibraryProvider :: getDebugModule(ReferenceInfo referenceInfo, bool silentMode)
{
   ModuleInfo info = {};
   info.module = resolveModule(referenceInfo.referenceName, info.reference, silentMode, true);

   return info;
}

ModuleInfo LibraryProvider :: getWeakModule(ustr_t weakReferenceName, bool silentMode)
{
   ModuleInfo retVal;

   retVal.module = resolveWeakModule(weakReferenceName, retVal.reference, true);
   if (retVal.module == nullptr) {
      // Bad luck : try to resolve it indirectly
      retVal.module = resolveIndirectWeakModule(weakReferenceName, retVal.reference, silentMode);
   }

   return retVal;
}

SectionInfo LibraryProvider :: getCoreSection(ref_t reference, bool silentMode)
{
   SectionInfo info;
   LoadResult result = LoadResult::NotFound;

   if (!_binaries.exist(nullptr)) {
      if (!loadCore(result)){
         if (!silentMode) {
            throw InternalError(errCommandSetAbsent);
         }
         return info;
      }
   }

   for(auto it = _binaries.start(); !it.eof(); ++it) {
      if (it.key().empty()) {
         MemoryBase* current = (*it)->mapSection(reference, true);
         if (current) {
            result = LoadResult::Successful;
            info.section = current;
            info.module = *it;

            break;
         }
      }
   }

   return info;
}

SectionInfo LibraryProvider :: getSection(ReferenceInfo referenceInfo, ref_t codeMask, ref_t metaMask, bool silentMode)
{
   SectionInfo info = {};

   ModuleInfo moduleInfo = getModule(referenceInfo, silentMode);
   if (moduleInfo.module && moduleInfo.reference) {
      info.module = moduleInfo.module;
      info.section = moduleInfo.module->mapSection(moduleInfo.reference | codeMask, true);
      if (metaMask)
         info.metaSection = moduleInfo.module->mapSection(moduleInfo.reference | metaMask, true);
      info.reference = moduleInfo.reference;
   }

   if (info.section == nullptr && !silentMode)
      throw JITUnresolvedException(referenceInfo);

   return info;
}

ClassSectionInfo LibraryProvider :: getClassSections(ReferenceInfo referenceInfo, ref_t vmtMask, ref_t codeMask, bool silentMode)
{
   ClassSectionInfo info;

   ModuleInfo moduleInfo = getModule(referenceInfo, silentMode);
   if (moduleInfo.module && moduleInfo.reference) {
      info.module = moduleInfo.module;
      info.vmtSection = moduleInfo.module->mapSection(moduleInfo.reference | vmtMask, true);
      info.codeSection = moduleInfo.module->mapSection(moduleInfo.reference | codeMask, true);
      info.reference = moduleInfo.reference;
   }

   if (info.vmtSection == nullptr && !silentMode)
      throw JITUnresolvedException(referenceInfo);

   return info;
}

ReferenceInfo LibraryProvider :: retrieveReferenceInfo(ModuleBase* module, ref_t reference, ref_t mask,
   ForwardResolverBase* forwardResolver)
{
   switch (mask) {
      case mskIntLiteralRef:
      case mskLongLiteralRef:
      case mskRealLiteralRef:
      case mskLiteralRef:
      case mskWideLiteralRef:
      case mskCharacterRef:
      case mskMssgLiteralRef:
         return module->resolveConstant(reference);
      default:
      {
         ustr_t referenceName = module->resolveReference(reference);
         while (isForwardReference(referenceName)) {
            ustr_t resolvedName = forwardResolver->resolveForward(referenceName + getlength(FORWARD_PREFIX_NS));
            if (!resolvedName.empty()) {
               referenceName = resolvedName;
            }
            else throw JITUnresolvedException(ReferenceInfo(referenceName));
         }

         if (NamespaceString::compareNs(referenceName, ROOT_MODULE)) {
            ReferenceName name;
            ReferenceName::copyProperName(name, referenceName);

            ReferenceName resolvedName(*_namespace, *name);

            auto info = retrieveReferenceInfo(*resolvedName, forwardResolver);
            // NOTE : if the reference was not resolved - reset the initial reference
            if (!info.module)
               info.referenceName = referenceName;

            return info;
         }

         if (isWeakReference(referenceName)) {
            if (isTemplateWeakReference(referenceName)) {
               referenceName = resolveTemplateWeakReference(referenceName, forwardResolver);
            }

            return ReferenceInfo(module, referenceName);
         }
         else return ReferenceInfo(referenceName);
      }
   }

}

ReferenceInfo LibraryProvider :: retrieveReferenceInfo(ustr_t referenceName, ForwardResolverBase* forwardResolver)
{
   while (isForwardReference(referenceName)) {
      ustr_t resolvedName = forwardResolver->resolveForward(referenceName + getlength(FORWARD_PREFIX_NS));
      if (!resolvedName.empty()) {
         referenceName = resolvedName;
      }
      else throw JITUnresolvedException(ReferenceInfo(referenceName));
   }

   if (isWeakReference(referenceName)) {
      if (isTemplateWeakReference(referenceName)) {
         referenceName = resolveTemplateWeakReference(referenceName, forwardResolver);
      }
   }

   ReferenceInfo referenceInfo;
   ref_t reference = 0;
   referenceInfo.module = resolveModule(referenceName, reference, true, false);
   if (referenceInfo.module) {
      referenceInfo.referenceName = referenceInfo.module->resolveReference(reference);

      return referenceInfo;
   }
   return ReferenceInfo(referenceName);
}

ModuleBase* LibraryProvider :: createModule(ustr_t name)
{
   auto module = new Module(name);

   _modules.add(name, module);

   return module;
}

ModuleBase* LibraryProvider :: createDebugModule(ustr_t name)
{
   auto module = new Module(name);

   _debugModules.add(name, module);

   return module;
}

bool LibraryProvider :: saveModule(ModuleBase* module)
{
   // resolving the module output path
   ustr_t name = module->name();
   PathString path;
   nameToPath(name, path, "nl");

   // re-creating path
   PathUtil::recreatePath(*path);

   // saving a module
   IdentifierString tmp(*path);
   printf("saving %s\n", tmp.str());

   FileWriter writer(*path, FileEncoding::Raw, false);
   return dynamic_cast<Module*>(module)->save(writer);
}

bool LibraryProvider::saveDebugModule(ModuleBase* module)
{
   // resolving the module output path
   ustr_t name = module->name();
   PathString path;
   nameToPath(name, path, "dnl");

   // re-creating path
   PathUtil::recreatePath(*path);

   // saving a module
   FileWriter writer(*path, FileEncoding::Raw, false);
   return dynamic_cast<Module*>(module)->save(writer);
}

void LibraryProvider :: loadDistributedSymbols(ustr_t virtualSymbolName, ModuleInfoList& list)
{
   for (auto it = _modules.start(); !it.eof(); ++it) {
      ref_t reference = (*it)->mapReference(virtualSymbolName, true);
      if (reference) {
         list.add({ *it, reference });
      }

      // get list of nested namespaces
      IdentifierString nsSectionName("'", NAMESPACES_SECTION);
      auto nsSection = (*it)->mapSection((*it)->mapReference(*nsSectionName, true) | mskLiteralListRef, true);
      if (nsSection) {
         MemoryReader nsReader(nsSection);
         while (!nsReader.eof()) {
            IdentifierString nsProperName("'");
            nsReader.appendString(nsProperName);
            nsProperName.append("'");
            nsProperName.append(virtualSymbolName);

            reference = (*it)->mapReference(*nsProperName, true);
            if (reference) {
               list.add({ *it, reference });
            }
         }
      }
   }
}
