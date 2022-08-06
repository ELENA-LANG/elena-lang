//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the project class declaration
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PROJECT_H
#define PROJECT_H

#include "clicommon.h"
#include "config.h"

namespace elena_lang
{
   // --- Project
   class Project : public ProjectBase
   {
      // --- ProjectTree ---
      typedef Tree<ProjectOption, ProjectOption::None>                        ProjectTree;
      typedef Tree<ProjectOption, ProjectOption::None>::Node                  ProjectNode;
      typedef List<path_t, freepath>                                          Paths;
      typedef MemoryMap<ustr_t, ustr_t, Map_StoreUStr, Map_GetUStr, freeUStr> Forwards;

      FileEncoding   _encoding;

      ProjectTree    _projectTree;
      ProjectNode    _root;

      Paths          _paths;
      Forwards       _forwards;

      PathString     _basePath;
      PathString     _projectPath;

      bool           _loaded;

      PresenterBase* _presenter;

      class ModuleIterator;

      class FileIterator : public FileIteratorBase
      {
         friend class ModuleIterator;
         friend class Project;

      protected:
         Project*    _project;
         ProjectNode _node;

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

         FileIterator(Project* project)
         {
            _project = project;
         }
      };

      class ModuleIterator : public ModuleIteratorBase
      {
         friend class Project;

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

         ModuleIterator(Project* project)
            : _fileIterator(project)
         {
         }
      };

      PlatformType     _platform;
      IdentifierString _projectName;
      IdentifierString _defaultNs;

      void addPathSetting(ProjectOption key, path_t path);
      void addStringSetting(ProjectOption key, ustr_t s);

      void loadSourceFiles(ConfigFile& config, ConfigFile::Node& configRoot);

      void loadPathCollection(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath, 
         ProjectOption collectionKey, path_t configPath);

      void loadForwards(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath);

      void loadKeyCollection(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath,
         ProjectOption collectionKey, ProjectOption itemKey, ustr_t prefix);

      void loadPathSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, 
         ProjectOption key, path_t configPath);

      void loadSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, IdentifierString& value);

      void copySetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, ProjectOption key);

      void loadTargetType(ConfigFile& config, ConfigFile::Node& configRoot);

      void loadConfig(ConfigFile& config, path_t configPath);
      void loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node& root);

      void loadDefaultConfig();

      path_t PathSetting(ProjectOption option, ustr_t key) const;

      ustr_t resolveKey(ProjectOption category, ProjectOption item, ustr_t key);

   public:
      PlatformType Platform() override;
      PlatformType TargetType() override;

      ustr_t ProjectName() override
      {
         return *_projectName;
      }

      ustr_t Namespace() override
      {
         return _defaultNs.str();
      }

      path_t PathSetting(ProjectOption option) const override;
      ustr_t StringSetting(ProjectOption option) const override;
      bool BoolSetting(ProjectOption option, bool defValue) const override;
      int IntSetting(ProjectOption option, int defValue) const override;

      ustr_t resolveForward(ustr_t weakReference) override;
      ustr_t resolveExternal(ustr_t forward) override;
      ustr_t resolveWinApi(ustr_t forward) override;

      void addForward(ustr_t forward, ustr_t referenceName) override;

      void addBoolSetting(ProjectOption option, bool value);

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

      void addSource(ustr_t ns, path_t path);

      bool loadConfig(path_t path, bool mainConfig);

      bool loadProject(path_t path);

      void prepare() override;

      Project(path_t path, PlatformType platform, PresenterBase* presenter)
         : _paths(nullptr), _forwards(nullptr), _basePath(path)
      {
         _encoding = FileEncoding::UTF8;

         ProjectTree::Writer writer(_projectTree);
         writer.newNode(ProjectOption::Root);

         _root = writer.CurrentNode();
         _platform = platform;

         _loaded = false;
         _presenter = presenter;
      }
      ~Project() override = default;
   };

}

#endif // PROJECT_H