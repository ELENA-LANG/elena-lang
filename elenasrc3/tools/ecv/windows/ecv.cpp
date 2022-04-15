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
   TextFileWriter* _writer;

public:
   void readLine(char* buffer, size_t length) override
   {
      // !! fgets is used instead of fgetws, because there is a strange bug in fgetws implementation
      fgets(buffer, LINE_LEN, stdin);
   }

   void print(ustr_t message) override
   {
      if (_writer)
         _writer->writeText(message);

      WideMessage wmssg(message);
      wprintf(L"%s", wmssg.str());
   }

   void print(ustr_t message, ustr_t arg) override
   {
      if (_writer) {
         char tmp[0x200];
         int len = sprintf(tmp, message.str(), arg.str());

         _writer->write(tmp, len);
      }

      WideMessage wmssg(message);
      WideMessage warg(arg);

      wprintf(wmssg.str(), warg.str());
   }

   void printPath(ustr_t message, path_t arg) override
   {
      WideMessage wmssg(message);
      wprintf(wmssg.str(), arg.str());
   }

   void setOutputMode(ustr_t arg) override
   {
      if (_writer)
         freeobj(_writer);

      PathString path(arg);

      _writer = new TextFileWriter(*path, FileEncoding::UTF8, false);
   }

   Presenter()
   {
      _writer = nullptr;
   }
   ~Presenter() override
   {
      freeobj(_writer);
   }
};

bool getConsoleSize(int& columns, int& rows)
{
   CONSOLE_SCREEN_BUFFER_INFO csbi;

   if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
      columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
      rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

      return true;
   }
   else return false;
}

int main()
{
   printf("ELENA command line ByteCode Viewer %d.%d.%d (C)2011-2022 by Aleksey Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ECV_REVISION_NUMBER);

   // Reading command-line arguments...
   int argc;
   wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

   int columns = 0, rows = 30;
   getConsoleSize(columns, rows);

   Presenter presenter;
   ByteCodeViewer viewer(&presenter, rows);

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