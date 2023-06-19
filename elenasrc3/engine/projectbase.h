//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Project
//
//		This file contains the project base class declaration
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PROJECTBASE_H
#define PROJECTBASE_H

#include "elena.h"
#include "config.h"

namespace elena_lang
{
   // --- ProjectBase ---

   enum OptimizationModes
   {
      optNone = 0,
      optLow = 1,
      optMiddle = 2
   };

   enum class ProjectOption
   {
      None = 0,

      // collections
      Root,
      Files,
      Templates,
      Primitives,
      Externals,
      Winapis,
      References,

      Namespace,

      Module,
      FileKey,
      External,
      Winapi,

      TargetPath,
      BasePath,
      OutputPath,
      ProjectPath,
      LibPath,

      ClassSymbolAutoLoad,
      StackAlignment,
      RawStackAlignment,
      GCMGSize,
      GCYGSize,
      EHTableEntrySize,
      ThreadCounter,

      // flags
      DebugMode,
      MappingOutputMode,
      OptimizationMode,
      GenerateParamNameInfo,

      Prolog,
      Epilog,

      Key,
      Value,
   };

   class FileIteratorBase
   {
   protected:
      virtual void next() = 0;
      virtual path_t path() = 0;

   public:
      virtual bool eof() = 0;

      virtual bool loadKey(IdentifierString& retVal) = 0;

      path_t operator*()
      {
         return path();
      }

      FileIteratorBase& operator ++()
      {
         next();

         return *this;
      }
      virtual ~FileIteratorBase() = default;
   };

   class ModuleIteratorBase
   {
   protected:
      virtual void next() = 0;

   public:
      virtual ustr_t name() = 0;

      virtual bool eof() = 0;

      virtual FileIteratorBase& files() = 0;

      ModuleIteratorBase& operator ++()
      {
         next();

         return *this;
      }
      virtual ~ModuleIteratorBase() = default;
   };

   class ProjectBase : public ForwardResolverBase
   {
   public:
      virtual ModuleIteratorBase* allocModuleIterator() = 0;

      virtual FileIteratorBase* allocPrimitiveIterator() = 0;
      virtual FileIteratorBase* allocPackageIterator() = 0;

      virtual path_t PathSetting(ProjectOption option) const = 0;
      virtual path_t PathSetting(ProjectOption option, ustr_t key) const = 0;

      virtual ustr_t StringSetting(ProjectOption option) const = 0;
      virtual bool BoolSetting(ProjectOption option, bool defValue = false) const = 0;
      virtual int IntSetting(ProjectOption option, int defValue = 0) const = 0;

      virtual void initLoader(LibraryProviderBase& libraryProvider)
      {
         // load primitives
         auto path_it = allocPrimitiveIterator();
         while (!path_it->eof()) {
            IdentifierString key;
            path_it->loadKey(key);

            if ((*key).compare(CORE_ALIAS)) {
               libraryProvider.addCorePath(**path_it);
            }
            else libraryProvider.addPrimitivePath(*key, **path_it);

            ++(*path_it);
         }
         freeobj(path_it);

         // load packages
         auto package_it = allocPackageIterator();
         while (!package_it->eof()) {
            IdentifierString key;
            package_it->loadKey(key);

            libraryProvider.addPackage(*key, **package_it);

            ++(*package_it);
         }
         freeobj(package_it);

         // set output paths
         path_t libPath = PathSetting(ProjectOption::LibPath);
         if (!libPath.empty())
            libraryProvider.setRootPath(libPath);
      }

      virtual ~ProjectBase() = default;
   };
}

#endif