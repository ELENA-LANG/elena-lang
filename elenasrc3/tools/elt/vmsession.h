//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session declaration
//
//                                             (C)2023-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELTVMSESSION_H
#define ELTVMSESSION_H

#include "core.h"

namespace elena_lang
{
   enum class TemplateType
   {
      REPL,
      Multiline,
      GetVar,
      SetVar
   };

   struct TemplateInfo
   {
      IdentifierString     prefix;
      IdentifierString     postfix;

      void clear()
      {
         prefix.clear();
         postfix.clear();
      }
   };

   class VMSession
   {
      bool                 _started;
      bool                 _multiLine;

      PathString           _appPath;
      PathString           _basePath;

      FileEncoding         _encoding;

      PresenterBase*       _presenter;

      TemplateInfo         _repl;
      TemplateInfo         _multiline;
      TemplateInfo         _get_var;
      TemplateInfo         _set_var;

      DynamicString<char>  _body;

      IdentifierList       _imports;

      SystemEnv            _env;

      bool connect(void* tape);
      bool execute(void* tape);

      void executeCommandLine(bool preview, TemplateType type, ustr_t script);

      bool executeTape(void* tape);

      void setBasePath(ustr_t baseStr);

      bool importScript(ustr_t name);

   public:
      void printHelp();

      bool loadTemplate(TemplateType type, ustr_t name);

      bool executeScript(const char* line);
      bool executeCommand(const char* line, bool& running);

      bool loadScript(ustr_t pathStr);

      void start();

      void run();

      VMSession(path_t appPath, PresenterBase* presenter);
   };

}

#endif
