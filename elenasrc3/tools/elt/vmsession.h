//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session code
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELTVMSESSION_H
#define ELTVMSESSION_H

namespace elena_lang
{
   class VMSession
   {
      bool                 _started;

      FileEncoding         _encoding;

      PresenterBase*       _presenter;

      IdentifierString     _prefix;
      IdentifierString     _postfix;

      DynamicString<char>  _body;

      bool connect(void* tape);
      bool execute(void* tape);

      void executeCommandLine(const char* line);

      bool executeTape(void* tape);
      bool executeScript(const char* line);

   public:
      bool loadTemplate(path_t path);

      bool loadScript(ustr_t pathStr);

      void run();

      VMSession(PresenterBase* presenter);
   };

}

#endif
