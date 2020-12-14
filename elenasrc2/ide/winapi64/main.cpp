//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI32 program entry 
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winapi\winide.h"
#include "winapi\winideconst.h"
////#include <direct.h>
//#include "winapi32\wingraphic.h"
#include "winapi32\winsplitter.h"
#include "..\appwindow.h"
#include "..\settings.h"

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

inline void setOption(Model* model, const wchar_t* parameter)
{
   if (parameter[0]!='-') {
      if (_ELENA_::Path::checkExtension(parameter, _T("l"))) {
         model->defaultFiles.add(_wcsdup(parameter));
      }
      else if (_ELENA_::Path::checkExtension(parameter, _T("prj"))) {
         model->defaultProject.copy(parameter);
      }
   }
   else if (text_str(parameter).compare(_T("-sclassic"))) {
      model->scheme = 1;
   }
}

void loadCommandLine(Model* model, char* cmdLine, _ELENA_::Path& configPath)
{
   wchar_t* cmdWLine = (wchar_t*)malloc((strlen(cmdLine) + 1) * 2);

   size_t len = strlen(cmdLine);
   _ELENA_::Convertor::copy(cmdWLine, cmdLine, len, len);
   cmdWLine[len] = 0;

   size_t start = 0;
   for (size_t i = 1 ; i <= wcslen(cmdWLine) ; i++) {
      if (cmdWLine[i]==' ' || cmdWLine[i]==0) {
         _ELENA_::Path parameter(cmdWLine + start, i - start);

         // check if a custom config file should be loaded
         if (text_str(parameter).compare(CMD_CONFIG_PATH, _ELENA_::getlength(CMD_CONFIG_PATH))) {
            configPath.copy(model->paths.appPath.c_str());
            configPath.combine(parameter + _ELENA_::getlength(CMD_CONFIG_PATH));
         }
         else setOption(model, parameter);         

         start = i + 1;
      }
   }

   _ELENA_::freestr(cmdWLine);
}

// --- loadSettings ---

void loadSettings(_ELENA_::path_t path, Model* model, IDEWindow* view)
{
   _ELENA_::XmlConfigFile file;

   if (file.load(path, _ELENA_::feUTF8)) {
      Settings::load(model, file);

      view->loadHistory(file, RECENTFILES_SECTION, RECENTRPOJECTS_SECTION);

      view->reloadSettings();
   }
}

// --- saveSettings ---

void saveSettings(_ELENA_::path_t path, Model* model, IDEWindow* view)
{
   _ELENA_::XmlConfigFile file;

   Settings::save(model, file);

   view->saveHistory(file, RECENTFILES_SECTION_NAME, RECENTRPOJECTS_SECTION_NAME);

   file.save(path, _ELENA_::feUTF8, false);
}

// --- WinMain ---

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE, LPSTR cmdLine, int)
{
   Model         model;

   // get app path
   wchar_t appPath[MAX_PATH];
   ::GetModuleFileName(NULL, appPath, MAX_PATH);
   ::PathRemoveFileSpec(appPath);

   // get default path
   wchar_t defPath[MAX_PATH];
   _wgetcwd(defPath, MAX_PATH);

   // init paths & settings
   Paths::init(&model, appPath, defPath);
   Settings::init(&model, _T("..\\src50"), _T("..\\lib50_64"));

   _ELENA_::Path configPath(model.paths.appPath);
   configPath.combine(_T("ide64.cfg"));

   // load command line arguments
   loadCommandLine(&model, cmdLine, configPath);

   // init IDE Controls
   initControls(hInstance);

   IDEController ide;
   IDEWindow     view(hInstance, _T("IDE x64"), &ide, &model);

   // init IDE settings
   loadSettings(configPath.c_str(), &model, &view);

   // start IDE
   ide.start(&view, &view, &model);

   // main program loop
   MSG msg;
   msg.wParam = 0;
   Menu* menu = view.getMenu();
   HWND hwnd = view.getHandle();;
   while (::GetMessage(&msg, NULL, 0, 0)) {
      if (!menu->_translate(hwnd, &msg)) {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }
   saveSettings(configPath.c_str(), &model, &view);
   Font::releaseFontCache();

   return 0;
}

