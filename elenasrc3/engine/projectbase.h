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

   // --- XmlProjectBase ---
   class XmlProjectBase : public ProjectBase
   {
   public:
      // --- ProjectTree ---
      typedef Tree<ProjectOption, ProjectOption::None>                        ProjectTree;
      typedef Tree<ProjectOption, ProjectOption::None>::Node                  ProjectNode;
      typedef List<path_t, freepath>                                          Paths;
      typedef MemoryMap<ustr_t, ustr_t, Map_StoreUStr, Map_GetUStr, freeUStr> Forwards;

      class ModuleIterator;

      class FileIterator : public FileIteratorBase
      {
         friend class ModuleIterator;
         friend class XmlProjectBase;

      protected:
         XmlProjectBase* _project;
         ProjectNode     _node;

         path_t path()
         {
            return _project->_paths.get(_node.arg.value);
         }

         void next() override
         {
            _node = _node.nextNode();
         }

      public:
         bool loadKey(IdentifierString& retVal) override
         {
            ProjectNode key = _node.findChild(ProjectOption::Key);
            if (key == ProjectOption::Key) {
               retVal.copy(key.identifier());

               return true;
            }
            else return false;
         }

         bool eof() override
         {
            return _node != ProjectOption::FileKey;
         }

         FileIterator(XmlProjectBase* project)
         {
            _project = project;
         }
      };

      class ModuleIterator : public ModuleIteratorBase
      {
         friend class XmlProjectBase;

         ProjectNode  _node;
         FileIterator _fileIterator;
      protected:
         void next() override
         {
            _node = _node.nextNode();
         }

      public:
         /// <summary>
         /// NOTE : the identifier can be reallocated
         /// </summary>
         /// <returns></returns>
         ustr_t name() override
         {
            return _node.identifier();
         }

         bool eof() override
         {
            return _node != ProjectOption::Module;
         }

         FileIteratorBase& files() override
         {
            _fileIterator._node = _node.findChild(ProjectOption::FileKey);

            return _fileIterator;
         }

         ModuleIterator(XmlProjectBase* project)
            : _fileIterator(project)
         {
         }
      };

   protected:
      PlatformType   _platform;

      Paths          _paths;
      Forwards       _forwards;

      ProjectTree    _projectTree;
      ProjectNode    _root;

      ConfigFile::Node getPlatformRoot(ConfigFile& config, PlatformType platform);

   public:
      ustr_t StringSetting(ProjectOption option) const override;
      path_t PathSetting(ProjectOption option) const override;
      bool BoolSetting(ProjectOption option, bool defValue = false) const override;
      int IntSetting(ProjectOption option, int defValue) const override;

      ModuleIteratorBase* allocModuleIterator() override
      {
         ModuleIterator* it = new ModuleIterator(this);

         it->_node = _root.findChild(ProjectOption::Files).findChild(ProjectOption::Module);

         return it;
      }

      FileIteratorBase* allocPrimitiveIterator() override
      {
         FileIterator* it = new FileIterator(this);

         ProjectNode primitiveNode = _root.findChild(ProjectOption::Primitives);
         it->_node = primitiveNode.findChild(ProjectOption::FileKey);

         return it;
      }

      FileIteratorBase* allocPackageIterator() override
      {
         FileIterator* it = new FileIterator(this);

         ProjectNode primitiveNode = _root.findChild(ProjectOption::References);
         it->_node = primitiveNode.findChild(ProjectOption::FileKey);

         return it;
      }

      XmlProjectBase(PlatformType platform);
   };
}

#endif