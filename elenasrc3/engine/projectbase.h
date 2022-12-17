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
#include "tree.h"

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
      virtual ustr_t StringSetting(ProjectOption option) const = 0;
      virtual bool BoolSetting(ProjectOption option, bool defValue = false) const = 0;
      virtual int IntSetting(ProjectOption option, int defValue = 0) const = 0;

      virtual ~ProjectBase() = default;
   };
}

#endif