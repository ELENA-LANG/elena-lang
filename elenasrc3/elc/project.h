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
#include "xmlprojectbase.h"

namespace elena_lang
{
   // --- Project
   class Project : public XmlProjectBase
   {
      FileEncoding   _encoding;

      PathString     _basePath;
      PathString     _projectPath;

      bool           _loaded;

      PresenterBase* _presenter;

      IdentifierString _projectName;
      IdentifierString _defaultNs;

      void addPathSetting(ProjectOption key, path_t path);
      void addStringSetting(ProjectOption key, ustr_t s);

      void loadSourceFiles(ConfigFile& config, ConfigFile::Node& configRoot);

      void loadForwards(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath);

      void loadKeyCollection(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath,
         ProjectOption collectionKey, ProjectOption itemKey, ustr_t prefix);

      void loadPathSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, 
         ProjectOption key, path_t configPath);

      void loadSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, IdentifierString& value);

      void copySetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, ProjectOption key, bool exclusiveMode = false);

      void loadTargetType(ConfigFile& config, ConfigFile::Node& configRoot);

      void loadConfig(ConfigFile& config, path_t configPath);
      void loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node& root);

      void loadDefaultConfig();

      path_t PathSetting(ProjectOption option, ustr_t key) const;

      ustr_t resolveKey(ProjectOption category, ProjectOption item, ustr_t key);

   public:
      PlatformType SystemTarget();
      PlatformType Platform();
      PlatformType TargetType();

      ustr_t ProjectName()
      {
         return *_projectName;
      }

      ustr_t Namespace()
      {
         return _defaultNs.str();
      }

      ustr_t StringSetting(ProjectOption option) const override;
      path_t PathSetting(ProjectOption option) const override;

      ustr_t resolveForward(ustr_t weakReference) override;
      ustr_t resolveExternal(ustr_t forward) override;
      ustr_t resolveWinApi(ustr_t forward) override;

      void addForward(ustr_t forward, ustr_t referenceName) override;

      void addBoolSetting(ProjectOption option, bool value);
      void addIntSetting(ProjectOption option, int value);

      void forEachForward(void* arg, void(* feedback)(void* arg, ustr_t key, ustr_t value)) override;

      void addSource(ustr_t ns, path_t path);

      bool loadConfigByName(path_t configPath, ustr_t name, bool markAsLoaded);

      bool loadConfig(path_t path, bool mainConfig);

      bool loadProject(path_t path);

      void prepare();

      Project(path_t path, PlatformType platform, PresenterBase* presenter)
         : XmlProjectBase(platform), _basePath(path)
      {
         _encoding = FileEncoding::UTF8;

         _platform = platform;

         _loaded = false;
         _presenter = presenter;
      }
      ~Project() override = default;
   };

}

#endif // PROJECT_H