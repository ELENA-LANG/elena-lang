//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef COMPILERLOGIC_H
#define COMPILERLOGIC_H

#include "buildtree.h"
#include "clicommon.h"

namespace elena_lang
{
   // --- CompilerLogic ---
   class CompilerLogic
   {
   public:
      BuildKey resolveOp(int operatorId, ref_t* arguments, size_t length);

      bool defineClassInfo(ModuleScopeBase& scope, ClassInfo& info, ref_t reference, bool headerOnly = false);

      SizeInfo defineStructSize(ClassInfo& info);
      SizeInfo defineStructSize(ModuleScopeBase& scope, ref_t reference);

      bool validateTemplateAttribute(ref_t attribute, Visibility& visibility, TemplateType& type);
      bool validateSymbolAttribute(ref_t attribute, Visibility& visibility);
      bool validateClassAttribute(ref_t attribute, ref_t& flags, Visibility& visibility);
      bool validateFieldAttribute(ref_t attribute, FieldAttributes& attrs);
      bool validateMethodAttribute(ref_t attribute, MethodHint& hint, bool& explicitMode);
      bool validateImplicitMethodAttribute(ref_t attribute, MethodHint& hint);
      bool validateDictionaryAttribute(ref_t attribute, ref_t& dictionaryType);
      bool validateExpressionAttribute(ref_t attrValue, ExpressionAttributes& attrs);

      bool isRole(ClassInfo& info);
      bool isEmbeddableStruct(ClassInfo& info);

      bool isValidObjOp(int operatorId);
      bool isValidStrDictionaryOp(int operatorId);
      bool isValidObjArrayOp(int operatorId);
      bool isValidOp(int operatorId, BuildKey op);

      void tweakClassFlags(ClassInfo& info, bool classClassMode);
      void tweakPrimitiveClassFlags(ClassInfo& info, ref_t classRef);

      bool validateMessage(mssg_t message);
      void validateClassDeclaration(ClassInfo& info, bool& emptyStructure);

      void writeDictionaryEntry(MemoryBase* section, ustr_t key, int value);
      bool readDictionary(MemoryBase* section, ReferenceMap& map);

      void writeArrayEntry(MemoryBase* section, ref_t reference);

      //void injectVirtualCode(ClassInfo& classInfo);

      static CompilerLogic* getInstance()
      {
         static CompilerLogic instance;

         return &instance;
      }
   };

}

#endif // COMPILERLOGIC_H