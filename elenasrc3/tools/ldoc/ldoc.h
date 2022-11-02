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

   struct ApiClassInfo
   {
      IdentifierString prefix;
      IdentifierString fullName;
      IdentifierString name;
      IdentifierString shortDescr;
      IdentifierString title;

      StringList       parents;

      ApiClassInfo()
         : parents(nullptr)
      {
         
      }
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

   typedef SortedList<ApiClassInfo*, sortApiClassInfo, freeobj> ApiClassInfoList;

   struct ApiModuleInfo
   {
      IdentifierString name;
      IdentifierString shortDescr;

      ApiClassInfoList classes;

      ApiModuleInfo()
         : classes(nullptr)
      {

      }
   };

   typedef List<ApiModuleInfo*, freeobj> ApiModuleInfoList;

   // --- DocGenerator ---
   class DocGenerator
   {
      PresenterBase*   _presenter;
      LibraryProvider* _provider;
      ModuleBase*      _module;
      IdentifierString _rootNs;
      bool             _publicOnly;

      ApiModuleInfo* findModule(ApiModuleInfoList& modules, ustr_t ns);
      ApiClassInfo* findClass(ApiModuleInfo* module, ustr_t name);

      void generateClassDoc(TextFileWriter& summaryWriter, TextFileWriter& bodyWriter, ApiClassInfo* classInfo, ustr_t bodyName);
      void generateModuleDoc(ApiModuleInfo* moduleInfo);

      bool loadClassInfo(ref_t reference, ClassInfo& info);

      void loadParents(ApiClassInfo* apiClassInfo, ref_t parentRef);
      void loadClassMembers(ApiClassInfo* apiClassInfo, ref_t reference);

   public:
      void loadNestedModules(ApiModuleInfoList& modules);
      void loadMember(ApiModuleInfoList& modules, ref_t reference);

      bool load(path_t path);
      bool loadByName(ustr_t name);

      void generate();

      DocGenerator(LibraryProvider* provider, PresenterBase* presenter)
      {
         _presenter = presenter;
         _provider = provider;
         _module = nullptr;
         _publicOnly = true;
      }
      virtual ~DocGenerator()
      {
      }
   };
}

#endif
