//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                              (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <windows.h>

#include "config.h"
#include "ecvconst.h"
#include "ecviewer.h"
#include "windows/presenter.h"

using namespace elena_lang;

constexpr auto PLATFORM_CATEGORY = "configuration/platform";
constexpr auto LIB_PATH = "project/libpath";

#ifdef _M_IX86

constexpr auto PLATFORM_KEY = "Win_x86";

#elif _M_X64

constexpr auto PLATFORM_KEY = "Win_x64";

#endif

class ConsoleHelper : public ConsoleHelperBase
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

   ConsoleHelper()
   {
      _writer = nullptr;
   }
   ~ConsoleHelper() override
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

void getAppPath(PathString& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(NULL, path, MAX_PATH);

   appPath.copySubPath(path, false);
   appPath.lower();
}

int main()
{
   printf(ECV_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ECV_REVISION_NUMBER);

   // Reading command-line arguments...
   int argc;
   wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

   int columns = 0, rows = 30;
   getConsoleSize(columns, rows);

   // prepare library provider
   LibraryProvider provider;

   PathString configPath;
   getAppPath(configPath);
   configPath.combine("templates\\lib60.cfg");

   ConfigFile config;
   if (config.load(*configPath, FileEncoding::UTF8)) {
      // select platform configuration
      ustr_t key = PLATFORM_KEY;
      ConfigFile::Node platformRoot = config.selectNode<ustr_t>(PLATFORM_CATEGORY, key, [](ustr_t key, ConfigFile::Node& node)
         {
            return node.compareAttribute("key", key);
         });

      auto configNode = config.selectNode(platformRoot, LIB_PATH);
      DynamicString<char> path;
      configNode.readContent(path);

      PathString libPath;
      getAppPath(libPath);
      libPath.combine("templates");
      libPath.combine(path.str());
      provider.setRootPath(*libPath);
   }

   ConsoleHelper consoleHelper;
   ByteCodeViewer viewer(&provider, &consoleHelper, rows);

   if (argc < 2) {
      consoleHelper.print("ecv-cli <module name> | ecv-cli <module path>");
      return 0;
   }

   if (wstr_t(argv[1]).endsWith(L".nl")) {
      // if direct path is provided

      PathString path(argv[1]);
      if(!viewer.load(*path)) {
         consoleHelper.printPath(ECV_MODULE_NOTLOADED, path.str());

         return -1;
      }
   }
   else {
      IdentifierString arg(argv[1]);
      if (!viewer.loadByName(*arg)) {
         consoleHelper.printPath(ECV_MODULE_NOTLOADED, argv[1]);

         return -1;
      }
   }

   viewer.runSession();

   return 0;
}