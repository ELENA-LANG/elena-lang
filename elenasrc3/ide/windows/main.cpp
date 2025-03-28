//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI32 program entry 
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "factory.h"
#include "framework.h"
#include "ideview.h"
#include "windows/wincommon.h"
#include "windows/win32controller.h"
#include "windows/win32debugprocess.h"
#include "windows/windialogs.h"
#include "text.h"

#include <shlwapi.h>

using namespace elena_lang;

#ifdef _M_IX86

constexpr auto CURRENT_PLATFORM = PlatformType::Win_x86;

constexpr auto TARGET_XPATH = "Win_x86";

#elif _M_X64

constexpr auto CURRENT_PLATFORM = PlatformType::Win_x86_64;

constexpr auto TARGET_XPATH = "Win_x64";

#endif

constexpr auto TEMPLATE_XPATH = "templates/*";

class PathHelper : public PathHelperBase
{
public:
   void makePathRelative(PathString& path, path_t rootPath) override
   {
      path_c tmpPath[MAX_PATH];

      ::PathRelativePathTo(tmpPath, rootPath, FILE_ATTRIBUTE_DIRECTORY, *path, FILE_ATTRIBUTE_NORMAL);
      if (!emptystr(tmpPath)) {
         if (path_t(tmpPath).compareSub(L".\\", 0, 2)) {
            path.copy(tmpPath + 2);
         }
         else path.copy(tmpPath);
      }
   }
};

bool compareFileModifiedTime(path_t sour, path_t dest)
{
   DateTime sourceDT = DateTime::getFileTime(sour);
   DateTime moduleDT = DateTime::getFileTime(dest);

   return sourceDT > moduleDT;
}

typedef Win32DebugProcess    DebugProcess;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);
   UNREFERENCED_PARAMETER(nCmdShow);

   PathHelper       pathHelper;

   GUISettinngs     guiSettings = { true };
   TextViewSettings textViewSettings = { EOLMode::CRLF, false, 3 };

   IDEModel          ideModel(textViewSettings);
   Win32Process      vmConsoleProcess(50);
   Win32Process      outputProcess(50);
   DebugProcess      debugProcess;
   IDEController     ideController(&outputProcess, &vmConsoleProcess, &debugProcess, &ideModel,
                        CURRENT_PLATFORM, &pathHelper, compareFileModifiedTime);
   IDEFactory        factory(hInstance, &ideModel, &ideController, guiSettings);

   ideModel.sourceViewModel.refreshSettings();

   PathString configPath(ideModel.projectModel.paths.appPath);
   configPath.combine(_T("ide60.cfg"));
   ideController.loadConfig(&ideModel, *configPath);

   PathString sysConfigPath(ideModel.projectModel.paths.appPath);
   sysConfigPath.combine(_T("elc60.cfg"));
   ideController.loadSystemConfig(&ideModel, *sysConfigPath, TEMPLATE_XPATH, TARGET_XPATH);

   GUIApp* app = factory.createApp();
   GUIControlBase* ideWindow = factory.createMainWindow(app, &outputProcess, &vmConsoleProcess);

   ideController.setNotifier(app);

   StartUpEvent startUpEvent(STATUS_NONE);
   int retVal = app->run(ideWindow, ideModel.appMaximized, &startUpEvent);

   ideController.onIDEStop(&ideModel);

   delete app;

   return retVal;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
       case WM_INITDIALOG:
           return (INT_PTR)TRUE;

       case WM_COMMAND:
           if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
           {
               EndDialog(hDlg, LOWORD(wParam));
               return (INT_PTR)TRUE;
           }
           break;
       default:
          // to make compiler happy
          break;
    }
    return (INT_PTR)FALSE;
}
