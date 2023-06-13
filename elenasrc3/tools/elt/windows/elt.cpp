//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM terminal
//
//                                              (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
#include "eltconst.h"
#include "vmsession.h"
#include "windows/presenter.h"

using namespace elena_lang;

class ELTPresenter : public WinConsolePresenter
{
public:
   ustr_t getMessage(int code) override
   {
      // !! temporally
      return nullptr;
   }
};

int main()
{
   printf(ELT_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELT_REVISION_NUMBER);

   ELTPresenter presenter;
   VMSession session(&presenter);

   if (!session.connect())
      return -1;

   PathString commandPath(COMMAMD_TEMPLATE);
   session.loadTemplate(*commandPath);

   session.loadScript(ELT_CONFIG);
   //loadScript("~\\scripts\\grammar.es");
   //loadScript("~\\scripts\\tscript.es");

   //// load script passed via command line arguments
   //if (argc > 1) {
   //   for (int i = 1; i < argc; i++) {
   //      if (argv[i][0] == '-') {
   //         // check exit command
   //         if (argv[i][1] == 'q')
   //            return 0;

   //         executeCommand(argv[i]);
   //      }
   //      else executeScript(argv[i]);
   //   }
   //}

   //runSession();

   return 0;
}