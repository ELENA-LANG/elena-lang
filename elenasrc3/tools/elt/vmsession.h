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
   class VMSession
   {
      bool                 _started;
      bool                 _multiLine;

      PathString           _appPath;

      FileEncoding         _encoding;

      PresenterBase*       _presenter;

      IdentifierString     _prefix1;
      IdentifierString     _postfix1;
      IdentifierString     _prefix2;
      IdentifierString     _postfix2;
      IdentifierString     _prefix3;
      IdentifierString     _postfix3;
      IdentifierString     _prefix4;
      IdentifierString     _postfix4;

      IdentifierString     _plugedCode;

      DynamicString<char>  _body;

      IdentifierList       _imports;

      SystemEnv            _env;

      bool connect(void* tape);
      bool execute(void* tape);

      bool executeAssigning(ustr_t line);

      void executeCommandLine(bool preview, const char* line, ustr_t prefix, ustr_t postfix, ustr_t prefix2 = nullptr, ustr_t postfix2 = nullptr);

      bool executeTape(void* tape);

      bool loadPlugin(ustr_t name);

   public:
      void printHelp();

      bool executeScript(const char* line);
      bool executeCommand(const char* line, bool& running);

      bool loadTemplate(path_t path);

      bool loadScript(ustr_t pathStr);

      void executeBody();

      void start();

      void run();

      VMSession(path_t appPath, PresenterBase* presenter);
   };

}

#endif
