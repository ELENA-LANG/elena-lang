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
      ParserTargets,

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

      ManifestName,
      ManifestVersion,
      ManifestAuthor,

      // flags
      DebugMode,
      MappingOutputMode,
      OptimizationMode,
      GenerateParamNameInfo,
      ConditionalBoxing,

      Prolog,
      Epilog,

      Key,
      Value,
      Target,
      TargetType,
      TargetOption,
   };

   class FileIteratorBase
   {
   protected:
      virtual void next() = 0;
      virtual path_t path() = 0;

   public:
      virtual bool eof() = 0;

      virtual bool loadKey(IdentifierString& retVal) = 0;
      virtual bool loadTarget(IdentifierString& retVal) = 0;

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

   class CategoryIteratorBase
   {
   protected:
      virtual void next() = 0;

   public:
      virtual ustr_t name() = 0;

      virtual bool eof() = 0;

      virtual void loadOptions(ProjectOption option, IdentifierString& value, char separator) = 0;
      virtual bool loadOption(ProjectOption option, IdentifierString& value) = 0;

      CategoryIteratorBase& operator ++()
      {
         next();

         return *this;
      }

      virtual ~CategoryIteratorBase() = default;
   };

   struct ProjectTarget
   {
      int              type;
      /**
       * options are separated by \n
       */
      IdentifierString options;
   };

   typedef Map<ustr_t, ProjectTarget*, allocUStr, freeUStr, freeobj> ProjectTargets;

   struct ProjectEnvironment
   {
      PathString       projectPath;
      IdentifierString fileProlog;
      IdentifierString fileEpilog;

      ProjectTargets   targets;

      ProjectEnvironment()
         : targets(nullptr)
      {
         
      }
   };

   class ProjectBase : public ForwardResolverBase
   {
   public:
      virtual ModuleIteratorBase* allocModuleIterator() = 0;

      virtual FileIteratorBase* allocPrimitiveIterator() = 0;
      virtual FileIteratorBase* allocPackageIterator() = 0;
      virtual CategoryIteratorBase* allocTargetIterator() = 0;

      virtual path_t PathSetting(ProjectOption option) const = 0;
      virtual path_t PathSetting(ProjectOption option, ustr_t key) const = 0;

      virtual ustr_t StringSetting(ProjectOption option) const = 0;
      virtual bool BoolSetting(ProjectOption option, bool defValue = false) const = 0;
      virtual int IntSetting(ProjectOption option, int defValue = 0) const = 0;
      virtual unsigned int UIntSetting(ProjectOption option, unsigned int defValue = 0) const = 0;

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

      virtual void initEnvironment(ProjectEnvironment& env)
      {
         env.projectPath.copy(PathSetting(ProjectOption::ProjectPath));
         env.fileProlog.copy(StringSetting(ProjectOption::Prolog));
         env.fileEpilog.copy(StringSetting(ProjectOption::Epilog));

         // load targets
         auto target_it = allocTargetIterator();
         while (!target_it->eof()) {
            ustr_t name = target_it->name();

            ProjectTarget* target = new ProjectTarget();
            IdentifierString tmp;
            if(target_it->loadOption(ProjectOption::TargetType, tmp)) {
               target->type = tmp.toInt();
            }

            env.targets.add(name, target);

            target_it->loadOptions(ProjectOption::TargetOption, target->options, '\n');
            ++(*target_it);
         }
         freeobj(target_it);
      }

      virtual ~ProjectBase() = default;
   };
}

#endif