//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI32 program entry 
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "factory.h"
#include "framework.h"
#include "ide.h"
#include "ideview.h"
#include "windows/wincommon.h"
#include "Resource.h"
#include "text.h"

using namespace elena_lang;

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

   Text::TabSize = 4; // !! tempooral

   GUISettinngs  settinngs = { true };
   IDEModel      ideModel;
   IDEController ideController;
   IDEFactory    factory(hInstance, nCmdShow, &ideModel, &ideController, settinngs);

   GUIApp* app = factory.createApp();
   int retVal = app->run(factory.createMainWindow());

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
