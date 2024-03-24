//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session declaration
//
//                                             (C)2023-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELTVMSESSION_H
#define ELTVMSESSION_H

#include "core.h"

namespace elena_lang
{
   class VMSession
   {
      bool                 _started;

      FileEncoding         _encoding;

      PresenterBase*       _presenter;

      IdentifierString     _prefix;
      size_t               _prefixBookmark;
      IdentifierString     _postfix;

      DynamicString<char>  _body;

      SystemEnv            _env;
      ThreadContent        _tcontext;

      bool connect(void* tape);
      bool execute(void* tape);

      void executeCommandLine(const char* line);

      bool executeTape(void* tape);

   public:
      void printHelp();

      bool executeScript(const char* line);
      bool executeCommand(const char* line, bool& running);

      bool loadTemplate(path_t path);

      bool loadScript(ustr_t pathStr);

      void start();

      void run();

      VMSession(PresenterBase* presenter);
   };

}

#endif
