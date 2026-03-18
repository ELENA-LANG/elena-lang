//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main linux / freebsd file containing VM terminal
//
//                                              (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
#include "eltconst.h"
#include "vmsession.h"
#include "linux/presenter.h"

using namespace elena_lang;

class ELTPresenter : public LinuxConsolePresenter
{
public:
   ustr_t getMessage(int code) override
   {
      // !! temporally
      return nullptr;
   }
};

//// default script mode
//void startInDefaultMode(VMSession& session)
//{
//   session.start();
//
//   session.loadScript(ELT_GRAMMAR_CONFIG);
//   session.loadScript(ELT_LSCRIPT_CONFIG);
//}
//
//inline void loadTemplate(ELTPresenter& presenter, VMSession& session, TemplateType type, ustr_t name)
//{
//   if (!session.loadTemplate(type, name))
//      presenter.printLine(ELT_CANNOT_LOAD_TEMPLATE, name);
//}

inline bool getAppPath(PathString& appPath)
{
   char buffer[512];
   size_t len = 512;

#if defined(__FreeBSD__) 

   int mib[4];
   mib[0] = CTL_KERN;
   mib[1] = KERN_PROC;
   mib[2] = KERN_PROC_PATHNAME;
   mib[3] = -1;
   sysctl(mib, 4, buffer, &len, nullptr, 0);

#elif defined(__unix__)

   if (readlink("/proc/self/exe", buffer, len) == -1)
      return false;

#endif

   size_t index = path_t(buffer).findLast(PATH_SEPARATOR);
   if (index != NOTFOUND_POS)
      appPath.copy(buffer, index);

   return true;
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

   //loadTemplate(presenter, session, TemplateType::REPL, REPL_TEMPLATE_NAME);
   //loadTemplate(presenter, session, TemplateType::Multiline, MULTILINE_TEMPLATE_NAME);
   //loadTemplate(presenter, session, TemplateType::GetVar, GETVAR_TEMPLATE_NAME);
   //loadTemplate(presenter, session, TemplateType::SetVar, SETVAR_TEMPLATE_NAME);

   //session.loadScript(ELT_CONFIG);

   //// load script passed via command line arguments
   //if (argc > 1) {
   //   for (int i = 1; i < argc; i++) {
   //      IdentifierString cmd(argv[i]);

   //      if (argv[i][0] == '-') {
   //         bool running = true;
   //         if (argv[i][1] == 'i') {
   //            startInDefaultMode(session);
   //         }
   //         else session.executeCommand(*cmd, running);

   //         // check exit command
   //         if (!running)
   //            return 0;
   //      }
   //      else session.executeScript(*cmd);
   //   }
   //}
   //else startInDefaultMode(session);

   //session.printHelp();

   session.run();

   return EXIT_SUCCESS;
}