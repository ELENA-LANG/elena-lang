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

   struct ApiClassInfo
   {
      IdentifierString prefix;
      IdentifierString fullName;
      IdentifierString shortDescr;
   };

   typedef List<ApiClassInfo*, freeobj> ApiClassInfoList;

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

      void generateClassDoc(TextFileWriter& summaryWriter, TextFileWriter& bodyWriter, ApiClassInfo* classInfo, ustr_t name);
      void generateModuleDoc(ApiModuleInfo* moduleInfo);

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
