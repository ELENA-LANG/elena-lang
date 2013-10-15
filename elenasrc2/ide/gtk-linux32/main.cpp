//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ program entry
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtk-linux32/gtkcommon.h"
//#include "gtk-linux32/gtksdi.h"
//#include "gtk-linux32/gtkmenu.h"
//#include "gtk-linux32/gtktoolbar.h"
//#include "gtk-linux32/gtkstatusbar.h"
//#include "gtkeditframe.h"

#include "gtkide.h"
//#include "gtkideconst.h"
//#include "settings.h"

#include <unistd.h>

using namespace _GUI_;

#pragma GCC diagnostic ignored "-Wwrite-strings"

// --- command line arguments ---
#define CMD_CONFIG_PATH    _T("-c")

// --- getBasePath --

void getBasePath(_path_t* path)
{
   pid_t pid = getpid();

   _ELENA_::String<char, 50> link;

   link.copy("/proc/");
   link.appendInt(pid);
   link.append("/exe");

   char proc[512];
   int ch = readlink(link, proc, 512);
   if (ch != -1) {
      proc[ch] = 0;
      int index = _ELENA_::StringHelper::findLast(proc, '/');
      _ELENA_::StringHelper::copy(path, proc, index);
      path[index] = 0;
   }
   else path[0] = 0;
}

// --- loadCommandLine ---

inline void setOption(const _text_t* parameter)
{
   if (parameter[0]!='-') {
      if (_ELENA_::Path::checkExtension(parameter, _T("l"))) {
         _path_t* dup = NULL;
         Settings::defaultFiles.add(_ELENA_::StringHelper::clone(dup, parameter));
      }
      else if (_ELENA_::Path::checkExtension(parameter, _T("prj"))) {
         Settings::defaultProject.copy(parameter);
      }
   }
}

void loadCommandLine(int argc, char *argv[], _ELENA_::Path& configPath)
{
   for (int i = 1 ; i < argc ; i++) {
      if (_ELENA_::StringHelper::compare(argv[i], CMD_CONFIG_PATH, _ELENA_::getlength(CMD_CONFIG_PATH))) {
         configPath.copy(Paths::appPath);
         configPath.combine(argv[i] + _ELENA_::getlength(CMD_CONFIG_PATH));
      }
      else setOption(argv[i]);
   }
}

// --- loadSettings ---

void loadSettings(const _path_t* path, IDE& appWindow)
{
   _ELENA_::IniConfigFile file;

   int encoding = Settings::defaultEncoding;
   if (encoding == _ELENA_::feUTF16)   // HOTFIX: it is not possible to open Ansi files if Unicode is a default encoding
      encoding = _ELENA_::feUTF8;

   if (file.load(path, encoding)) {
      Settings::load(file);

      appWindow.loadRecentFiles(file);
      appWindow.loadRecentProjects(file);
   }
}

// --- saveSettings ---

void saveSettings(const _path_t* path, IDE& appWindow)
{
   _ELENA_::IniConfigFile file;

   Settings::save(file);
   appWindow.saveRecentFiles(file);
   appWindow.saveRecentProjects(file);

   file.save(path);
}

// --- main ---

int main( int argc, char *argv[])
{
   // get app path
   _path_t appPath[FILENAME_MAX];
   getBasePath(appPath);

   // get default path
   _path_t defPath[FILENAME_MAX];
   getcwd(defPath, FILENAME_MAX);

   Paths::init(appPath, defPath);
   Settings::init(_T("../src27"), _T("../lib27"));

   _ELENA_::Path configPath(Paths::appPath, _T("ide.cfg"));

   Gtk::Main kit(argc, argv);

   // load command line argiments
   loadCommandLine(argc, argv, configPath);

   //Linux32AppDebugController controller;
   GTKIDE ide/*(&controller)*/;

   // init IDE settings
   loadSettings(configPath, ide);

   ide.start(_GUI_::Settings::appMaximized);

   saveSettings(configPath, ide);
//   Font::releaseFontCache();

   return 0;
}
