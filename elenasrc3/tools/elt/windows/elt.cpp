//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing VM terminal
//
//                                              (C)2021-2026, by Aleksey Rakov
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

// default script mode
void startInDefaultMode(VMSession& session)
{
   session.start();

   session.loadScript(ELT_GRAMMAR_CONFIG);
   session.loadScript(ELT_LSCRIPT_CONFIG);
}

int main(int argc, char* argv[])
{
   printf(ELT_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELT_REVISION_NUMBER);

   PathString appPath;
   getAppPath(appPath);

   ELTPresenter presenter;
   VMSession session(*appPath, &presenter);

   PathString configPath(*appPath, ELT_COMMAND_CONFIG);
   if (!session.loadConfig(*configPath)) {
      presenter.printPath(ELT_CANNOT_LOAD_TEMPLATE, *configPath);

      return EXIT_FAILURE;
   }      

   PathString basePath(*appPath);
   basePath.combine(L"scripts");
   session.setBasePath(*basePath);

   session.loadScript(ELT_CONFIG);

   // load script passed via command line arguments
   if (argc > 1) {
      for (int i = 1; i < argc; i++) {
         IdentifierString cmd(argv[i]);

         if (argv[i][0] == '-') {
            bool running = true;
            if (argv[i][1] == 'i') {
               startInDefaultMode(session);
            }            

            // check exit command
            if (!running)
               return 0;
         }
         else session.executeScript(*cmd);
      }
   }
   else startInDefaultMode(session);

//   session.printHelp();

   session.run();

   return EXIT_SUCCESS;
}