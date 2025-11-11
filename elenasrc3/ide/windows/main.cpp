//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI32 program entry 
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "factory.h"
#include "framework.h"
#include "ideview.h"
#include "windows/wincommon.h"
#include "windows/win32controller.h"
#include "windows/win32debugadapter.h"
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

// --- loadCommandLine ---

inline void setOption(IDEController* ideController, IDEModel* model, path_t parameter)
{
   if (parameter[0] != '-') {
      if (PathUtil::checkExtension(parameter, _T("l"))) {
         ideController->addToRecentProjects(model, parameter);
      }
      else if (PathUtil::checkExtension(parameter, _T("prj"))) {
         ideController->addToRecentProjects(model, parameter);
      }
   }
   else if (text_str(parameter).compare(_T("-sclassic"))) {
      model->sourceViewModel.schemeIndex = 1;
   }
   else if (text_str(parameter).compare(_T("-sdark"))) {
      model->sourceViewModel.schemeIndex = 2;
   }
}

void loadCommandLine(IDEController* ideController, IDEModel* model, LPWSTR cmdWLine)
{
   size_t start = 0;
   for (size_t i = 1; i <= wcslen(cmdWLine); i++) {
      if (cmdWLine[i] == ' ' || cmdWLine[i] == 0) {
         PathString parameter(cmdWLine + start, i - start);

         /*// check if a custom config file should be loaded
         if (text_str(parameter).compare(CMD_CONFIG_PATH, _ELENA_::getlength(CMD_CONFIG_PATH))) {
            configPath.copy(model->paths.appPath.c_str());
            configPath.combine(parameter + _ELENA_::getlength(CMD_CONFIG_PATH));
         }
         else*/ setOption(ideController, model, *parameter);

         start = i + 1;
      }
   }
}

typedef Win32DebugAdapter    DebugProcess;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);
   UNREFERENCED_PARAMETER(nCmdShow);

   PathHelper       pathHelper;

   GUISettinngs     guiSettings = { true, true };
   TextViewSettings textViewSettings = { EOLMode::CRLF, false, 3 };

   IDEModel          ideModel(textViewSettings);
   Win32Process      vmConsoleProcess(50);
   Win32Process      outputProcess(50);
   DebugProcess      debugProcess;
   IDEController     ideController(&outputProcess, &vmConsoleProcess, &debugProcess, &ideModel,
                        CURRENT_PLATFORM, &pathHelper, compareFileModifiedTime);

   // NOTE : it must be initialized before factory / controller
   IDEFactory::initPathSettings(&ideModel);

   ideModel.sourceViewModel.refreshSettings();

   PathString configPath(ideModel.projectModel.paths.appPath);
   configPath.combine(_T("ide60.cfg"));
   ideController.loadConfig(&ideModel, *configPath, guiSettings);

   PathString sysConfigPath(ideModel.projectModel.paths.appPath);
   sysConfigPath.combine(_T("elc60.cfg"));
   ideController.loadSystemConfig(&ideModel, *sysConfigPath, TEMPLATE_XPATH, TARGET_XPATH);

   IDEFactory        factory(hInstance, &ideModel, &ideController, guiSettings);

   GUIApp* app = factory.createApp();
   GUIControlBase* ideWindow = factory.createMainWindow(app, &outputProcess, &vmConsoleProcess);

   ideController.setNotifier(app);

   loadCommandLine(&ideController, &ideModel, lpCmdLine);

   StartUpEvent startUpEvent(STATUS_NONE);
   int retVal = app->run(ideWindow, ideModel.appMaximized, &startUpEvent);

   ideController.onIDEStop(&ideModel, guiSettings);

   delete app;

   return retVal;
}
