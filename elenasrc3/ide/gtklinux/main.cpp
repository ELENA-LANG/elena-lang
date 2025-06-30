//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ program entry
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtkcommon.h"
#include "factory.h"

using namespace elena_lang;

#if defined(__x86_64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86_64;

#elif defined(__i386__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86;

#elif defined(__PPC64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_PPC64le;

#elif defined(__aarch64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_ARM64;

#endif

class PathHelper : public PathHelperBase
{
public:
   void makePathRelative(PathString& path, path_t rootPath) override
   {
      size_t len = getlength(rootPath);
      if ((*path).compare(rootPath, len)) {
         PathString tempPath(path.str() + len);

         path.copy(tempPath.str());
      }
   }
};

int main(int argc, char* argv[])
{
   Gtk::Main kit(argc, argv);

   TextViewSettings textViewSettings = { EOLMode::LF, false, 3 };

   PathHelper    pathHelper;

   IDEModel      ideModel(textViewSettings);
   GUISettinngs  settings = { true };
   IDEController ideController(/*&outputProcess*/nullptr, /*&vmConsoleProcess*/nullptr, /*&debugProcess*/nullptr, &ideModel,
                        CURRENT_PLATFORM, &pathHelper, /*compareFileModifiedTime*/nullptr);
   IDEFactory    factory(&ideModel, &ideController, settings);

   GUIApp* app = factory.createApp();
   GUIControlBase* ideWindow = factory.createMainWindow(app, /*&outputProcess*/nullptr, /*&vmConsoleProcess*/nullptr);

   ideController.setNotifier(app);

   int retVal = app->run(ideWindow, false, nullptr); // !! temporal

   delete app;

   return retVal;
}
