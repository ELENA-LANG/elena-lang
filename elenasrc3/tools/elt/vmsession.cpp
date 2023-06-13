//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM session code
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
#include "vmsession.h"
#include "core.h"
#include "elenasm.h"
#include "elenavm.h"
#include "eltconst.h"

using namespace elena_lang;

const char* trim(const char* s)
{
   while (s[0] == 0x20)s++;

   return s;
}

// --- VMSession ---

VMSession :: VMSession(PresenterBase* presenter)
{
   _encoding = FileEncoding::UTF8;

   _presenter = presenter;
}

bool VMSession :: loadTemplate(path_t path)
{
   char buff[512];

   TextFileReader reader(path, FileEncoding::UTF8, false);

   if (reader.isOpen()) {
      IdentifierString content;
      reader.readAll(content, buff);

      size_t index = (*content).findStr("$1");
      if (index != NOTFOUND_POS) {
         _prefix.copy(*content, index);
         _postfix.copy(*content + index + 2);
      }
      else _prefix.copy(*content);

      return true;
   }
   else return false;
}

bool VMSession :: executeTape(void* tape)
{
   return false;
}

bool VMSession :: loadScript(ustr_t pathStr)
{
   pathStr = trim(pathStr);

   void* tape = InterpretFileSMLA(pathStr, (int)_encoding, false);
   if (tape == nullptr) {
      char error[0x200];
      size_t length = GetStatusSMLA(error, 0x200);
      error[length] = 0;
      if (!emptystr(error)) {
         _presenter->print(ELT_SCRIPT_FAILED, error);
         return false;
      }
      return true;
   }
   return executeTape(tape);
}

bool VMSession :: connect()
{
   SystemEnv env = { };

   env.gc_yg_size = 0x15000;
   env.gc_mg_size = 0x54000;

   int retVal = InitializeVMSTLA(&env, nullptr, nullptr);
   if (retVal != 0) {
      _presenter->print(ELT_STARTUP_FAILED);

      return false;
   }

   return true;
}
