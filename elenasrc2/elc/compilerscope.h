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

   void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly);
   
   ref_t loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false);
   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false)
   {
      return loadClassInfo(info, module->resolveReference(reference), headerOnly);
   }

   virtual _Memory* mapSection(ref_t reference, bool existing)
   {
      //ref_t mask = reference & mskAnyRef;
   
      //ident_t referenceName = module->resolveReference(reference & ~mskAnyRef);
      /*if (isTemplateWeakReference(referenceName)) {
         return module->mapSection(module->mapReference(resolveWeakTemplateReference(referenceName)) | mask, existing);
      }
      else */return module->mapSection(reference, existing);
   }
   
   void raiseError(const char* message, ident_t sourcePath, SNode terminal);

   CompilerScope(_ProjectManager* project)
   {
      this->project = project;
   }
};

} // _ELENA_

#endif // compilerscopeH
