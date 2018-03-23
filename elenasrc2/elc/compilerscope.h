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

   void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly);
   
   ref_t loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false);
   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false)
   {
      return loadClassInfo(info, module->resolveReference(reference), headerOnly);
   }

   ref_t mapIdentifier(ident_t referenceName, bool existing = false);
   ref_t mapReference(ident_t referenceName, bool existing = false);

   /*virtual */ref_t mapTerminal(SNode terminal, bool existing = false);

   virtual _Memory* mapSection(ref_t reference, bool existing)
   {
      //ref_t mask = reference & mskAnyRef;
   
      //ident_t referenceName = module->resolveReference(reference & ~mskAnyRef);
      /*if (isTemplateWeakReference(referenceName)) {
         return module->mapSection(module->mapReference(resolveWeakTemplateReference(referenceName)) | mask, existing);
      }
      else */return module->mapSection(reference, existing);
   }
   
   /*virtual */ident_t resolveFullName(ref_t reference)
   {
      ident_t referenceName = module->resolveReference(reference & ~mskAnyRef);
//         if (isTemplateWeakReference(referenceName)) {
//            return project->resolveForward(referenceName);
//         }
      /*else */return referenceName;
   }
   
   _Module* resolveReference(ref_t reference, ref_t& moduleReference);

   void raiseError(const char* message, ident_t sourcePath, SNode terminal);
   void raiseWarning(int level, const char* message, ident_t sourcePath, SNode terminal);

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
