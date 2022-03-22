//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                              (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <windows.h>

#include "ecvconst.h"
#include "ecviewer.h"

using namespace elena_lang;

class Presenter : public PresenterBase
{
public:
   void readLine(char* buffer, size_t length) override
   {
      // !! fgets is used instead of fgetws, because there is a strange bug in fgetws implementation
      fgets(buffer, LINE_LEN, stdin);
   }

   void print(ustr_t message) override
   {
      WideMessage wmssg(message);
      wprintf(L"%s", wmssg.str());
   }

   void print(ustr_t message, ustr_t arg) override
   {
      WideMessage wmssg(message);
      WideMessage warg(arg);

      wprintf(wmssg.str(), warg.str());
   }

   void printPath(ustr_t message, path_t arg) override
   {
      WideMessage wmssg(message);
      wprintf(wmssg.str(), arg.str());
   }
};

int main()
{
   printf("ELENA command line ByteCode Viewer %d.%d.%d (C)2011-2022 by Aleksey Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ECV_REVISION_NUMBER);

   // Reading command-line arguments...
   int argc;
   wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

   Presenter presenter;
   ByteCodeViewer viewer(&presenter);

   if (argc < 2) {
      presenter.print("ecv <module name> | ecv -p<module path>");
      return 0;
   }

   if (wstr_t(argv[1]).endsWith(L".nl")) {
      // if direct path is provided

      PathString path(argv[1]);
      if(!viewer.load(*path)) {
         presenter.printPath(ECV_MODULE_NOTLOADED, path.str());

         return -1;
      }
   }

   viewer.runSession();

   return 0;
}