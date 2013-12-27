//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI32 program entry 
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winide.h"
#include <direct.h>

#include "Shlwapi.h"

// --- command line arguments ---
#define CMD_CONFIG_PATH    _T("-c")

using namespace _GUI_;

void initCommonControls()
{
   INITCOMMONCONTROLSEX icex;
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC  = ICC_WIN95_CLASSES|ICC_COOL_CLASSES|ICC_BAR_CLASSES|ICC_USEREX_CLASSES|
	   ICC_TAB_CLASSES|ICC_LISTVIEW_CLASSES;

   ::InitCommonControlsEx(&icex);
}

void registerControlClasses(HINSTANCE hInstance)
{
   SDIWindow :: _registerClass(hInstance, ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)), MAKEINTRESOURCE(IDR_MAIN_MENU));
   TextView :: _registerClass(hInstance);
   Splitter::_registerClass(hInstance, true);
   Splitter::_registerClass(hInstance, false);
}

// --- InitControls ---

void initControls(HINSTANCE instance)
{
   registerControlClasses(instance);
   initCommonControls();
}
// --- loadCommandLine ---

inline void setOption(const wchar_t* parameter)
{
   if (parameter[0]!='-') {
      if (_ELENA_::Path::checkExtension(parameter, _T("l"))) {
         Settings::defaultFiles.add(wcsdup(parameter));
      }
      else if (_ELENA_::Path::checkExtension(parameter, _T("prj"))) {
         Settings::defaultProject.copy(parameter);
      }
   }
   else if (_ELENA_::StringHelper::compare(parameter, _T("-test"))) {
      Settings::testMode = true;
   }
   else if (_ELENA_::StringHelper::compare(parameter, _T("-sclassic"))) {
      Settings::scheme = 1;
   }
}

void loadCommandLine(char* cmdLine, _ELENA_::Path& configPath)
{
   wchar_t* cmdWLine = (wchar_t*)malloc((strlen(cmdLine) + 1) * 2);

   size_t len = strlen(cmdLine);
   _ELENA_::StringHelper::copy(cmdWLine, cmdLine, len);
   cmdWLine[len] = 0;

   int start = 0;
   for (size_t i = 1 ; i <= wcslen(cmdWLine) ; i++) {
      if (cmdWLine[i]==' ' || cmdWLine[i]==0) {
         _ELENA_::Path parameter(cmdWLine + start, i - start);

         // check if a custom config file should be loaded
         if (_ELENA_::StringHelper::compare(parameter, CMD_CONFIG_PATH, _ELENA_::getlength(CMD_CONFIG_PATH))) {
            configPath.copy(Paths::appPath);
            configPath.combine(parameter + _ELENA_::getlength(CMD_CONFIG_PATH));
         }
         else setOption(parameter);         

         start = i + 1;
      }
   }

   _ELENA_::freestr(cmdWLine);
}

// --- loadSettings ---

void loadSettings(const _path_t* path, IDE& appWindow)
{
   _ELENA_::IniConfigFile file;

   if (file.load(path, _ELENA_::feUTF8)) {
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

   file.save(path, _ELENA_::feUTF8);
}

// --- WinMain ---

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE, LPSTR cmdLine, int)
{
   // get app path
   wchar_t appPath[MAX_PATH];
   ::GetModuleFileName(NULL, appPath, MAX_PATH);
   ::PathRemoveFileSpec(appPath);

   // get default path
   wchar_t defPath[MAX_PATH];
   _wgetcwd(defPath, MAX_PATH);

   // init paths & settings
   Paths::init(appPath, defPath);
   Settings::init(_T("..\\src30"), _T("..\\lib30"));

   _ELENA_::Path configPath(Paths::appPath, _T("ide.cfg"));

   // load command line argiments
   loadCommandLine(cmdLine, configPath);

   // init IDE Controls
   initControls(hInstance);

   Win32AppDebugController controller;
   WIN32IDE ide(hInstance, &controller);

   controller.assign(ide.getAppWindow());

   // init IDE settings
   loadSettings(configPath, ide);

   // set test mode if necessary
   if (Settings::testMode) {
      controller.setTestMode(Settings::testMode);
   }

   // start IDE
   ide.start(_GUI_::Settings::appMaximized);

   // main program loop
   MSG msg;
   msg.wParam = 0;
   Menu* menu = ide.getMainMenu();
   HWND hwnd = ide.getAppWindow()->getHandle();;
   while (::GetMessage(&msg, NULL, 0, 0)) {
      if (!menu->_translate(hwnd, &msg)) {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }
   saveSettings(configPath, ide);
   Font::releaseFontCache();

   return 0;
}
