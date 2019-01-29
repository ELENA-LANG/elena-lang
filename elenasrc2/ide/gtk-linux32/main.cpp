//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ program entry
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkide.h"
////#include "gtkideconst.h"
#include "../appwindow.h"
#include "../settings.h"

using namespace _GUI_;

//#pragma GCC diagnostic ignored "-Wwrite-strings"

// --- command line arguments ---
#define CMD_CONFIG_PATH    _T("-c")

//// --- getBasePath --
//
//void getBasePath(_path_t* path)
//{
//   pid_t pid = getpid();
//
//   _ELENA_::String<char, 50> link;
//
//   link.copy("/proc/");
//   link.appendInt(pid);
//   link.append("/exe");
//
//   char proc[512];
//   int ch = readlink(link, proc, 512);
//   if (ch != -1) {
//      proc[ch] = 0;
//      int index = _ELENA_::StringHelper::findLast(proc, '/');
//      _ELENA_::StringHelper::copy(path, proc, index);
//      path[index] = 0;
//   }
//   else path[0] = 0;
//}

// --- loadCommandLine ---

inline void setOption(Model* model, const char* parameter)
{
   if (parameter[0]!='-') {
      if (_ELENA_::Path::checkExtension(parameter, "l")) {
         model->defaultFiles.add(_ELENA_::StrFactory::clone(parameter));
      }
      else if (_ELENA_::Path::checkExtension(parameter, "prj")) {
         model->defaultProject.copy(parameter);
      }
   }
}

void loadCommandLine(Model* model, int argc, char *argv[], _ELENA_::Path& configPath)
{
   for (int i = 1 ; i < argc ; i++) {
      if (_ELENA_::ident_t(argv[i]).compare(CMD_CONFIG_PATH, _ELENA_::getlength(CMD_CONFIG_PATH))) {
         configPath.copy(argv[i] + _ELENA_::getlength(CMD_CONFIG_PATH));
      }
      else setOption(model, argv[i]);
   }
}

// --- loadSettings ---

//void loadSettings(const _path_t* path, IDE& appWindow)
void loadSettings(_ELENA_::path_t path, Model* model, GTKIDEWindow* view)
{
   _ELENA_::IniConfigFile file;

   if (file.load(path, _ELENA_::feUTF8)) {
      Settings::load(model, file);

      // !! temporal
      //view->appWindow.loadHistory(file, RECENTFILES_SECTION, RECENTRPOJECTS_SECTION);

      //view->appWindow.reloadSettings();
   }
}

// --- saveSettings ---

void saveSettings(_ELENA_::path_t path, Model* model, GTKIDEWindow* view)
{
   _ELENA_::IniConfigFile file;

   Settings::save(model, file);

   // !! temporal
   //view->appWindow.saveHistory(file, RECENTFILES_SECTION, RECENTRPOJECTS_SECTION);

   file.save(path, _ELENA_::feUTF8);
}

// --- main ---

int main( int argc, char *argv[])
{
   Model         model;

//   // get app path
//   _path_t appPath[FILENAME_MAX];
//   getBasePath(appPath);
//
//   // get default path
//   _path_t defPath[FILENAME_MAX];
//   getcwd(defPath, FILENAME_MAX);

   Gtk::Main kit(argc, argv);

   // init paths & settings
//   Paths::init(&model, appPath, defPath);
   Settings::init(&model, "/usr/elena-lang/src/elena/src40", "/usr/lib/elena/lib40");

   _ELENA_::Path configPath("/etc/elena");
   configPath.combine(_T("ide.config"));

   // load command line argiments
   loadCommandLine(&model, argc, argv, configPath);

   IDEController ide;
   GTKIDEWindow  view("IDE", &ide, &model);

   // init IDE settings
   loadSettings(configPath.str(), &model, &view);

//   controller.assign(ide.getAppWindow());

   // start IDE
   ide.start(&view, &view, &model);

   Gtk::Main::run(view);

   saveSettings(configPath.str(), &model, &view);
//   Font::releaseFontCache();

   return 0;
}
