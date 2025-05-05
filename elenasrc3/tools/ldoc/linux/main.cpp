//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ldoc main code
//
//                                             (C)2022-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "config.h"
#include "ldocconst.h"
#include "ldoc.h"

using namespace elena_lang;

#if defined(__FreeBSD__)

constexpr auto DEFAULT_CONFIG       = "/usr/local/etc/elena/templates/lib60.config";

#else

constexpr auto DEFAULT_CONFIG       = "/etc/elena/templates/lib60.config";

#endif

#if defined(__x86_64__)

constexpr auto PLATFORM_KEY = "Linux_AMD64";

#elif defined(__i386__)

constexpr auto PLATFORM_KEY = "Linux_I386";

#elif defined(__PPC64__)

constexpr auto PLATFORM_KEY = "Linux_PPC64le";

#elif defined(__aarch64__)

constexpr auto PLATFORM_KEY = "Linux_ARM64";

#endif

class Presenter : public PresenterBase
{
public:
   void print(ustr_t message) override
   {
      printf("%s", message.str());
   }

   void print(ustr_t message, ustr_t arg) override
   {
      printf(message.str(), arg.str());
   }

   void printPath(ustr_t message, path_t arg) override
   {
      printf(message.str(), arg.str());
   }

   Presenter()
   {
   }
   ~Presenter() override
   {
   }
};

int main(int argc, char* argv[])
{
   printf(LDOC_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, LDOC_REVISION_NUMBER);

   if (argc != 2) {
      printf("ldoc {<module> | <path>}\n");
      return -1;
   }

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

   Presenter presenter;
   DocGenerator generator(&provider, &presenter);

   if (ustr_t(argv[1]).endsWith(".nl")) {
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
