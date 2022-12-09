//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ldoc main code
//
//                                              (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <windows.h>

#include "config.h"
#include "ldocconst.h"
#include "ldoc.h"

using namespace elena_lang;

constexpr auto DEFAULT_CONFIG       = "templates\\lib60.cfg";

#ifdef _M_IX86

constexpr auto PLATFORM_KEY = "Win_x86";

#elif _M_X64

constexpr auto PLATFORM_KEY = "Win_x64";

#endif

class Presenter : public PresenterBase
{
public:
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

   Presenter()
   {
   }
   ~Presenter() override
   {
   }
};

void getAppPath(PathString& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(NULL, path, MAX_PATH);

   appPath.copySubPath(path, false);
   appPath.lower();
}

int main()
{
   printf(LDOC_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, LDOC_REVISION_NUMBER);

   // Reading command-line arguments...
   int argc;
   wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
   
   if (argc != 2) {
      printf("ldoc {<module> | <path>}\n");
      return -1;
   }

   // prepare library provider
   LibraryProvider provider;

   PathString configPath;
   getAppPath(configPath);
   configPath.combine(DEFAULT_CONFIG);

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

   Presenter presenter;
   DocGenerator generator(&provider, &presenter);

   if (wstr_t(argv[1]).endsWith(L".nl")) {
      // if direct path is provided

      PathString path(argv[1]);
      if(!generator.load(*path)) {
         presenter.printPath(LDOC_MODULE_NOTLOADED, path.str());

         return -1;
      }
   }
   else {
      IdentifierString arg(argv[1]);
      if (!generator.loadByName(*arg)) {
         presenter.printPath(LDOC_MODULE_NOTLOADED, argv[1]);

         return -1;
      }
   }

   generator.generate();

   return 0;
}