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
      FileEncoding      _encoding;

      PresenterBase*    _presenter;

      IdentifierString  _prefix;
      IdentifierString  _postfix;

      bool executeTape(void* tape);

   public:
      bool loadTemplate(path_t path);

      bool loadScript(ustr_t pathStr);

      bool connect();

      VMSession(PresenterBase* presenter);
   };

}

#endif
