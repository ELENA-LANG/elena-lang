//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing doc generator header
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LODC_H
#define LODC_H

#include "elena.h"
#include "libman.h"

namespace elena_lang
{
   // --- PresenterBase ---
   class PresenterBase
   {
   public:
      virtual void print(ustr_t message) = 0;
      virtual void print(ustr_t message, ustr_t arg) = 0;
      virtual void printPath(ustr_t message, path_t arg) = 0;

      virtual ~PresenterBase() = default;
   };

   typedef List<ustr_t, freeUStr>       StringList;

   struct ApiMethodInfo
   {
      IdentifierString  prefix;
      IdentifierString  name;
      IdentifierString  shortDescr;
      IdentifierString  outputType;

      bool              extensionOne;
      bool              special;
      bool              property;
      bool              cast;

      StringList        params;
      StringList        paramNames;

      ApiMethodInfo()
         : extensionOne(false), params(nullptr), paramNames(nullptr)
      {
         cast = property = special = false;
      }
   };

   typedef List<ApiMethodInfo*, freeobj> ApiMethodInfoList;

   struct ApiFieldInfo
   {
      bool              special;
      IdentifierString  prefix;
      IdentifierString  name;
      IdentifierString  shortDescr;
      IdentifierString  type;
   };

   typedef List<ApiFieldInfo*, freeobj> ApiFieldInfoList;

   struct ApiClassInfo
   {
      bool              templateBased;

      IdentifierString  prefix;
      IdentifierString  fullName;
      IdentifierString  name;
      IdentifierString  shortDescr;
      IdentifierString  title;

      StringList        parents;
      ApiMethodInfoList methods;
      ApiFieldInfoList  fields;
      ApiMethodInfoList constructors;
      ApiMethodInfoList properties;
      ApiMethodInfoList staticProperties;
      ApiMethodInfoList extensions;

      ApiClassInfo()
         : parents(nullptr), methods(nullptr ), fields(nullptr),
            constructors(nullptr), extensions(nullptr),
            properties(nullptr), staticProperties(nullptr)
      {
         
      }
   };

   struct ApiSymbolInfo
   {
      IdentifierString  prefix;
      IdentifierString  fullName;
      IdentifierString  name;
      IdentifierString  shortDescr;
      IdentifierString  title;

      IdentifierString  type;
   };

   inline int sortApiClassInfo(ApiClassInfo* p, ApiClassInfo* n)
   {
      if ((*p->name).greater((*n->name))) {
         return -1;
      }
      else if ((*p->name).compare((*n->name))) {
         return 0;
      }
      else return 1;
   }

   inline int sortApiSymbolInfo(ApiSymbolInfo* p, ApiSymbolInfo* n)
   {
      if ((*p->name).greater((*n->name))) {
         return -1;
      }
      else if ((*p->name).compare((*n->name))) {
         return 0;
      }
      else return 1;
   }

   typedef SortedList<ApiClassInfo*, sortApiClassInfo, freeobj> ApiClassInfoList;
   typedef SortedList<ApiSymbolInfo*, sortApiSymbolInfo, freeobj> ApiSymbolInfoList;

   struct ApiModuleInfo
   {
      IdentifierString name;
      IdentifierString shortDescr;

      ApiClassInfoList  classes;
      ApiSymbolInfoList symbols;

      ApiModuleInfo()
         : classes(nullptr), symbols(nullptr)
      {

      }
   };

   typedef List<ApiModuleInfo*, freeobj> ApiModuleInfoList;
   typedef Map<ustr_t, ustr_t, allocUStr, freeUStr, freeUStr>   DescriptionMap;

   // --- DocGenerator ---
   class DocGenerator
   {
      enum class MemberType
      {
         Normal = 0,
         ClassClass,
         Extension
      };

      PresenterBase*   _presenter;
      LibraryProvider* _provider;
      ModuleBase*      _module;
      IdentifierString _rootNs;
      bool             _publicOnly;

      DescriptionMap   _classDescriptions;

      ApiModuleInfo* findModule(ApiModuleInfoList& modules, ustr_t ns);
      ApiClassInfo* findClass(ApiModuleInfo* module, ustr_t name);
      ApiSymbolInfo* findSymbol(ApiModuleInfo* module, ustr_t name);

      bool isExtension(ref_t reference);
      ref_t findExtensionTarget(ref_t reference);

      void generateFieldList(TextFileWriter& bodyWriter, ApiFieldInfoList& list);
      void generateMethodList(TextFileWriter& bodyWriter, ApiMethodInfoList& list);
      void generateClassDoc(TextFileWriter& summaryWriter, TextFileWriter& bodyWriter, ApiClassInfo* classInfo, ustr_t bodyName);
      void generateSymbolDoc(TextFileWriter& summaryWriter, TextFileWriter& bodyWriter, ApiSymbolInfo* symbolInfo, ustr_t bodyName);
      void generateModuleDoc(ApiModuleInfo* moduleInfo);

      bool loadClassInfo(ref_t reference, ClassInfo& info, bool headerOnly = true);
      bool loadSymbolInfo(ref_t reference, SymbolInfo& info);

      void loadClassPrefixes(ApiClassInfo* apiClassInfo, ref_t reference);
      void loadParents(ApiClassInfo* apiClassInfo, ref_t parentRef);
      void loadFields(ApiClassInfo* apiClassInfo, ClassInfo& info);
      void loadMethodName(ApiMethodInfo* apiMethodInfo, bool templateBased);
      void loadClassMethod(ApiClassInfo* apiClassInfo, mssg_t message, MethodInfo& methodInfo, 
         MemberType memberType, DescriptionMap* descriptions);

      void loadClassMembers(ApiClassInfo* apiClassInfo, ref_t reference, DescriptionMap* descriptions);
      void loadConstructors(ApiClassInfo* apiClassInfo, ref_t reference, DescriptionMap* descriptions);
      void loadExtensions(ApiClassInfo* apiClassInfo, ref_t reference, DescriptionMap* descriptions);

      void loadDescriptions(ref_t reference, DescriptionMap& map);
      void loadDescriptions();

   public:
      void loadNestedModules(ApiModuleInfoList& modules);
      void loadMember(ApiModuleInfoList& modules, ref_t reference);

      bool load(path_t path);
      bool loadByName(ustr_t name);

      void generate();

      DocGenerator(LibraryProvider* provider, PresenterBase* presenter)
         : _classDescriptions(nullptr)
      {
         _presenter = presenter;
         _provider = provider;
         _module = nullptr;
         _publicOnly = true;
      }
      virtual ~DocGenerator() = default;
   };
}

#endif
