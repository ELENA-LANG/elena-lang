//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Project
//
//		This file contains the xml project base class declaration
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef XMLPROJECTBASE_H
#define XMLPROJECTBASE_H

#include "projectbase.h"
#include "tree.h"

namespace elena_lang
{

   // --- XmlProjectBase ---
   class XmlProjectBase : public ProjectBase
   {
   public:
      // --- ProjectTree ---
      typedef Tree<ProjectOption, ProjectOption::None>                        ProjectTree;
      typedef Tree<ProjectOption, ProjectOption::None>::Node                  ProjectNode;
      typedef List<path_t, freepath>                                          Paths;

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
            return false;
         }

         bool loadTarget(IdentifierString& retVal) override
         {
            ProjectNode key = _node.findChild(ProjectOption::Target);
            if (key == ProjectOption::Target) {
               retVal.copy(key.identifier());

               return true;
            }
            return false;
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

         int hints() override
         {
            ProjectNode hintNode = _node.findChild(ProjectOption::Hints);
            if (hintNode == ProjectOption::Hints) {
               return StrConvertor::toInt(hintNode.identifier(), 10);
            }
            
            return 0;
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

      class CategoryIterator : public  CategoryIteratorBase
      {
         ProjectNode  _node;

      protected:
         void next() override
         {
            _node = _node.nextNode();
         }

      public:
         bool eof() override
         {
            return _node == ProjectOption::None;
         }

         ustr_t name() override
         {
            return _node.identifier();
         }

         bool loadOption(ProjectOption option, IdentifierString& value) override
         {
            ProjectNode current = _node.findChild(option);
            if (current != ProjectOption::None) {
               value.copy(current.identifier());

               return true;
            }
            return false;
         }

         void loadOptions(ProjectOption option, IdentifierString& value, char separator) override
         {
            value.clear();

            ProjectNode current = _node.findChild(option);
            while (current != ProjectOption::None) {
               if (current.key == option) {
                  if (!value.empty()) {
                     value.append(separator);
                  }
                  value.append(current.identifier());
               }
               current = current.nextNode();
            }
         }

         CategoryIterator(ProjectNode node)
         {
            _node = node;
         }
      };

   protected:
      PlatformType   _platform;

      Paths          _paths;
      Forwards       _forwards;
      LexicalMap     _lexicals;

      ProjectTree    _projectTree;
      ProjectNode    _root;

      ConfigFile::Node getPlatformRoot(ConfigFile& config, PlatformType platform);
      ConfigFile::Node getProfileRoot(ConfigFile& config, ConfigFile::Node& root, ustr_t profileName);

      void loadKeyCollection(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath,
         ProjectOption collectionKey, ProjectOption itemKey, ustr_t prefix);

      void loadPathCollection(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath,
         ProjectOption collectionKey, path_t configPath);

      void loadPathSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath,
         ProjectOption key, path_t configPath);

      void loadBoolSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath,
         ProjectOption key);

      void loadForwards(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath);
      void loadLexicals(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath);

      ustr_t resolveKey(ProjectOption category, ProjectOption item, ustr_t key);

   public:
      void addForward(ustr_t forward, ustr_t referenceName) override;

      ustr_t resolveForward(ustr_t weakReference) override;
      ustr_t resolveWinApi(ustr_t forward) override;
      ustr_t resolveExternal(ustr_t forward) override;

      ustr_t StringSetting(ProjectOption option) const override;

      path_t PathSetting(ProjectOption option) const override;
      path_t PathSetting(ProjectOption option, ustr_t key) const override;

      bool BoolSetting(ProjectOption option, bool defValue = false) const override;

      int IntSetting(ProjectOption option, int defValue) const override;
      unsigned UIntSetting(ProjectOption option, unsigned int defValue) const override;

      LexicalMap::Iterator getLexicalIterator() override
      {
         return _lexicals.start();
      }

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

      CategoryIteratorBase* allocTargetIterator() override
      {
         CategoryIterator* it = new CategoryIterator(_root.findChild(ProjectOption::ParserTargets));

         return it;
      }

      XmlProjectBase(PlatformType platform);
   };

}

#endif
