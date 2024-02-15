//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM terminal
//
//                                              (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
#include "eltconst.h"
#include "vmsession.h"
#include "windows/presenter.h"

#include <windows.h>

using namespace elena_lang;

void getAppPath(PathString& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(NULL, path, MAX_PATH);

   appPath.copySubPath(path, true);
   appPath.lower();
}

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

   PathString commandPath;
   getAppPath(commandPath);
   commandPath.combine(COMMAMD_TEMPLATE);
   session.loadTemplate(*commandPath);

   session.loadScript(ELT_CONFIG);
   session.loadScript(ELT_GRAMMAR_CONFIG);
   session.loadScript(ELT_LSCRIPT_CONFIG);

   session.printHelp();

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

   session.run();

   return 0;
}