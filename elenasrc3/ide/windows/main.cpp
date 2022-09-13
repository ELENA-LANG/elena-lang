//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI32 program entry 
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "factory.h"
#include "framework.h"
#include "ide.h"
#include "ideview.h"
#include "windows/wincommon.h"
#include "windows/win32controller.h"
#include "windows/win32debugprocess.h"
#include "windows/windialogs.h"
#include "Resource.h"
#include "text.h"

using namespace elena_lang;

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

   Text::TabSize = 4; // !! temporal

   GUISettinngs  guiSettings = { true };
   TextViewSettings textViewSettings = { EOLMode::CRLF, false, 3 };

   IDEModel          ideModel;
   Win32Process      outputProcess(50);
   DebugProcess      debugProcess;
   IDEController     ideController(&outputProcess, &debugProcess, &ideModel, textViewSettings);
   IDEFactory        factory(hInstance, &ideModel, &ideController, guiSettings);

   PathString configPath(ideModel.projectModel.paths.appPath);
   configPath.combine(_T("ide60.cfg"));
   ideController.loadConfig(&ideModel, *configPath);

   GUIApp* app = factory.createApp();
   GUIControlBase* ideWindow = factory.createMainWindow(app, &outputProcess);

   ideController.setNotifier(app);
   ideController.init(&ideModel);

   int retVal = app->run(ideWindow, ideModel.appMaximized);

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
