//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler scope class.
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerscopeH
#define compilerscopeH

#include "syntaxtree.h"
#include "compilercommon.h"

namespace _ELENA_
{
   
struct CompilerScope : _CompilerScope
{
   _ProjectManager*  project;

   // warning mapiing
//      bool warnOnWeakUnresolved;
   int  warningMask;

   SymbolMap savedPaths;

   virtual _Module* loadReferenceModule(ident_t referenceName, ref_t& reference);
   virtual _Module* loadReferenceModule(ref_t reference, ref_t& moduleReference)
   {
      return loadReferenceModule(module->resolveReference(reference), moduleReference);
   }

   void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly);
   
   ref_t loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false);
   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false)
   {
      return loadClassInfo(info, module->resolveReference(reference), headerOnly);
   }

   //   ref_t mapIdentifier(ident_t referenceName, bool existing = false);
   ref_t mapFullReference(ident_t referenceName, bool existing = false);
   ///*virtual */ref_t mapNewTerminal(SNode terminal, bool privateOne);
   /*virtual */ref_t mapTemplateClass(ident_t ns, ident_t templateName, bool& alreadyDeclared);
   ref_t mapNewIdentifier(ident_t ns, ident_t identifier, bool privateOne);

   virtual _Memory* mapSection(ref_t reference, bool existing)
   {
      ref_t mask = reference & mskAnyRef;
   
      ident_t referenceName = module->resolveReference(reference & ~mskAnyRef);
      if (isTemplateWeakReference(referenceName)) {
         return module->mapSection(module->mapReference(resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN)) | mask, existing);
      }
      else return module->mapSection(reference, existing);
   }
   
   ident_t resolveWeakTemplateReference(ident_t referenceName);

   ref_t resolveImplicitIdentifier(ident_t ns, ident_t identifier, bool referenceOne, IdentifierList& importedNs);

   /*virtual */ident_t resolveFullName(ref_t reference)
   {
      ident_t referenceName = module->resolveReference(reference & ~mskAnyRef);
      if (isTemplateWeakReference(referenceName)) {
         return project->resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }
      else return referenceName;
   }   
   ident_t resolveFullName(ident_t referenceName)
   {
      if (isTemplateWeakReference(referenceName)) {
         return project->resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }
      else return referenceName;
   }

   /*virtual */void saveAttribute(ident_t typeName, ref_t classReference);

   virtual void raiseError(const char* message, ident_t sourcePath, SNode terminal);
   virtual void raiseWarning(int level, const char* message, ident_t sourcePath, SNode terminal);

   CompilerScope(_ProjectManager* project)
      : savedPaths(-1)
   {
      this->project = project;

   //   warnOnWeakUnresolved = project->WarnOnWeakUnresolved();
      warningMask = project->getWarningMask();
   }
};

} // _ELENA_

#endif // compilerscopeH
