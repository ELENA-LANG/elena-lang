//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Common implementation
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------


#include "pch.h"
// --------------------------------------------------------------------------
#include "tests_common.h"
#include "module.h"

using namespace elena_lang;

#ifdef _M_IX86

constexpr auto DEFAULT_STACKALIGNMENT = 1;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 4;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 16;

constexpr int MINIMAL_ARG_LIST = 1;

#elif _M_X64

constexpr auto DEFAULT_STACKALIGNMENT = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 32;

constexpr int MINIMAL_ARG_LIST = 2;

#endif

// --- TestModuleScope ---

TestModuleScope::TestModuleScope(bool tapeOptMode, bool threadFriendly)
   : ModuleScopeBase(new Module(), nullptr, DEFAULT_STACKALIGNMENT, DEFAULT_RAW_STACKALIGNMENT, 
      DEFAULT_EHTABLE_ENTRY_SIZE, MINIMAL_ARG_LIST, sizeof(uintptr_t), tapeOptMode, threadFriendly)
{

}

bool TestModuleScope :: isStandardOne()
{
   return false;
}

bool TestModuleScope :: withValidation()
{
   return false;
}

ref_t TestModuleScope :: mapAnonymous(ustr_t prefix)
{
   return 0;
}

ref_t TestModuleScope :: mapNewIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility)
{
   ReferenceName fullName(ns, identifier);

   return module->mapReference(*fullName);
}

ref_t TestModuleScope :: mapTemplateIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility, bool& alreadyDeclared,
   bool declarationMode)
{
   return 0;
}

ref_t TestModuleScope :: mapFullReference(ustr_t referenceName, bool existing)
{
   return 0;
}

ref_t TestModuleScope :: mapWeakReference(ustr_t referenceName, bool existing)
{
   return 0;
}

ExternalInfo TestModuleScope :: mapExternal(ustr_t dllAlias, ustr_t functionName)
{
   return {};
}

inline ref_t mapExistingIdentifier(ModuleBase* module, ustr_t identifier, Visibility visibility)
{
   ustr_t prefix = CompilerLogic::getVisibilityPrefix(visibility);

   IdentifierString name(prefix, identifier);

   return module->mapReference(*name, true);
}

ref_t TestModuleScope :: resolveImplicitIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility)
{
   if (!ns.empty()) {
      ReferenceName fullName(ns, identifier);

      return ::mapExistingIdentifier(module, *fullName, visibility);
   }
   else return ::mapExistingIdentifier(module, identifier, visibility);
}

ref_t TestModuleScope :: resolveImportedIdentifier(ustr_t identifier, IdentifierList* importedNs)
{
   return 0;
}

ref_t TestModuleScope :: resolveWeakTemplateReferenceID(ref_t reference)
{
   return 0;
}

SectionInfo TestModuleScope :: getSection(ustr_t referenceName, ref_t mask, bool silentMode)
{
   return {};
}

MemoryBase* TestModuleScope :: mapSection(ref_t reference, bool existing)
{
   return module->mapSection(reference, existing);
}

ModuleInfo TestModuleScope :: getModule(ustr_t referenceName, bool silentMode)
{
   if (isWeakReference(referenceName)) {
      return { module, module->mapReference(referenceName) };
   }
   return {};
}

ModuleInfo TestModuleScope :: getWeakModule(ustr_t referenceName, bool silentMode)
{
   return {};
}

ref_t TestModuleScope :: loadClassInfo(ClassInfo& info, ustr_t referenceName, bool headerOnly, bool fieldsOnly)
{
   ModuleInfo moduleInfo = { module, module->mapReference(referenceName)};

   return CompilerLogic::loadClassInfo(info, moduleInfo, module, headerOnly, fieldsOnly);
}

ref_t TestModuleScope :: loadSymbolInfo(SymbolInfo& info, ustr_t referenceName)
{
   return 0;
}

void TestModuleScope :: newNamespace(ustr_t name)
{
}

bool TestModuleScope :: includeNamespace(IdentifierList& importedNs, ustr_t name, bool& duplicateInclusion)
{
   return false;
}

bool TestModuleScope :: isDeclared(ref_t reference)
{
   return false;
}

Visibility TestModuleScope :: retrieveVisibility(ref_t reference)
{
   return Visibility::Private;
}

// --- CompilerEnvironment ---

CompilerEnvironment :: CompilerEnvironment()
{

}

ModuleScopeBase* CompilerEnvironment :: createModuleScope(bool tapeOptMode, bool threadFriendly)
{
   auto scope = new TestModuleScope(tapeOptMode, threadFriendly);

   // by default - the first reference is a super class
   scope->buildins.superReference = 1;

   return scope;
}

Compiler* CompilerEnvironment :: createCompiler()
{
   auto compiler = new Compiler(nullptr, nullptr, nullptr, CompilerLogic::getInstance());

   compiler->setNoValidation();

   return compiler;
}