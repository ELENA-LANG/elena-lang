//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "config.h"
#include "ecvconst.h"
#include "ecviewer.h"

using namespace elena_lang;

constexpr auto DEFAULT_CONFIG       = "/etc/elena/templates/lib60.config";

constexpr auto PLATFORM_CATEGORY    = "configuration/platform";
constexpr auto LIB_PATH             = "project/libpath";

#if defined(__x86_64__)

constexpr auto PLATFORM_KEY = "Linux_AMD64";

#elif defined(__i386__)

constexpr auto PLATFORM_KEY = "Linux_I386";

#elif defined(__PPC64__)

constexpr auto PLATFORM_KEY = "Linux_PPC64le";

#elif defined(__aarch64__)

constexpr auto PLATFORM_KEY = "Linux_ARM64";

#endif

class ConsoleHelper : public ConsoleHelperBase
{
   TextFileWriter* _writer;

public:
   void readLine(char* buffer, size_t length) override
   {
      fgets(buffer, LINE_LEN, stdin);
   }

   void print(ustr_t message) override
   {
      if (_writer)
         _writer->writeText(message);

      printf("%s", message.str());
   }

   void print(ustr_t message, ustr_t arg) override
   {
      if (_writer) {
         char tmp[0x200];
         int len = sprintf(tmp, message.str(), arg.str());

         _writer->write(tmp, len);
      }

      printf(message.str(), arg.str());
   }

   void printPath(ustr_t message, path_t arg) override
   {
      printf(message.str(), arg.str());
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

int main(int argc, char* argv[])
{
   printf("ELENA command line ByteCode Viewer %d.%d.%d (C)2011-2022 by Aleksey Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ECV_REVISION_NUMBER);

   // prepare library provider
   LibraryProvider provider;

   PathString configPath(DEFAULT_CONFIG);

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

      PathString libPath(path.str());
      provider.setRootPath(*libPath);
   }

   ConsoleHelper consoleHelper;
   ByteCodeViewer viewer(&provider, &consoleHelper, 30);

   if (argc < 2) {
      consoleHelper.print("ecv <module name> | ecv -p<module path>");
      return 0;
   }

   if (ustr_t(argv[1]).endsWith(".nl")) {
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
